#include "Level_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <algorithm>

Level_window::Level_window(SDL_Renderer* p_rnd) :
	m_current_level{ 1 }, m_current_gp{ 1 }, m_cam_x{ 0 },
	m_ui_show_grid{ false }, m_ui_animate{ true }
{
	m_texture = SDL_CreateTexture(p_rnd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 60 * 16, 24 * 16);
	auto l_bytes = klib::file::read_file_as_bytes("./gamedata/LEVELS.DAT");

	for (std::size_t i{ 0 }; i < l_bytes.size(); i += 1536)
		m_levels.push_back(SP_Level(std::vector<byte>(begin(l_bytes) + i, begin(l_bytes) + i + 1536)));
}

void Level_window::move(int p_delta_ms, const klib::User_input& p_input) {
	bool l_shift = p_input.is_shift_pressed();
	bool l_ctrl = p_input.is_ctrl_pressed();

	if (p_input.mw_down()) {
		if (l_shift && get_current_level_idx() > 0)
			--m_current_level;
		else if (!l_shift && m_cam_x > 0)
			--m_cam_x;
	}
	else if (p_input.mw_up()) {
		if (l_shift && get_current_level_idx() < m_levels.size() - 1)
			++m_current_level;
		else if (!l_shift && m_cam_x < 40)
			++m_cam_x;
	}
}

void Level_window::draw(SDL_Renderer* p_rnd, const Project_gfx& p_gfx, int p_w, int p_h) {
	regenerate_texture(p_rnd, p_gfx);

	// get tile pixel width
	int l_tpw = get_tile_pixel_w(p_h);
	// get screen tile widht
	int l_stw = p_w / l_tpw + 1;
	// get rest tile count
	int l_stw_tc = std::min<int>(60 - m_cam_x, l_stw);

	klib::gfx::blit_full_spec(p_rnd, m_texture,
		0, 0, l_stw_tc * l_tpw, p_h,
		16 * static_cast<int>(m_cam_x), 0, 16 * l_stw_tc, 16 * 24);
}

void Level_window::regenerate_texture(SDL_Renderer* p_rnd, const Project_gfx& p_gfx) {
	SDL_SetRenderTarget(p_rnd, m_texture);
	SDL_SetRenderDrawColor(p_rnd, 0, 0, 0, 0);
	SDL_RenderClear(p_rnd);

	for (int i = 0; i < 60; ++i)
		for (int j = 0; j < 24; ++j)
			klib::gfx::blit(p_rnd, p_gfx.get_static(m_levels.at(get_current_level_idx()).get_tile_no(i, j)), i * 16, j * 16);

	auto l_spos = m_levels.at(get_current_level_idx()).get_start_pos();
	klib::gfx::blit(p_rnd, p_gfx.get_static(3), 16 * l_spos.first, 16 * l_spos.second);

	SDL_SetRenderTarget(p_rnd, nullptr);
}


void Level_window::draw_ui(void) {
	std::string l_clvl{ std::to_string(m_current_level) };

	std::string m_lvl_label{ "Levels (" + std::to_string(m_levels.size()) + ")###levels" };
	ImGui::Begin(m_lvl_label.c_str());

	// curren level number
	ImGui::SliderInt("Level", &m_current_level, 1, static_cast<int>(m_levels.size()));

	ImGui::Separator();
	ImGui::Text("Level Metadata");

	// title
	std::string l_title_id{ "Title###title" + l_clvl };
	char l_lvl_title[23]{};
	strncpy_s(l_lvl_title, m_levels.at(get_current_level_idx()).get_title().c_str(), sizeof(l_lvl_title) - 1);
	if (ImGui::InputText(l_title_id.c_str(), l_lvl_title, 23))
		m_levels.at(get_current_level_idx()).set_title(std::string(l_lvl_title));

	// solve infotron count
	std::string l_solve_it_id{ "Infotrons Required (0=all)###it" + l_clvl };
	int l_solve_it_cnt = m_levels.at(get_current_level_idx()).get_solve_it_count();
	if (ImGui::SliderInt(l_solve_it_id.c_str(), &l_solve_it_cnt, 0, 255))
		m_levels.at(get_current_level_idx()).set_solve_it_count(l_solve_it_cnt);

	// gravity
	std::string l_grav_id{ "Gravity###grav" + l_clvl };
	bool l_grav = m_levels.at(get_current_level_idx()).get_gravity();
	if (ImGui::Checkbox(l_grav_id.c_str(), &l_grav))
		m_levels.at(get_current_level_idx()).set_gravity(l_grav);

	ImGui::SameLine();

	// freeze zonks
	std::string l_fz_id{ "Freeze Zonks###fz" + l_clvl };
	bool l_fz = m_levels.at(get_current_level_idx()).get_freeze_zonks();
	if (ImGui::Checkbox(l_fz_id.c_str(), &l_fz))
		m_levels.at(get_current_level_idx()).set_freeze_zonks(l_fz);

	// gravity ports
	ImGui::Separator();
	int l_gp_count = m_levels.at(get_current_level_idx()).get_gravity_port_count();
	std::string l_gp_text{ "Gravity Ports (" + std::to_string(l_gp_count) + ")" };

	ImGui::Text(l_gp_text.c_str());

	if (m_levels.at(get_current_level_idx()).get_gravity_port_count() > 0) {
		m_current_gp = std::min(m_current_gp, l_gp_count);
		ImGui::SliderInt("Port", &m_current_gp, 1, l_gp_count);

		// gravity port position
		int l_gp_x{ m_levels.at(get_current_level_idx()).get_gp_x(m_current_gp - 1) };
		int l_gp_y{ m_levels.at(get_current_level_idx()).get_gp_y(m_current_gp - 1) };

		// x and y position
		std::string l_gp_x_id{ "x###gpx" + l_clvl };
		if (ImGui::SliderInt(l_gp_x_id.c_str(), &l_gp_x, 1, 60))
			m_levels.at(get_current_level_idx()).set_gp_x(m_current_gp - 1, l_gp_x);
		std::string l_gp_y_id{ "y###gpy" + l_clvl };
		if (ImGui::SliderInt(l_gp_y_id.c_str(), &l_gp_y, 1, 24))
			m_levels.at(get_current_level_idx()).set_gp_y(m_current_gp - 1, l_gp_y);

		// gravity, freeze zonks and freeze enemies
		bool l_gp_grav{ m_levels.at(get_current_level_idx()).get_gp_gravity(m_current_gp - 1) };
		bool l_gp_fz{ m_levels.at(get_current_level_idx()).get_gp_freeze_zonks(m_current_gp - 1) };
		bool l_gp_fe{ m_levels.at(get_current_level_idx()).get_gp_freeze_enemies(m_current_gp - 1) };

		std::string l_gp_grav_id{ "Gravity###gpg" + l_clvl };
		std::string l_gp_fz_id{ "Freeze Zonks###gpfz" + l_clvl };
		std::string l_gp_fe_id{ "Freeze Enemies###gpfe" + l_clvl };

		if (ImGui::Checkbox(l_gp_grav_id.c_str(), &l_gp_grav))
			m_levels.at(get_current_level_idx()).set_gp_gravity(m_current_gp - 1, l_gp_grav);
		ImGui::SameLine();
		if (ImGui::Checkbox(l_gp_fz_id.c_str(), &l_gp_fz))
			m_levels.at(get_current_level_idx()).set_gp_freeze_zonks(m_current_gp - 1, l_gp_fz);
		ImGui::SameLine();
		if (ImGui::Checkbox(l_gp_fe_id.c_str(), &l_gp_fe))
			m_levels.at(get_current_level_idx()).set_gp_freeze_enemies(m_current_gp - 1, l_gp_fe);

		if (ImGui::Button("Delete Gravity Port")) {
			m_levels.at(get_current_level_idx()).delete_gravity_port(m_current_gp - 1);
		}
	}

	ImGui::Separator();

	// save to disk
	if (ImGui::Button("Save DAT")) {
		std::vector<byte> l_file_bytes;
		for (const auto& lvl : m_levels) {
			auto l_lvl_bytes = lvl.get_bytes();
			l_file_bytes.insert(end(l_file_bytes),
				begin(l_lvl_bytes), end(l_lvl_bytes));
			klib::file::write_bytes_to_file(l_file_bytes,
				"./gamedata/LEVELS.DAT");
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Save XML")) {
		save_xml(get_current_level_idx());
	}
	ImGui::SameLine();
	if (ImGui::Button("Save BMP")) {

	}

	// load from disk
	if (ImGui::Button("Load DAT")) {

	}
	ImGui::SameLine();
	if (ImGui::Button("Load XML")) {

	}

	// UI settings
	ImGui::Separator();
	ImGui::Text("UI Settings");
	ImGui::Checkbox("Show Grid", &m_ui_show_grid);
	ImGui::SameLine();
	ImGui::Checkbox("Animate", &m_ui_animate);

	ImGui::End();
}

int Level_window::get_tile_pixel_w(int p_screen_pixel_h) const {
	return static_cast<int>(static_cast<float>(p_screen_pixel_h) / 24.0f);
}

std::size_t Level_window::get_current_level_idx(void) const {
	return static_cast<std::size_t>(m_current_level) - 1;
}

#include "Level_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <algorithm>

Level_window::Level_window(SDL_Renderer* p_rnd) :
	m_current_level{ 1 }, m_cam_x{ 0 }
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

	// freeze zonks
	std::string l_fz_id{ "Freeze Zonks###fz" + l_clvl };
	bool l_fz = m_levels.at(get_current_level_idx()).get_freeze_zonks();
	if (ImGui::Checkbox(l_fz_id.c_str(), &l_fz))
		m_levels.at(get_current_level_idx()).set_freeze_zonks(l_fz);

	ImGui::End();
}

int Level_window::get_tile_pixel_w(int p_screen_pixel_h) const {
	return static_cast<int>(static_cast<float>(p_screen_pixel_h) / 24.0f);
}

std::size_t Level_window::get_current_level_idx(void) const {
	return static_cast<std::size_t>(m_current_level) - 1;
}

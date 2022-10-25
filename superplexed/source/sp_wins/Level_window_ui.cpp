#include "Level_window.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include "./../common/klib/klib_file.h"

void Level_window::draw_ui(const Project_gfx& p_gfx) {
	draw_ui_level_win();
	draw_ui_tile_win(p_gfx);
	draw_ui_gp_win();
}

void Level_window::draw_ui_tile_win(const Project_gfx& p_gfx) {
	//ImGui::ImageButton((ImTextureID)(intptr_t)p_gfx.get_static(3), { 32,32 });
}

void Level_window::draw_ui_gp_win(void) {
	int l_gp_count = m_levels.at(get_current_level_idx()).get_gravity_port_count();
	std::string l_clvl{ std::to_string(m_current_level) };

	std::string l_gp_label{ "Gravity Ports (" + std::to_string(l_gp_count) + ")###gp" };
	ImGui::Begin(l_gp_label.c_str());

	if (l_gp_count > 0) {
		m_current_gp = std::min(m_current_gp, l_gp_count);
		ImGui::SliderInt("Port", &m_current_gp, 1, l_gp_count);

		// gravity port position
		int l_gp_x{ m_levels.at(get_current_level_idx()).get_gp_x(m_current_gp - 1) };
		int l_gp_y{ m_levels.at(get_current_level_idx()).get_gp_y(m_current_gp - 1) };

		// x and y position
		std::string l_gp_x_id{ "x###gpx" + l_clvl };
		if (ImGui::SliderInt(l_gp_x_id.c_str(), &l_gp_x, 0, 59))
			m_levels.at(get_current_level_idx()).set_gp_x(m_current_gp - 1, l_gp_x);
		std::string l_gp_y_id{ "y###gpy" + l_clvl };
		if (ImGui::SliderInt(l_gp_y_id.c_str(), &l_gp_y, 0, 23))
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

		if (ImGui::Button("Delete Port")) {
			m_levels.at(get_current_level_idx()).delete_gravity_port(m_current_gp - 1);
		}
	}

	if (l_gp_count < 10) {
		if (l_gp_count > 0)
			ImGui::SameLine();
		if (ImGui::Button("Add Port")) {
			m_levels.at(get_current_level_idx()).add_gravity_port(m_sel_x, m_sel_y,
				false, false, false);
		}
	}

	ImGui::End();
}

void Level_window::draw_ui_level_win(void) {
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

	ImGui::Separator();
	ImGui::Text("Level Collection Operations");

	if (ImGui::Button("Copy Level")) {
		m_levels.insert(begin(m_levels) + get_current_level_idx(),
			m_levels.at(get_current_level_idx()));
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Level")) {
		if (m_levels.size() > 1) {
			m_levels.erase(begin(m_levels) + get_current_level_idx());
			m_current_level = std::min<int>(static_cast<int>(m_levels.size()), m_current_level);
		}
	}
	ImGui::Separator();
	ImGui::Text("File Operations");
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
		for (std::size_t i{ 0 }; i < m_levels.size(); ++i)
			save_xml(i);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save BMP")) {

	}

	// load from disk
	if (ImGui::Button("Load DAT")) {

	}
	ImGui::SameLine();
	if (ImGui::Button("Load XML")) {

		for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
			try {
				auto l_lvl = load_xml(i);
				m_levels.at(i) = l_lvl;
			}
			catch (const std::exception&) {

			}
		}
	}

	// UI settings
	ImGui::Separator();
	ImGui::Text("UI Settings");
	ImGui::Checkbox("Show Grid", &m_ui_show_grid);
	ImGui::SameLine();
	ImGui::Checkbox("Animate", &m_ui_animate);

	ImGui::End();
}

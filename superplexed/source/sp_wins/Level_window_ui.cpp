#include "Level_window.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include "./../common/klib/klib_file.h"
#include "./../common/klib/klib_util.h"
#include "./../SP_Constants.h"

void Level_window::draw_ui(const Project_gfx& p_gfx, const klib::User_input& p_input, SP_Config& p_config) {
	draw_ui_level_win(p_input, p_config);
	draw_ui_tile_win(p_gfx);
	draw_ui_gp_win(p_config);
	/*
	// test code for putting the gameboard graphics into a window
	ImGui::Begin("Gameboard", nullptr, ImGuiWindowFlags_AlwaysHorizontalScrollbar);

	ImVec2 l_wmin = ImGui::GetWindowContentRegionMin();
	ImVec2 l_wmax = ImGui::GetWindowContentRegionMax();

	float l_win_height = l_wmax.y - l_wmin.y;
	float l_win_width = l_wmax.x - l_wmin.x;

	int l_tpw = std::max(get_tile_pixel_w(static_cast<int>(l_win_height)), 1);
	float l_imgpw = static_cast<float>(l_tpw * 60.0f);

	ImGui::Image((void*)(intptr_t)m_texture,
		ImVec2(l_imgpw, l_win_height));

	ImGui::End();
	*/
}

void Level_window::draw_ui_tile_win(const Project_gfx& p_gfx) {

	std::string l_sel_tile{ "Tiles - Cursor @ (" + std::to_string(m_sel_x) + "," + std::to_string(m_sel_y) + ")###tiles" };
	ImGui::Begin(l_sel_tile.c_str());

	float l_icon_w = m_tile_picker_scale * static_cast<float>(c::TILE_W);

	std::string l_sel_tile_no{ "Selected Tile: #" + std::to_string(m_sel_tile) +
		" (" + SP_Level::get_description(m_sel_tile) + ")" };
	ImGui::ImageButton((ImTextureID)(intptr_t)p_gfx.get_static(m_sel_tile), { 2.0f * l_icon_w,2.0f * l_icon_w });
	ImGui::Text(l_sel_tile_no.c_str());

	ImGui::Separator();
	ImGui::Text("Tile Picker");
	ImGui::Separator();

	for (const auto& kv : m_tile_picker) {
		ImGui::Text(kv.first.c_str());
		for (int n : kv.second) {
			bool l_is_selected{ m_sel_tile == n };

			if (l_is_selected)
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 1.0f, 1.0f, 0.0f, 1.0f });
			ImGui::PushID(n);
			if (ImGui::ImageButton((ImTextureID)(intptr_t)p_gfx.get_static(n), { l_icon_w,l_icon_w }))
				m_sel_tile = n;
			ImGui::PopID();
			if (l_is_selected)
				ImGui::PopStyleColor();

			ImGui::SameLine();
		}
		ImGui::NewLine();
	}

	ImGui::End();
}

void Level_window::draw_ui_gp_win(SP_Config& p_config) {
	int l_gp_count = m_levels.at(get_current_level_idx()).get_gravity_port_count();
	std::string l_clvl{ std::to_string(m_current_level) };

	std::string l_gp_label{ "Special Ports (" + std::to_string(l_gp_count) + ")###gp" };
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
		bool l_gp_status{ m_levels.at(get_current_level_idx()).get_gp_status(m_current_gp - 1) };

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

		ImGui::Separator();
		if (!l_gp_status)
			ImGui::Text("Warning: No Gravity Port tile at this position");
		else {
			std::string l_gp_tile{ "Tile: " + m_levels.at(get_current_level_idx()).get_description(
			m_levels.at(get_current_level_idx()).get_tile_no(l_gp_x, l_gp_y)
			) };
			ImGui::Text(l_gp_tile.c_str());
		}
		ImGui::Separator();

		if (ImGui::Button("Delete Port")) {
			m_levels.at(get_current_level_idx()).delete_gravity_port(m_current_gp - 1);
			p_config.add_message("Deleted special port #" + std::to_string(m_current_gp));
		}

	}

	if (l_gp_count < 10) {
		if (l_gp_count > 0)
			ImGui::SameLine();
		if (ImGui::Button("Add Port")) {
			m_levels.at(get_current_level_idx()).add_gravity_port(m_sel_x, m_sel_y,
				false, false, false);
			p_config.add_message("Added special port at (" + std::to_string(m_sel_x) + "," + std::to_string(m_sel_y) + ")");
		}
	}

	ImGui::End();
}

void Level_window::draw_ui_level_win(const klib::User_input& p_input, SP_Config& p_config) {
	std::string l_clvl{ std::to_string(m_current_level) };

	std::string m_lvl_label{ "Levels (" + l_clvl + " of " + std::to_string(m_levels.size()) + ")###levels" };
	ImGui::Begin(m_lvl_label.c_str());

	// current level number
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
	std::string l_solve_it_id{ "#Infotrons###it" + l_clvl };
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
		p_config.add_message("Made a copy of level #" + l_clvl);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete Level")) {
		if (m_levels.size() > 1) {
			m_levels.erase(begin(m_levels) + get_current_level_idx());
			p_config.add_message("Deleted level #" + l_clvl);
			m_current_level = std::min<int>(static_cast<int>(m_levels.size()), m_current_level);
		}
	}
	ImGui::Separator();
	ImGui::Text("File Operations");
	// save to disk
	if (ImGui::Button("Save DAT")) {
		// generate LEVELS.DAT and LEVEL.LST
		std::vector<byte> l_file_bytes;
		std::vector<byte> l_list_file_bytes;

		for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
			auto l_lvl_bytes = m_levels[i].get_bytes();
			l_file_bytes.insert(end(l_file_bytes),
				begin(l_lvl_bytes), end(l_lvl_bytes));

			std::string l_line = klib::util::stringnum(i + 1) + ' ' + m_levels[i].get_title();

			l_list_file_bytes.insert(end(l_list_file_bytes),
				begin(l_line), end(l_line));
			l_list_file_bytes.push_back(13);
			l_list_file_bytes.push_back(10);
		}

		klib::file::write_bytes_to_file(l_file_bytes,
			p_config.get_levels_dat_full_path());
		klib::file::write_bytes_to_file(l_list_file_bytes,
			p_config.get_level_lst_full_path());
	}
	ImGui::SameLine();
	if (ImGui::Button("Save XML")) {
		for (std::size_t i{ 0 }; i < m_levels.size(); ++i)
			save_xml(i, p_config);
	}
	ImGui::SameLine();
	if (ImGui::Button("Save SP")) {
		for (std::size_t i{ 0 }; i < m_levels.size(); ++i)
			save_sp(i, p_config);
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
				auto l_lvl = load_xml(i, p_config);
				m_levels.at(i) = l_lvl;
			}
			catch (const std::exception&) {

			}
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Load SP")) {

		for (std::size_t i{ 0 }; i < m_levels.size(); ++i) {
			try {
				auto l_lvl = load_sp(i, p_config);
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
	ImGui::SliderFloat("Icon Scale", &m_tile_picker_scale, 0.5f, 4.0f);

	ImGui::End();
}

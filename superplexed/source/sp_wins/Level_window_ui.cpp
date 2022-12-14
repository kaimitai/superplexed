// Visual Studio warnings
#ifdef _MSC_VER
#pragma warning (disable: 4996)     // 'This function or variable may be unsafe': strcpy, strdup, sprintf, vsnprintf, sscanf, fopen
#endif

#include "Level_window.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include "./../common/klib/klib_file.h"
#include "./../common/klib/klib_util.h"
#include "./../SP_Constants.h"

// forward declare as much of Main_window as we need
class Main_window {
public:
	static void window_start(const std::string& p_title, c::SP_Color p_text, c::SP_Color p_active, c::SP_Color p_inactive, c::SP_Color p_collapsed);
};

void Level_window::draw_ui(const Project_gfx& p_gfx, const klib::User_input& p_input, SP_Config& p_config) {
	draw_ui_level_win(p_input, p_gfx, p_config);
	draw_ui_tile_win(p_input, p_config, p_gfx);
	draw_ui_gp_win(p_gfx, p_config);
	if (m_show_stats != -1)
		draw_ui_statistics(p_gfx);
}

void Level_window::draw_ui_tile_win(const klib::User_input& p_input, SP_Config& p_config, const Project_gfx& p_gfx) {
	std::string l_sel_tile{ "Tiles - Cursor @ (" + std::to_string(m_sel_x) + "," + std::to_string(m_sel_y) + ")###tiles" };

	ImGui::SetNextWindowPos(ImVec2(c::WIN_TP_X, c::WIN_TP_Y), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(c::WIN_TP_W, c::WIN_TP_H), ImGuiCond_FirstUseEver);

	Main_window::window_start(l_sel_tile, c::COL_BLACK, c::COL_GREEN, c::COL_GREEN_LIGHT, c::COL_GREEN_LIGHT);

	float l_icon_w = m_tile_picker_scale * static_cast<float>(c::TILE_W);

	std::string l_sel_tile_no{ "Selected Tile: #" + std::to_string(m_sel_tile) +
		" (" + SP_Level::get_description(m_sel_tile) + ")" };
	ImGui::ImageButton(p_gfx.get_tile_texture(m_sel_tile), { 2.0f * l_icon_w,2.0f * l_icon_w });
	ImGui::SameLine();
	ImGui::Checkbox("Flash", &m_ui_flash);
	ImGui::SameLine();
	ImGui::Checkbox("Grid", &m_ui_show_grid);
	ImGui::SameLine();
	ImGui::Checkbox("A-Tiles", &m_ui_animate);
	ImGui::SameLine();
	ImGui::Checkbox("A-Ports", &m_ui_animate_ports);

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

			if (ImGui::ImageButton(p_gfx.get_tile_texture(n), { l_icon_w,l_icon_w })) {
				m_sel_tile = n;
			}

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
				ImGui::SetTooltip(SP_Level::get_description(n).c_str());

			ImGui::PopID();
			if (l_is_selected)
				ImGui::PopStyleColor();

			ImGui::SameLine();
		}
		ImGui::NewLine();
	}
	ImGui::Separator();
	if (ImGui::Button("Count")) {
		bool l_all = p_input.is_shift_pressed();
		auto l_tile_counts = get_tile_counts(l_all);
		p_config.add_message("Tile count for #" + std::to_string(m_sel_tile) +
			(l_all ? " (all levels" : " (level " + std::to_string(m_current_level)) +
			"): " + std::to_string(l_tile_counts.at(m_sel_tile))
		);
	}
	ImGui::SameLine();
	if (m_show_stats == -1 && ImGui::Button("Statistics")) {
		bool l_all = p_input.is_shift_pressed();
		m_statistics = get_tile_counts(l_all);
		m_show_stats = l_all ? 0 : m_current_level;
	}

	ImGui::Separator();
	ImGui::SliderFloat("Icon Scale", &m_tile_picker_scale, 0.5f, 4.0f);

	ImGui::End();
}

void Level_window::draw_ui_gp_win(const Project_gfx& p_gfx, SP_Config& p_config) {
	int l_gp_count = get_current_level().get_gravity_port_count();
	std::string l_clvl{ std::to_string(m_current_level) };

	std::string l_gp_label{ "Special Ports (" + std::to_string(l_gp_count) + ")###gp" };

	ImGui::SetNextWindowPos(ImVec2(c::WIN_SP_X, c::WIN_SP_Y), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(c::WIN_SP_W, c::WIN_SP_H), ImGuiCond_FirstUseEver);

	Main_window::window_start(l_gp_label, c::COL_WHITE, c::COL_RED, c::COL_RED_DARK, c::COL_RED_DARK);

	if (l_gp_count > 0) {
		m_current_gp = std::min(m_current_gp, l_gp_count);
		ImGui::SliderInt("Port", &m_current_gp, 1, l_gp_count);

		// gravity port position
		int l_gp_x{ get_current_level().get_gp_x(m_current_gp - 1) };
		int l_gp_y{ get_current_level().get_gp_y(m_current_gp - 1) };

		// x and y position
		std::string l_gp_x_id{ "x###gpx" + l_clvl };
		if (ImGui::SliderInt(l_gp_x_id.c_str(), &l_gp_x, 0, 59))
			get_current_level().set_gp_x(m_current_gp - 1, l_gp_x);
		std::string l_gp_y_id{ "y###gpy" + l_clvl };
		if (ImGui::SliderInt(l_gp_y_id.c_str(), &l_gp_y, 0, 23))
			get_current_level().set_gp_y(m_current_gp - 1, l_gp_y);

		// gravity, freeze zonks and freeze enemies
		bool l_gp_grav{ get_current_level().get_gp_gravity(m_current_gp - 1) };
		bool l_gp_fz{ get_current_level().get_gp_freeze_zonks(m_current_gp - 1) };
		bool l_gp_fe{ get_current_level().get_gp_freeze_enemies(m_current_gp - 1) };
		bool l_gp_status{ get_current_level().get_gp_status(m_current_gp - 1) };

		std::string l_gp_grav_id{ "Gravity###gpg" + l_clvl };
		std::string l_gp_fz_id{ "Freeze Zonks###gpfz" + l_clvl };
		std::string l_gp_fe_id{ "Freeze Enemies###gpfe" + l_clvl };

		if (ImGui::Checkbox(l_gp_grav_id.c_str(), &l_gp_grav))
			get_current_level().set_gp_gravity(m_current_gp - 1, l_gp_grav);
		ImGui::SameLine();
		if (ImGui::Checkbox(l_gp_fz_id.c_str(), &l_gp_fz))
			get_current_level().set_gp_freeze_zonks(m_current_gp - 1, l_gp_fz);
		ImGui::SameLine();
		if (ImGui::Checkbox(l_gp_fe_id.c_str(), &l_gp_fe))
			get_current_level().set_gp_freeze_enemies(m_current_gp - 1, l_gp_fe);

		ImGui::Separator();
		if (!l_gp_status)
			ImGui::Text("Warning: No Special Port tile at this position");
		else {
			int l_tile_no = get_current_level().get_tile_no(l_gp_x, l_gp_y);
			ImGui::Image(p_gfx.get_tile_texture(l_tile_no), { c::TILE_W, c::TILE_W });
			ImGui::SameLine();
			ImGui::Text(SP_Level::get_description(l_tile_no).c_str());
		}
		ImGui::Separator();

		if (ImGui::Button("Delete Port")) {
			get_current_level().delete_gravity_port(m_current_gp - 1);
			p_config.add_message("Deleted special port #" + std::to_string(m_current_gp));
		}

	}

	if (l_gp_count < c::MAX_GP_COUNT) {
		if (l_gp_count > 0)
			ImGui::SameLine();
		if (ImGui::Button("Add Port")) {
			get_current_level().add_gravity_port(m_sel_x, m_sel_y,
				false, false, false);
			p_config.add_message("Added special port at (" + std::to_string(m_sel_x) + "," + std::to_string(m_sel_y) + ")");
		}
	}

	ImGui::End();
}

void Level_window::draw_ui_level_win(const klib::User_input& p_input, const Project_gfx& p_gfx, SP_Config& p_config) {
	bool l_shift = p_input.is_shift_pressed();
	bool l_ctrl = p_input.is_ctrl_pressed();
	bool l_is_levelset = p_config.is_levelset_file();
	std::string l_clvl{ std::to_string(m_current_level) };

	std::string m_lvl_label{ "Level" +
		(l_is_levelset ? " " + l_clvl + " of " + std::to_string(m_levels.size()) : "") +
		": \"" +
		get_current_level().get_title() + "\"###levels" };

	ImGui::SetNextWindowPos(ImVec2(c::WIN_LVL_X, c::WIN_LVL_Y), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(c::WIN_LVL_W, c::WIN_LVL_H), ImGuiCond_FirstUseEver);
	Main_window::window_start(m_lvl_label, c::COL_BLACK, c::COL_ORANGE, c::COL_ORANGE_LIGHT, c::COL_ORANGE_LIGHT);

	// current level number, if in a levelset file
	if (l_is_levelset) {
		ImGui::SliderInt("Level", &m_current_level, 1, static_cast<int>(m_levels.size()));
		ImGui::Separator();
	}

	ImGui::Text("Level Metadata");

	// title
	std::string l_title_id{ "Title###title" + l_clvl };
	char l_lvl_title[c::LENGTH_TITLE + 1]{};
	strncpy(l_lvl_title, get_current_level().get_title().c_str(), sizeof(l_lvl_title) - 1);
	if (ImGui::InputText(l_title_id.c_str(), l_lvl_title, c::LENGTH_TITLE + 1))
		get_current_level().set_title(std::string(l_lvl_title));

	// solve infotron count
	std::string l_solve_it_id{ "Infotrons###it" + l_clvl };
	int l_solve_it_cnt = get_current_level().get_solve_it_count();
	if (ImGui::SliderInt(l_solve_it_id.c_str(), &l_solve_it_cnt, 0, 255))
		get_current_level().set_solve_it_count(l_solve_it_cnt);

	std::string l_infotron_toolip{ "Infotrons: " +
		std::to_string(get_current_level().get_tile_count(c::TILE_NO_INFOTRON)) +
		", Electrons: " +
		std::to_string(get_current_level().get_tile_count(c::TILE_NO_ELECTRON))
	};

	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip(l_infotron_toolip.c_str());

	// gravity
	std::string l_grav_id{ "Gravity###grav" + l_clvl };
	bool l_grav = get_current_level().get_gravity();
	if (ImGui::Checkbox(l_grav_id.c_str(), &l_grav))
		get_current_level().set_gravity(l_grav);

	ImGui::SameLine();

	// freeze zonks
	std::string l_fz_id{ "Freeze Zonks###fz" + l_clvl };
	bool l_fz = get_current_level().get_freeze_zonks();
	if (ImGui::Checkbox(l_fz_id.c_str(), &l_fz))
		get_current_level().set_freeze_zonks(l_fz);

	ImGui::Separator();

	if (l_is_levelset) {
		ImGui::Text("Level Collection Operations");

		if (ImGui::Button("Insert Level")) {
			std::size_t l_insert_idx{ get_current_level_idx() + (l_shift ? 0 : 1) };
			m_levels.insert(begin(m_levels) + l_insert_idx,
				SP_Level());
			if (l_shift)
				++m_current_level;
			p_config.add_message("Inserted new level " +
				std::string(l_shift ? "before" : "after")
				+ " current");
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete Level") && l_ctrl) {
			if (m_levels.size() > 1) {
				m_levels.erase(begin(m_levels) + get_current_level_idx());
				p_config.add_message("Deleted level #" + l_clvl);
				m_current_level = std::min<int>(static_cast<int>(m_levels.size()), m_current_level);
			}
		}
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			ImGui::SetTooltip(c::TXT_HOLD_CTRL_TO_USE);
	}

	ImGui::Separator();
	ImGui::Text(c::TXT_FILE_OPERATIONS);
	// save to disk
	std::string l_save_dat{ "Save " + p_config.get_dat_label() };
	std::string l_load_dat{ "Load " + p_config.get_dat_label() };

	if (l_is_levelset) {
		if (ImGui::Button(l_save_dat.c_str()))
			save_levels_dat(p_config);
		ImGui::SameLine();
	}

	if (ImGui::Button(c::SAVE_XML))
		save_file(SP_Config::SP_file_type::xml, p_config, p_gfx, l_shift);
	ImGui::SameLine();
	if (ImGui::Button(c::SAVE_SP))
		save_file(SP_Config::SP_file_type::sp, p_config, p_gfx, l_shift);
	ImGui::SameLine();
	if (ImGui::Button(c::SAVE_BMP))
		save_file(SP_Config::SP_file_type::bmp, p_config, p_gfx, l_shift);

	// load from disk
	if (l_is_levelset) {
		if (ImGui::Button(l_load_dat.c_str()) && l_ctrl)
			load_levels_dat(p_config);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
			ImGui::SetTooltip(c::TXT_HOLD_CTRL_TO_USE);
		ImGui::SameLine();
	}
	if (ImGui::Button(c::LOAD_XML) && l_ctrl)
		load_file(SP_Config::SP_file_type::xml, p_config, l_shift);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip(c::TXT_HOLD_CTRL_TO_USE);
	ImGui::SameLine();
	if (ImGui::Button(c::LOAD_SP) && l_ctrl)
		load_file(SP_Config::SP_file_type::sp, p_config, l_shift);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip(c::TXT_HOLD_CTRL_TO_USE);
	ImGui::SameLine();
	if (ImGui::Button(c::PIXEL_ART)) {
		try {
			auto l_tiles = p_gfx.generate_tile_art(p_config, c::LEVEL_W - m_sel_x, c::LEVEL_H - m_sel_y);
			for (std::size_t j{ 0 }; j < l_tiles.size(); ++j)
				for (std::size_t i{ 0 }; i < l_tiles[j].size(); ++i)
					set_tile_value(m_sel_x + static_cast<int>(i), m_sel_y + static_cast<int>(j), l_tiles[j][i]);
			commit_undo_block();
			p_config.add_message("Loaded pixel art from " + p_config.get_pixel_art_bmp_full_path());
		}
		catch (const std::exception& ex) {
			p_config.add_message(ex.what());
		}
	}
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
		ImGui::SetTooltip(c::TXT_PIXEL_ART_TOOLTIP);

	ImGui::End();
}

void Level_window::draw_ui_statistics(const Project_gfx& p_gfx) {
	ImGui::SetNextWindowPos(ImVec2(c::WIN_STATS_X, c::WIN_STATS_Y), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(c::WIN_STATS_W, c::WIN_STATS_H), ImGuiCond_FirstUseEver);

	std::string l_win_title{ "Statistics for " +
		std::string(m_show_stats == 0 ? "all levels" : "level "
		+ std::to_string(m_show_stats)) + "###stats"
	};
	ImGui::Begin(l_win_title.c_str());

	for (const auto& kv : m_tile_picker) {
		ImGui::Separator();
		ImGui::Text(kv.first.c_str());
		ImGui::Separator();
		for (int tv : kv.second) {
			// filter out virtual tiles so count 0-39 only
			if (tv < c::TILE_COUNT) {
				int l_tile_count = m_statistics.at(tv);
				if (l_tile_count > 0 || m_show_stats_tc0) {
					std::string l_text = "#" + std::to_string(tv) + " (" +
						SP_Level::get_description(tv) + "): " + std::to_string(l_tile_count);
					ImGui::Image(p_gfx.get_tile_texture(tv), { c::TILE_W, c::TILE_W });
					ImGui::SameLine();
					ImGui::Text(l_text.c_str());
				}
			}
		}
	}

	ImGui::Separator();
	ImGui::Checkbox("Show all", &m_show_stats_tc0);
	if (ImGui::Button("Close"))
		m_show_stats = -1;

	ImGui::End();
}

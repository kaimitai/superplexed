#include "Savefile_window.h"
#include "./../common/klib/klib_file.h"
#include "./../common/klib/klib_util.h"

Savefile_window::Savefile_window(SP_Config& p_config) :
	m_selected_hf{ 1 }, m_selected_player{ 1 }
{
	load_hallfame_lst(p_config, true);
	load_player_lst(p_config, true);

	// initialize button labels - no need to do this continuously
	for (int i{ 1 }; i <= c::DEFAULT_LEVEL_COUNT; ++i)
		m_button_labels.push_back({ klib::util::stringnum(i) });
}

void Savefile_window::load_player_lst(SP_Config& p_config, bool p_first_load) {
	std::string l_in_file{ p_config.get_lst_full_path(c::FILENAME_PLAYER) };

	try {
		m_players = SP_Player_list(klib::file::read_file_as_bytes(l_in_file));
		if (!p_first_load)
			p_config.add_message("Loaded " + l_in_file);
	}
	catch (const std::exception& ex) {
		if (p_first_load)
			m_players = SP_Player_list();
		else
			p_config.add_message(ex.what());
	}
}

void Savefile_window::save_player_lst(SP_Config& p_config) {
	std::string l_out_file{ p_config.get_lst_full_path(c::FILENAME_PLAYER) };

	try {
		klib::file::write_bytes_to_file(m_players.to_bytes(), l_out_file);
		p_config.add_message("Saved " + l_out_file);
	}
	catch (const std::exception& ex) {
		p_config.add_message(ex.what());
	}
}

void Savefile_window::load_hallfame_lst(SP_Config& p_config, bool p_first_load) {
	std::string l_infile{ p_config.get_lst_full_path(c::FILENAME_HALLFAME) };
	try {
		m_hallfame = SP_Hallfame(klib::file::read_file_as_bytes(l_infile.c_str()));
		if (!p_first_load)
			p_config.add_message("Loaded " + l_infile);
	}
	catch (const std::exception& ex) {
		if (p_first_load)
			m_hallfame = SP_Hallfame();
		else
			p_config.add_message(ex.what());
	}
}

void Savefile_window::save_hallfame_lst(SP_Config& p_config) {
	std::string l_out_file{ p_config.get_lst_full_path(c::FILENAME_HALLFAME) };

	try {
		klib::file::write_bytes_to_file(m_hallfame.to_bytes(), l_out_file);
		p_config.add_message("Saved " + l_out_file);
	}
	catch (const std::exception& ex) {
		p_config.add_message(ex.what());
	}
}

void Savefile_window::draw_ui(SP_Config& p_config) {
	draw_ui_hallfame(p_config);
	draw_ui_players(p_config);
}

void Savefile_window::draw_ui_hallfame(SP_Config& p_config) {
	int l_zindex = m_selected_hf - 1;

	ImGui::Begin("Hall of Fame");
	ImGui::SliderInt("Slot", &m_selected_hf, 1, c::HALLFAME_ENTRY_COUNT);

	ImGui::Separator();
	std::string l_name = "Name:" + m_hallfame.get_name(l_zindex);
	ImGui::Text(l_name.c_str());
	ImGui::Separator();

	int l_hrs{ static_cast<int>(m_hallfame.get_hrs(l_zindex)) };
	if (ImGui::SliderInt("Hours", &l_hrs, 0, 255))
		m_hallfame.set_hrs(l_zindex, l_hrs);

	int l_mins{ static_cast<int>(m_hallfame.get_mins(l_zindex)) };
	if (ImGui::SliderInt("Minutes", &l_mins, 0, 59))
		m_hallfame.set_mins(l_zindex, l_mins);

	int l_secs{ static_cast<int>(m_hallfame.get_secs(l_zindex)) };
	if (ImGui::SliderInt("Seconds", &l_secs, 0, 59))
		m_hallfame.set_secs(l_zindex, l_secs);

	ImGui::Separator();
	ImGui::Text(c::TXT_FILE_OPERATIONS);
	ImGui::Separator();
	if (ImGui::Button(c::SAVE_LST))
		save_hallfame_lst(p_config);
	ImGui::SameLine();
	ImGui::Button(c::SAVE_XML);

	if (ImGui::Button(c::LOAD_LST))
		load_hallfame_lst(p_config);
	ImGui::SameLine();
	ImGui::Button(c::LOAD_XML);

	ImGui::End();
}

void Savefile_window::draw_ui_players(SP_Config& p_config) {
	int l_zindex = m_selected_player - 1;
	int l_next_lvl = m_players.get_current_level(l_zindex);
	ImGui::Begin("Players");

	ImGui::SliderInt("Slot", &m_selected_player, 1, c::PLAYER_COUNT);

	ImGui::Separator();
	ImGui::Text(m_players.get_name(l_zindex).c_str());

	ImGui::Separator();

	int l_hrs{ static_cast<int>(m_players.get_hrs(l_zindex)) };
	if (ImGui::SliderInt("Hours", &l_hrs, 0, 255))
		m_players.set_hrs(l_zindex, l_hrs);

	int l_mins{ static_cast<int>(m_players.get_mins(l_zindex)) };
	if (ImGui::SliderInt("Minutes", &l_mins, 0, 59))
		m_players.set_mins(l_zindex, l_mins);

	int l_secs{ static_cast<int>(m_players.get_secs(l_zindex)) };
	if (ImGui::SliderInt("Seconds", &l_secs, 0, 59))
		m_players.set_secs(l_zindex, l_secs);

	ImGui::Separator();

	ImGui::PushStyleColor(ImGuiCol_Text, sp_color_to_imgui(c::COL_BLACK)); // start text color
	for (std::size_t i{ 0 }; i <= 11; ++i) {
		for (std::size_t j{ 0 }; j < (i == 11 ? 1 : 10); ++j) {
			int l_level_no = static_cast<int>(i * 10 + j);

			if (l_level_no == l_next_lvl)
				ImGui::PushStyleColor(ImGuiCol_Button, sp_color_to_imgui(c::COL_YELLOW));
			else {
				byte l_status = m_players.get_status(l_zindex, l_level_no);
				ImGui::PushStyleColor(ImGuiCol_Button, sp_color_to_imgui(
					l_status == 0 ? c::COL_RED : (l_status == 1 ? c::COL_GREEN_DARK : c::COL_BLUE_LIGHT)
				));
			}
			if (ImGui::Button(m_button_labels[l_level_no].c_str()))
				m_players.toggle_status(l_zindex, l_level_no);
			ImGui::PopStyleColor();

			ImGui::SameLine();
		}
		ImGui::NewLine();
	}
	ImGui::PopStyleColor(); // end text color

	ImGui::Separator();
	ImGui::Text("Player Database Operations");
	ImGui::Separator();

	if (ImGui::Button("Move Player Up"))
		if (l_zindex > 0) {
			m_players.move_player(l_zindex, true);
			--m_selected_player;
		}
	ImGui::SameLine();
	if (ImGui::Button("Move Player Down"))
		if (l_zindex < c::PLAYER_COUNT - 1) {
			m_players.move_player(l_zindex, false);
			++m_selected_player;
		}
	if (ImGui::Button("Delete Player"))
		m_players.delete_player(l_zindex);

	ImGui::Separator();

	ImGui::Text(c::TXT_FILE_OPERATIONS);
	ImGui::Separator();
	if (ImGui::Button(c::SAVE_LST))
		save_player_lst(p_config);
	ImGui::SameLine();
	ImGui::Button(c::SAVE_XML);

	if (ImGui::Button(c::LOAD_LST))
		load_player_lst(p_config);
	ImGui::SameLine();
	ImGui::Button(c::LOAD_XML);

	ImGui::End();
}


constexpr ImU32 Savefile_window::sp_color_to_imgui(c::SP_Color p_color) {
	return IM_COL32(p_color.r, p_color.g, p_color.b, 255);
}

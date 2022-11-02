#ifndef SP_SAVEFILE_WINDOW_H
#define SP_SAVEFILE_WINDOW_H

#include "./../SP_Config.h"
#include "./../SP_Constants.h"
#include "./../sp_saves/SP_Hallfame.h"
#include "./../sp_saves/SP_Player_list.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"

class Savefile_window {

	SP_Player_list m_players;
	SP_Hallfame m_hallfame;

	int m_selected_hf, m_selected_player;
	std::vector<std::string> m_button_labels;

	void draw_ui_hallfame(SP_Config& p_config);
	void draw_ui_players(SP_Config& p_config);

	// save and load hall of fame list
	void load_hallfame_lst(SP_Config& p_config, bool p_first_load = false);
	void save_hallfame_lst(SP_Config& p_config);

	// save and load player database
	void load_player_lst(SP_Config& p_config, bool p_first_load = false);
	void save_player_lst(SP_Config& p_config);

public:
	Savefile_window(SP_Config& p_config);
	void draw_ui(SP_Config& p_config);

	static constexpr ImU32 sp_color_to_imgui(c::SP_Color p_color);
};

#endif

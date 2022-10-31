#ifndef SP_MAIN_WINDOW_H
#define SP_MAIN_WINDOW_H

#include <SDL.h>
#include <string>
#include <vector>
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include "./../SP_Config.h"
#include "./../sp_levels/SP_Level.h"
#include "./../sp_gfx/Project_gfx.h"
#include "./../common/klib/User_input.h"
#include "Level_window.h"
#include "Graphics_window.h"

class Main_window {
	Project_gfx m_gfx;
	std::size_t m_current_window;
	Level_window m_lvl_win;
	Graphics_window m_gfx_win;
	std::vector<std::string> m_selectable_windows;

public:
	Main_window(SDL_Renderer* p_rnd, SP_Config& p_config);
	void move(int p_delta_ms, const klib::User_input& p_input, SP_Config& p_config, int p_w, int p_h);
	void draw(SDL_Renderer* p_rnd, const klib::User_input& p_input, SP_Config& p_config, int p_w, int p_h);
	void draw_ui(SDL_Renderer* p_rnd, const klib::User_input& p_input, SP_Config& p_config);
	void set_application_icon(SDL_Window* p_win) const;

	// static utility functions
	static void window_start(const std::string& p_title, c::SP_Color p_text, c::SP_Color p_active, c::SP_Color p_inactive, c::SP_Color p_collapsed);
	static ImU32 sp_color_to_imgui(c::SP_Color p_color);
};

#endif

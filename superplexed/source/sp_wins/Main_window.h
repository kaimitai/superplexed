#ifndef SP_MAIN_WINDOW_H
#define SP_MAIN_WINDOW_H

#include <SDL.h>
#include <vector>
#include "./../sp_levels/SP_Level.h"
#include "./../sp_gfx/Project_gfx.h"
#include "./../common/klib/User_input.h"
#include "Level_window.h"

class Main_window {
	Project_gfx m_gfx;
	std::size_t m_current_window;
	Level_window m_lvl_win;

public:
	Main_window(SDL_Renderer* p_rnd);
	void move(int p_delta_ms, const klib::User_input& p_input, int p_w, int p_h);
	void draw(SDL_Renderer* p_rnd, int p_w, int p_h);
	void draw_ui(void);
};

#endif

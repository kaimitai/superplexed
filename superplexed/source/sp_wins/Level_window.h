#ifndef SP_LEVEL_WINDOW_H
#define SP_LEVEL_WINDOW_H

#include <SDL.h>
#include <vector>
#include "./../sp_levels/SP_Level.h"
#include "./../sp_gfx/Project_gfx.h"
#include "./../common/klib/User_input.h"

class Level_window {

	std::vector<SP_Level> m_levels;
	std::size_t m_current_level, m_cam_x;
	SDL_Texture* m_texture;

	void regenerate_texture(SDL_Renderer* p_rnd, const Project_gfx& p_gfx);

public:
	Level_window(SDL_Renderer* p_rnd);
	void move(int p_delta_ms, const klib::User_input& p_input);
	void draw(SDL_Renderer* p_rnd, const Project_gfx& p_gfx, int p_w, int p_h);
	void draw_ui(void);
};

#endif

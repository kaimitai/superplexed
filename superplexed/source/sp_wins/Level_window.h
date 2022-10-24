#ifndef SP_LEVEL_WINDOW_H
#define SP_LEVEL_WINDOW_H

#include <SDL.h>
#include <vector>
#include "./../sp_levels/SP_Level.h"
#include "./../sp_gfx/Project_gfx.h"
#include "./../common/klib/User_input.h"

class Level_window {

	std::vector<SP_Level> m_levels;
	int m_cam_x, m_current_level, m_current_gp;
	SDL_Texture* m_texture;
	bool m_ui_show_grid, m_ui_animate;

	int get_tile_pixel_w(int p_screen_pixel_h) const;
	void regenerate_texture(SDL_Renderer* p_rnd, const Project_gfx& p_gfx);
	std::size_t get_current_level_idx(void) const;

	void save_xml(std::size_t p_level_no) const;

public:
	Level_window(SDL_Renderer* p_rnd);
	void move(int p_delta_ms, const klib::User_input& p_input);
	void draw(SDL_Renderer* p_rnd, const Project_gfx& p_gfx, int p_w, int p_h);
	void draw_ui(void);
};

#endif

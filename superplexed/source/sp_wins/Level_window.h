#ifndef SP_LEVEL_WINDOW_H
#define SP_LEVEL_WINDOW_H

#include <SDL.h>
#include <deque>
#include <string>
#include <utility>
#include <vector>
#include "./../SP_Config.h"
#include "./../sp_levels/SP_Level.h"
#include "./../sp_gfx/Project_gfx.h"
#include "./../common/klib/User_input.h"
#include "./../common/klib/Timer.h"

class Level_window {

	std::vector<SP_Level> m_levels;
	int m_cam_x, m_current_level, m_current_gp, m_sel_tile;
	SDL_Texture* m_texture;
	bool m_ui_show_grid, m_ui_animate;
	std::vector<klib::Timer> m_timers;
	std::vector<std::pair<std::string, std::vector<int>>> m_tile_picker;
	float m_tile_picker_scale;

	// selection
	int m_sel_x, m_sel_y,
		m_sel_x2, m_sel_y2;
	std::vector<std::vector<byte>> m_clipboard;
	// dictionary of substitutions for applying on rotations and flips
	std::vector<std::map<byte, byte>> m_transforms;
	byte apply_transform(byte p_byte_value, std::size_t p_transform_no) const;

	int get_tile_pixel_w(int p_screen_pixel_h) const;
	void regenerate_texture(SDL_Renderer* p_rnd, const Project_gfx& p_gfx);
	std::size_t get_current_level_idx(void) const;

	// selection procedures and functions
	bool has_selection(void) const;
	void clear_selection(void);
	void copy_to_clipboard(void);
	void paste_from_clipboard(void);
	void delete_selection(void);
	void cut_selection(void);
	SDL_Rect get_selection_rectangle(void) const;
	bool is_selection_empty(void) const;
	void flip_selection(bool p_vertical);
	void select_all(void);
	void show_clipboard_destination(void);
	bool selection_fits(void) const;
	void rotate_selection(bool p_cclockwise, SP_Config& p_config);
	std::pair<int, int> mouse_coords_to_tile(int p_mouse_x, int p_mouse_y, int p_screen_h) const;

	// xml read/write
	void save_xml(std::size_t p_level_no, const SP_Config& p_config) const;
	SP_Level load_xml(std::size_t p_level_no, const SP_Config& p_config) const;
	// sp read/write
	void save_sp(std::size_t p_level_no, const SP_Config& p_config) const;
	SP_Level load_sp(std::size_t p_level_no, const SP_Config& p_config) const;
	// dat read/write
	void load_levels_dat(SP_Config& p_config);
	// ui
	void draw_ui_level_win(const klib::User_input& p_input, const Project_gfx& p_gfx, SP_Config& p_config);
	void draw_ui_gp_win(SP_Config& p_config);
	void draw_ui_tile_win(const Project_gfx& p_gfx);

public:
	Level_window(SDL_Renderer* p_rnd, SP_Config& p_config);
	void move(int p_delta_ms, const klib::User_input& p_input, SP_Config& p_config, int p_w, int p_h);
	void draw(SDL_Renderer* p_rnd, const Project_gfx& p_gfx, int p_w, int p_h);
	void draw_ui(const Project_gfx& p_gfx, const klib::User_input& p_input, SP_Config& p_config);
};

#endif

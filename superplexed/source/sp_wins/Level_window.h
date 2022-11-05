#ifndef SP_LEVEL_WINDOW_H
#define SP_LEVEL_WINDOW_H

#include <SDL.h>
#include <deque>
#include <string>
#include <utility>
#include <vector>
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include "./../SP_Config.h"
#include "./../sp_levels/SP_Level.h"
#include "./../sp_levels/SP_Level_undo_interface.h"
#include "./../sp_gfx/Project_gfx.h"
#include "./../common/klib/User_input.h"
#include "./../common/klib/Timer.h"
#include "./../SP_Constants.h"

class Level_window {
	enum class SP_file_type { bmp, dat, sp, xml };

	struct Level {
		SP_Level m_level;
		SP_Level_undo_interface m_undo;
		Level(const SP_Level& p_level) : m_level{ p_level }
		{}
	};

	std::vector<Level> m_levels;
	std::vector<std::string> m_levelset_files;
	int m_cam_x, m_current_level, m_current_gp, m_sel_tile;
	SDL_Texture* m_texture;
	bool m_ui_show_grid, m_ui_animate, m_ui_flash;
	std::vector<klib::Timer> m_timers;
	std::vector<std::pair<std::string, std::vector<int>>> m_tile_picker;
	float m_tile_picker_scale;
	std::string m_selected_file;

	// statistics variables
	// -1: do not show
	// 0: stats for all levels
	// n>0: stats for level #n (counted from 1)
	int m_show_stats;
	bool m_show_stats_tc0; // show tiles with a count of 0 in the stats
	std::vector<int> m_statistics;


	// selection
	int m_sel_x, m_sel_y,
		m_sel_x2, m_sel_y2;
	std::vector<std::vector<byte>> m_clipboard;
	// dictionary of substitutions for applying on rotations and flips
	std::vector<std::map<byte, byte>> m_transforms;
	byte apply_transform(byte p_byte_value, std::size_t p_transform_no) const;
	int get_max_cam_x(int p_w, int p_h) const;

	int get_tile_pixel_w(int p_screen_pixel_h) const;
	void regenerate_texture(SDL_Renderer* p_rnd, const Project_gfx& p_gfx);
	std::size_t get_current_level_idx(void) const;
	SP_Level& get_current_level(void);
	const SP_Level& get_current_level(void) const;

	// selection procedures and functions
	bool has_selection(void) const;
	void clear_selection(void);
	void copy_to_clipboard(void);
	void paste_from_clipboard(void);
	void delete_selection(bool p_delete_special_ports);
	void cut_selection(void);
	SDL_Rect get_selection_rectangle(void) const;
	bool is_selection_empty(void) const;
	void flip_selection(bool p_vertical);
	void select_all(void);
	void show_clipboard_destination(void);
	bool selection_fits(void) const;
	void rotate_selection(bool p_cclockwise, SP_Config& p_config);
	std::pair<int, int> mouse_coords_to_tile(int p_mouse_x, int p_mouse_y, int p_screen_h) const;
	std::vector<int> get_tile_counts(bool p_all_levels) const;
	void toggle_selected_gp_property(int p_property_no);
	void floorfill(byte p_source_col, byte p_target_col, int p_x, int p_y);

	// xml read/write
	void save_xml(std::size_t p_level_no, const SP_Config& p_config) const;
	SP_Level load_xml(std::size_t p_level_no, const SP_Config& p_config) const;
	// sp read/write
	void save_sp(std::size_t p_level_no, const SP_Config& p_config) const;
	SP_Level load_sp(std::size_t p_level_no, const SP_Config& p_config) const;
	// dat read/write
	void load_levels_dat(SP_Config& p_config);
	void save_levels_dat(SP_Config& p_config);
	// ui
	void draw_ui_level_win(const klib::User_input& p_input, const Project_gfx& p_gfx, SP_Config& p_config);
	void draw_ui_gp_win(const Project_gfx& p_gfx, SP_Config& p_config);
	void draw_ui_tile_win(const klib::User_input& p_input, SP_Config& p_config, const Project_gfx& p_gfx);
	void draw_ui_statistics(const Project_gfx& p_gfx);

	// general procedures for saving to and loading from files
	void load_file(SP_file_type p_ftype, SP_Config& p_config, bool p_all);
	void save_file(SP_file_type p_ftype, SP_Config& p_config, const Project_gfx& p_gfx, bool p_all) const;

	void commit_undo_block(void);
	void set_tile_value(int p_x, int p_y, byte p_value, bool p_autocommit = false);
	void apply_border_to_current_level(void);

	// file
	std::vector<std::string> get_levelset_files(const SP_Config& p_config);

public:
	Level_window(SDL_Renderer* p_rnd, SP_Config& p_config);
	void move(int p_delta_ms, const klib::User_input& p_input, SP_Config& p_config, int p_w, int p_h);
	void draw(SDL_Renderer* p_rnd, const Project_gfx& p_gfx, int p_w, int p_h);
	void draw_ui(const Project_gfx& p_gfx, const klib::User_input& p_input, SP_Config& p_config);
};

#endif

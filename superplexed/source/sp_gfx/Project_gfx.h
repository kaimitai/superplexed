#ifndef SP_PROJECT_GFX_H
#define SP_PROJECT_GFX_H

#include <SDL.h>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "SP_Palette.h"
#include "SP_Image.h"
#include "./../SP_Config.h"
#include "./../sp_levels/SP_Level.h"
#include "./../SP_Constants.h"

using byte = unsigned char;

class Project_gfx {

	struct Gfx_metadata {
		int m_width, m_height, m_palette_no;
		bool m_binary;
		bool m_extra;

		Gfx_metadata(int p_w, int p_h, int p_pal_no,
			bool p_xtra = false, bool p_binary = false);
	};

	std::vector<std::pair<int, int>> m_tile_definitions;
	std::vector<SDL_Texture*> m_tile_textures;
	std::vector<std::vector<std::size_t>> m_animations;

	std::vector<SDL_Texture*> m_app_gfx;
	std::vector<SP_Palette> m_palettes;

	std::map<std::string, SP_Image> m_image_files;
	std::map<std::string, SDL_Texture*> m_image_textures;
	std::map<std::string, Gfx_metadata> m_image_metadata;

	SDL_Surface* sp_image_to_sdl_surface(const SP_Image& p_image, const SP_Palette& p_palette, int p_transp_idx = -1) const;
	SDL_Color sp_color_to_sdl(const std::tuple<byte, byte, byte, byte>& p_col) const;
	bool save_bmp(const SP_Image& p_image, const SP_Palette& p_palette, const std::string& p_filename) const;
	void regenerate_all_textures(SDL_Renderer* p_rnd);
	void regenerate_texture(SDL_Renderer* p_rnd, const std::string& p_filename);
	void generate_tile_definitions(void);

	// palette functions
	void load_fixed_palettes(void);

	// bmp functions
	// will retrieve pixel values from FIXED and MOVING
	// acts as if the pictures are glued together with FIXED on top
	byte get_sprite_pixel(int p_x, int p_y) const;
	void draw_tile_on_sdl_surface(SDL_Surface* p_srf, int p_tile_no, int p_tile_x, int p_tile_y) const;
	byte find_nearest_palette_index(SDL_Color p_color, const SP_Palette& p_palette) const;
	SDL_Surface* create_sdl_surface_with_sp_palette(int p_w, int p_h, int p_palette_no) const;
	SDL_Texture* create_tile_texture(SDL_Renderer* p_rnd, int p_tile_no = 0) const;
	void draw_rect_on_surface(SDL_Surface* p_srf, int p_x, int p_y, int p_w, int p_h, byte p_col_index, int p_thickness = 0) const;

public:
	Project_gfx(SDL_Renderer* p_rnd, const SP_Config& p_config);
	~Project_gfx(void);
	SDL_Texture* get_tile_texture(std::size_t p_texture_no, std::size_t p_frame_no = 0) const;
	SDL_Texture* get_image_texture(const std::string& p_filename) const;
	void blit_fixed(SDL_Renderer* p_rnd, std::size_t p_char_no, int p_x, int p_y, SDL_Color p_color) const;

	std::pair<int, int> get_image_dimensions(const std::string& p_filename) const;

	void load_image_data_from_file(SDL_Renderer* p_rnd, const std::string& p_filename, const SP_Config& p_config);
	void load_image_data_from_vector(SDL_Renderer* p_rnd, const SP_Config& p_config, const std::string& p_filename, const std::vector<byte>& p_bytes);
	bool save_bmp(const std::string& p_filename, SP_Config& p_config) const;
	void save_dat(const std::string& p_filename, SP_Config& p_config) const;

	const std::map<std::string, Project_gfx::Gfx_metadata>& get_meta(void) const;

	constexpr static SDL_Color sp_col_to_sdl(c::SP_Color p_col) {
		return SDL_Color{ p_col.r, p_col.g, p_col.b };
	}

	constexpr static SDL_Color sp_col_to_sdl(unsigned char r, unsigned char g, unsigned char b) {
		return SDL_Color{ r, g, b };
	}

	// palette functions
	const std::vector<SP_Palette>& get_palettes(void) const;
	void load_palettes(SDL_Renderer* p_rnd, const SP_Config& p_config);
	void set_palettes(SDL_Renderer* p_rnd, const std::vector<SP_Palette>& p_palettes);
	void save_palettes_dat(const SP_Config& p_config);

	// bmp functions
	void load_bmp(SDL_Renderer* p_rnd, const SP_Config& p_config, const std::string& p_filename);
	bool save_level_bmp(const SP_Level& p_level, std::size_t p_level_no, const SP_Config& p_config, bool p_draw_metadata) const;
	SDL_Surface* create_application_icon(void) const;
	std::vector<std::vector<byte>> generate_tile_art(const SP_Config& p_config, int p_w, int p_h) const;

	// xml functions
	void load_image_xml(SDL_Renderer* p_rnd, const SP_Config& p_config, const std::string& p_filename);
	void load_palette_xml(SDL_Renderer* p_rnd, const SP_Config& p_config);
	void save_image_xml(const SP_Config& p_config, const std::string& p_filename) const;
	void save_palette_xml(const SP_Config& p_config) const;
};

#endif

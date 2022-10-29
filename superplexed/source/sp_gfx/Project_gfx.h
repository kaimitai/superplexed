#ifndef SP_PROJECT_GFX_H
#define SP_PROJECT_GFX_H

#include <SDL.h>
#include <map>
#include <string>
#include <vector>
#include "SP_Palette.h"
#include "SP_Image.h"
#include "./../SP_Config.h"

using byte = unsigned char;

class Project_gfx {

	struct Gfx_metadata {
		int m_width, m_height, m_palette_no;
		bool m_binary;
		bool m_extra;

		Gfx_metadata(int p_w, int p_h, int p_pal_no,
			bool p_xtra = false, bool p_binary = false);
	};

	std::vector<SDL_Texture*> m_static;
	std::vector<SDL_Texture*> m_moving;
	std::vector<SDL_Texture*> m_font;
	std::vector<SP_Palette> m_palettes;
	std::map<std::size_t, std::vector<std::size_t>> m_animations;

	std::map<std::string, SP_Image> m_image_files;
	std::map<std::string, SDL_Texture*> m_image_textures;
	std::map<std::string, Gfx_metadata> m_image_metadata;

	SDL_Surface* sp_image_to_sdl_surface(const SP_Image& p_image, const SP_Palette& p_palette, int p_transp_idx = -1) const;
	SDL_Color sp_color_to_sdl(const std::tuple<byte, byte, byte, byte>& p_col) const;
	bool save_bmp(const SP_Image& p_image, const SP_Palette& p_palette, const std::string& p_filename) const;
	void regenerate_texture(SDL_Renderer* p_rnd, const std::string& p_filename);

public:
	Project_gfx(SDL_Renderer* p_rnd, const SP_Config& p_config);
	~Project_gfx(void);
	SDL_Texture* get_static(std::size_t p_texture_no) const;
	SDL_Texture* get_animated(std::size_t p_texture_no, std::size_t p_frame_no) const;
	SDL_Texture* get_image_texture(const std::string& p_filename) const;
	void blit_font(SDL_Renderer* p_rnd, std::size_t p_char_no, int p_x, int p_y, int p_w, int p_h, SDL_Color p_color) const;

	std::vector<byte> get_palette_bytes(void) const;

	bool load_image_data_from_file(SDL_Renderer* p_rnd, const std::string& p_filename, const SP_Config& p_config);
	bool save_bmp(const std::string& p_filename, SP_Config& p_config) const;
	void save_dat(const std::string& p_filename, SP_Config& p_config) const;

	const std::map<std::string, Project_gfx::Gfx_metadata>& get_meta(void) const;

	// palette functions
	const std::vector<SP_Palette>& get_palettes(void) const;
	void set_palettes(const std::vector<SP_Palette>& p_palettes);
	void save_palettes_dat(const SP_Config& p_config);
};

#endif

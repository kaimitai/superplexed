#include "Project_gfx.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include <tuple>

using byte = unsigned char;

SDL_Texture* Project_gfx::get_static(std::size_t p_texture_no) const {
	return m_static.at(p_texture_no);
}

SDL_Texture* Project_gfx::get_animated(std::size_t p_texture_no, std::size_t p_frame_no) const {
	if (m_animations.find(p_texture_no) != end(m_animations))
		return m_moving[m_animations.at(p_texture_no).at(p_frame_no)];
	else
		return get_static(p_texture_no);
}

Project_gfx::~Project_gfx(void) {
	for (auto texture : m_static)
		if (texture != nullptr)
			SDL_DestroyTexture(texture);

	for (auto texture : m_moving)
		if (texture != nullptr)
			SDL_DestroyTexture(texture);
}

Project_gfx::Project_gfx(SDL_Renderer* p_rnd) {
	// read palette data
	std::vector<byte> l_bytes = klib::file::read_file_as_bytes("./gamedata/PALETTES.DAT");
	for (std::size_t i{ 0 }; i < 4; ++i)
		m_palettes.push_back(SP_Palette(std::vector<byte>(begin(l_bytes) + i * 4 * 16,
			begin(l_bytes) + (i + 1) * 4 * 16)));

	// read image data
	m_image_files.insert(std::make_pair("FIXED",
		SP_Image(klib::file::read_file_as_bytes("./gamedata/FIXED.DAT"),
			640)));
	m_image_files.insert(std::make_pair("MOVING",
		SP_Image(klib::file::read_file_as_bytes("./gamedata/MOVING.DAT"),
			320)));

	// create textures
	m_static = klib::gfx::split_surface(p_rnd,
		sp_image_to_sdl_surface(m_image_files.at("FIXED"), m_palettes.at(1)),
		16, 16);

	auto l_moving_srf = sp_image_to_sdl_surface(m_image_files.at("MOVING"), m_palettes.at(1));
	std::vector<SDL_Rect> l_moving_rects{
		{304, 64, 16, 16 }, { 304, 100, 16, 16 }, { 256, 196, 16, 16 }, { 272, 196, 16, 16 }, { 288, 196, 16, 16 }, { 304, 196, 16, 16 },
		{176, 446, 16, 16}, {176 + 16, 446, 16, 16},{176 + 32, 446, 16, 16},{176 + 48, 446, 16, 16},{176 + 64, 446, 16, 16} //,{176 + 80, 446, 16, 16}
	};

	m_moving = klib::gfx::split_surface_specified(p_rnd, l_moving_srf, l_moving_rects);

	m_animations[3] = { 6, 7,8,9,10,9 }; // player
	m_animations[25] = { 0, 1,2,3,4,5 }; // "bug" enemy
}

bool Project_gfx::save_bmp(const std::string& p_filename) const {
	if (p_filename == "FIXED")
		return save_bmp(m_image_files.at(p_filename),
			m_palettes.at(1),
			p_filename + ".bmp");
	else if (p_filename == "MOVING")
		return save_bmp(m_image_files.at(p_filename),
			m_palettes.at(1),
			p_filename + ".bmp");
	else
		return false;
}

bool Project_gfx::save_bmp(const SP_Image& p_image, const SP_Palette& p_palette, const std::string& p_filename) const {
	SDL_Surface* l_bmp = sp_image_to_sdl_surface(p_image, p_palette);
	int file_status = SDL_SaveBMP(l_bmp, p_filename.c_str());
	SDL_FreeSurface(l_bmp);
	return (file_status != -1);
}

SDL_Surface* Project_gfx::sp_image_to_sdl_surface(const SP_Image& p_image, const SP_Palette& p_palette) const {
	SDL_Surface* l_bmp = SDL_CreateRGBSurface(0, p_image.get_w(),
		p_image.get_h(), 8, 0, 0, 0, 0);

	SDL_Color out_palette[256] = { 0, 0, 0 };
	for (int i{ 0 }; i < static_cast<int>(p_palette.get_size()); ++i)
		out_palette[i] = SDL_Color{ std::get<0>(p_palette.get_color(i)),
	std::get<1>(p_palette.get_color(i)),
	std::get<2>(p_palette.get_color(i)) };

	SDL_SetPaletteColors(l_bmp->format->palette, out_palette, 0, 256);

	for (int j{ 0 }; j < p_image.get_h(); ++j)
		for (int i{ 0 }; i < p_image.get_w(); ++i)
			klib::gfx::put_pixel(l_bmp, i, j, p_image.get_palette_index(i, j));

	return l_bmp;
}

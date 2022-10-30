#include "Project_gfx.h"
#include "./../common/klib/klib_gfx.h"
#include "./../SP_Constants.h"
#include <filesystem>

// load a bmp from file, update internal image and regenerate SDL texture
void Project_gfx::load_bmp(SDL_Renderer* p_rnd,
	const SP_Config& p_config,
	const std::string& p_filename) {

	auto frame_srf = SDL_LoadBMP(p_config.get_bmp_full_path(p_filename).c_str());
	int l_target_w = m_image_metadata.at(p_filename).m_width;
	int l_target_h = m_image_metadata.at(p_filename).m_height;

	if (frame_srf == nullptr)
		throw std::exception("Could not load BMP");
	else if (frame_srf->format->BitsPerPixel != 8) {
		SDL_FreeSurface(frame_srf);
		throw std::exception("Not a 256-color BMP");
	}
	else if (frame_srf->w != l_target_w ||
		frame_srf->h != l_target_h) {
		SDL_FreeSurface(frame_srf);
		throw std::exception("Invalid image dimensions");
	}

	// vector to be used as input for SP_Image
	std::vector<std::vector<byte>> result;
	// recolor dictionary
	std::map<byte, byte> recolors;

	// make a map of palette indexes from the loaded image to the actual game palette
	for (int i{ 0 }; i < frame_srf->format->palette->ncolors; ++i) {
		SDL_Color pcol = frame_srf->format->palette->colors[i];
		recolors.insert(std::make_pair(i,
			find_nearest_palette_index(pcol,
				m_palettes.at(m_image_metadata.at(p_filename).m_palette_no))));
	}

	// recolor the image using the map above
	for (int j{ 0 }; j < l_target_h; ++j) {
		std::vector<byte> l_pixel_row;
		for (int i{ 0 }; i < l_target_w; ++i) {
			l_pixel_row.push_back(recolors[
				static_cast<byte>(klib::gfx::get_pixel(frame_srf, i, j))
			]);
		}
		result.push_back(l_pixel_row);
	}

	// final touch - add unknown end byte if the metadata tells us one 0xff exists
	m_image_files.erase(p_filename);
	m_image_files.insert(std::make_pair(p_filename,
		SP_Image(result,
			m_image_metadata.at(p_filename).m_extra,
			m_image_metadata.at(p_filename).m_binary)
	));

	regenerate_texture(p_rnd, p_filename);
}

void Project_gfx::draw_tile_on_sdl_surface(SDL_Surface* p_srf, int p_tile_no, int p_tile_x, int p_tile_y) const {
	int l_tx = c::TILE_W * p_tile_x;
	int l_ty = c::TILE_W * p_tile_y;

	for (int x{ 0 }; x < c::TILE_W; ++x)
		for (int y{ 0 }; y < c::TILE_W; ++y)
			klib::gfx::put_pixel(p_srf, l_tx + x, l_ty + y,
				get_sprite_pixel(p_tile_no * c::TILE_W + x, y));
}

bool Project_gfx::save_level_bmp(const SP_Level& p_level, std::size_t p_level_no, const SP_Config& p_config) const {
	SDL_Surface* l_bmp{ create_sdl_surface_with_sp_palette(c::TILE_W * c::LEVEL_W,
		c::TILE_W * c::LEVEL_H, m_image_metadata.at("FIXED").m_palette_no) };

	for (int j{ 0 }; j < c::LEVEL_H; ++j)
		for (int i{ 0 }; i < c::LEVEL_W; ++i)
			draw_tile_on_sdl_surface(l_bmp,
				static_cast<int>(p_level.get_tile_no(i, j)),
				i, j);

	draw_tile_on_sdl_surface(l_bmp, 3,
		p_level.get_start_pos().first, p_level.get_start_pos().second);

	std::filesystem::create_directory(p_config.get_bmp_folder());
	int file_status = SDL_SaveBMP(l_bmp, p_config.get_bmp_full_path(p_level_no).c_str());
	SDL_FreeSurface(l_bmp);
	return (file_status != -1);
}

SDL_Surface* Project_gfx::create_sdl_surface_with_sp_palette(int p_w, int p_h, int p_palette_no) const {
	SDL_Surface* l_bmp = SDL_CreateRGBSurface(0, p_w,
		p_h, 8, 0, 0, 0, 0);

	SDL_Color out_palette[256] = { 0, 0, 0 };
	for (int i{ 0 }; i < static_cast<int>(m_palettes.at(p_palette_no).get_size()); ++i)
		out_palette[i] = sp_color_to_sdl(m_palettes.at(p_palette_no).get_color(i));

	SDL_SetPaletteColors(l_bmp->format->palette, out_palette, 0, 256);

	return l_bmp;
}

bool Project_gfx::save_bmp(const std::string& p_filename, SP_Config& p_config) const {
	if (m_image_files.find(p_filename) == end(m_image_files))
		return false;

	std::filesystem::create_directory(p_config.get_bmp_folder());

	return save_bmp(m_image_files.at(p_filename),
		m_palettes.at(m_image_metadata.at(p_filename).m_palette_no),
		p_config.get_bmp_full_path(p_filename));
}

bool Project_gfx::save_bmp(const SP_Image& p_image, const SP_Palette& p_palette, const std::string& p_filename) const {
	SDL_Surface* l_bmp = sp_image_to_sdl_surface(p_image, p_palette);
	int file_status = SDL_SaveBMP(l_bmp, p_filename.c_str());
	SDL_FreeSurface(l_bmp);
	return (file_status != -1);
}

byte Project_gfx::get_sprite_pixel(int p_x, int p_y) const {
	if (p_y < c::TILE_W)
		return m_image_files.at("FIXED").get_palette_index(p_x, p_y);
	else
		return m_image_files.at("MOVING").get_palette_index(p_x, p_y - c::TILE_W);
}

void save_level_bmp(const SP_Level& p_level, std::size_t p_level_no, const SP_Config& p_config) {

}

byte Project_gfx::find_nearest_palette_index(SDL_Color p_color, const SP_Palette& p_palette) const {
	int min_distance = 3 * 256 * 256 + 1; // higher than any possible color distance
	byte result{ 0 };

	for (int i{ 0 }; i < static_cast<int>(p_palette.get_size()); ++i) {
		SDL_Color col{ sp_color_to_sdl(p_palette.get_color(i)) };

		int dr = col.r - p_color.r;
		int dg = col.g - p_color.g;
		int db = col.b - p_color.b;

		int l_dist = dr * dr + dg * dg + db * db;

		if (l_dist < min_distance) {
			min_distance = l_dist;
			result = static_cast<byte>(i);
		}
	}

	return result;
}

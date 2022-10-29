#include "Project_gfx.h"
#include "./../common/klib/klib_gfx.h"

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

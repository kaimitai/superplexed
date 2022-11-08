#include "KImage.h"
#include <SDL.h>
#include <map>
#include <stdexcept>
#include "klib_gfx.h"

using byte = unsigned char;

SDL_Surface* klib::gfx::KImage::to_sdl_surface(std::size_t p_transp_ind) const {
	SDL_Surface* l_bmp = SDL_CreateRGBSurface(0, w(), h(), 8, 0, 0, 0, 0);

	SDL_Color out_palette[256] = { 0, 0, 0 };
	for (int i{ 0 }; i < static_cast<int>(m_palette.size()); ++i)
		out_palette[i] = m_palette[i];

	SDL_SetPaletteColors(l_bmp->format->palette, out_palette, 0, 256);

	for (int j{ 0 }; j < h(); ++j)
		for (int i{ 0 }; i < w(); ++i)
			put_pixel(l_bmp, i, j, m_pixels[j][i]);

	if (p_transp_ind < 256) {
		SDL_Color l_trans_rgb = m_palette[p_transp_ind];
		SDL_SetColorKey(l_bmp, true, SDL_MapRGB(l_bmp->format, l_trans_rgb.r, l_trans_rgb.g, l_trans_rgb.b));
	}

	return l_bmp;
}

const std::vector<SDL_Color>& klib::gfx::KImage::get_palette(void) const {
	return m_palette;
}

klib::gfx::KImage::KImage(const std::string& p_filename) {
	std::vector<SDL_Color> l_palette;
	std::map<byte, byte> l_color_map;

	auto frame_srf = SDL_LoadBMP(p_filename.c_str());
	if (frame_srf->format->BitsPerPixel != 8) {
		SDL_FreeSurface(frame_srf);
		throw std::runtime_error("Not a 256-color image");
	}

	byte l_curcol{ 0 };
	for (int j{ 0 }; j < frame_srf->h; ++j) {
		std::vector<byte> col_row;
		for (int i{ 0 }; i < frame_srf->w; ++i) {
			byte l_spal_index = get_pixel(frame_srf, i, j);
			auto iter = l_color_map.find(l_spal_index);
			if (iter == end(l_color_map)) {
				l_color_map.insert(std::make_pair(l_spal_index, l_curcol++));
				m_palette.push_back(frame_srf->format->palette->colors[l_spal_index]);
			}
			col_row.push_back(l_color_map.find(l_spal_index)->second);
		}
		m_pixels.push_back(col_row);
	}

	SDL_FreeSurface(frame_srf);
}

klib::gfx::KImage::KImage(const std::vector<byte>& p_bytes) {
	int l_w = p_bytes.at(0) * 256 + p_bytes.at(1);
	int l_h = p_bytes.at(2) * 256 + p_bytes.at(3);

	int l_pal_size = p_bytes.at(4);

	for (int i{ 0 }; i < l_pal_size; ++i) {
		byte l_r = p_bytes.at(5 + 3 * i);
		byte l_g = p_bytes.at(5 + 3 * i + 1);
		byte l_b = p_bytes.at(5 + 3 * i + 2);
		m_palette.push_back(SDL_Color{ l_r, l_g, l_b });
	}

	if (bits_per_pixel() == 2) {

		for (std::size_t i{ static_cast<std::size_t>(5 + 3 * l_pal_size) }; i < p_bytes.size(); i += l_w / 4) {
			std::vector<byte> l_row;
			for (std::size_t j{ 0 }; j < l_w / 4; ++j) {
				byte b{ p_bytes[i + j] };
				l_row.push_back((b & 0b11000000) >> 6);
				l_row.push_back((b & 0b00110000) >> 4);
				l_row.push_back((b & 0b00001100) >> 2);
				l_row.push_back(b & 0b00000011);
			}
			m_pixels.push_back(l_row);
		}

	}
}

std::vector<byte> klib::gfx::KImage::to_bytes(void) const {
	std::vector<byte> result;
	// add image dimensions, width and height
	result.push_back(static_cast<byte>(w() / 256));
	result.push_back(static_cast<byte>(w() % 256));
	result.push_back(static_cast<byte>(h() / 256));
	result.push_back(static_cast<byte>(h() % 256));
	// add palette count
	result.push_back(static_cast<byte>(m_palette.size()));
	// add palette, 3 bytes per color
	for (const auto& col : m_palette) {
		result.push_back(col.r);
		result.push_back(col.g);
		result.push_back(col.b);
	}
	// add the actual encoded pixels
	if (bits_per_pixel() == 2) {
		for (const auto& l_prow : m_pixels)
			for (std::size_t i{ 0 }; i < l_prow.size(); i += 4)
				result.push_back(l_prow[i] * 64 + l_prow[i + 1] * 16 + l_prow[i + 2] * 4 + l_prow[i + 3]);
	}
	return result;
}

int klib::gfx::KImage::bits_per_pixel(void) const {
	if (m_palette.size() <= 2)
		return 1;
	else if (m_palette.size() <= 4)
		return 2;
	else if (m_palette.size() <= 16)
		return 4;
	else
		return 8;
}

int klib::gfx::KImage::w(void) const {
	return static_cast<int>(m_pixels.empty() ? 0 : m_pixels[0].size());
}

int klib::gfx::KImage::h(void) const {
	return static_cast<int>(m_pixels.size());
}

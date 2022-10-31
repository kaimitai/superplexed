#ifndef KIMAGE_H
#define KIMAGE_H

#include <SDL.h>
#include <string>
#include <vector>

using byte = unsigned char;

namespace klib {
	namespace gfx {

		class KImage {
			std::vector<SDL_Color> m_palette;
			std::vector<std::vector<byte>> m_pixels;

		public:
			KImage(const std::string& p_bmp_file);
			KImage(const std::vector<byte>& p_bytes, int p_w, int p_h, const std::vector<SDL_Color>& p_palette);
			SDL_Surface* to_sdl_surface(std::size_t p_transp_ind = 256) const;
			std::vector<byte> to_bytes(void) const;
			std::vector<SDL_Color> get_palette(void) const;
			int bits_per_pixel(void) const;
		};
	}
}

#endif

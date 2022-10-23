#ifndef KLIB_GFX_H
#define KLIB_GFX_H

#include <SDL.h>

namespace klib {

	namespace gfx {

		void put_pixel(SDL_Surface* srf, int x, int y, Uint32 pixel);

	}

}

#endif

#ifndef KLIB_GFX_H
#define KLIB_GFX_H

#include <SDL.h>
#include <vector>

namespace klib {

	namespace gfx {

		SDL_Texture* surface_to_texture(SDL_Renderer* p_rnd, SDL_Surface* p_srf, bool p_destroy_surface = true);
		std::vector<SDL_Texture*> split_surface(SDL_Renderer* p_rnd, SDL_Surface* srf, int p_w, int p_h, bool p_destroy_surface = true);

		void put_pixel(SDL_Surface* srf, int x, int y, Uint32 pixel);

	}

}

#endif

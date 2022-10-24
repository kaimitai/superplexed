#ifndef KLIB_GFX_H
#define KLIB_GFX_H

#include <SDL.h>
#include <vector>

namespace klib {

	namespace gfx {

		// texture blitting
		void blit(SDL_Renderer* p_rnd, SDL_Texture* p_texture, int p_x, int p_y);
		void blit_full_spec(SDL_Renderer* p_rnd, SDL_Texture* p_txt, int s_x, int s_y, int s_w, int s_h, int t_x, int t_y, int t_w, int t_h);

		// drawing
		void draw_rect(SDL_Renderer* p_rnd, int p_x, int p_y, int p_w, int p_h, SDL_Color p_color, int p_thickness);

		SDL_Texture* surface_to_texture(SDL_Renderer* p_rnd, SDL_Surface* p_srf, bool p_destroy_surface = true);
		std::vector<SDL_Texture*> split_surface(SDL_Renderer* p_rnd, SDL_Surface* srf, int p_w, int p_h, bool p_destroy_surface = true);
		std::vector<SDL_Texture*> split_surface_specified(SDL_Renderer* p_rnd, SDL_Surface* srf, const std::vector<SDL_Rect>& p_rects, bool p_destroy_surface = true);

		void put_pixel(SDL_Surface* srf, int x, int y, Uint32 pixel);

	}

}

#endif

#include "klib_gfx.h"

std::vector<SDL_Texture*> klib::gfx::split_surface(SDL_Renderer* rnd, SDL_Surface* full_surface, int p_w, int p_h, bool p_destroy_surface) {
	std::vector<SDL_Texture*> result;

	SDL_Rect tmpRectangle;
	tmpRectangle.w = p_w;
	tmpRectangle.h = p_h;

	for (int j = 0; j < full_surface->h; j += p_h)
		for (int i = 0; i < full_surface->w; i += p_w) {
			tmpRectangle.x = i;
			tmpRectangle.y = j;

			SDL_Surface* tmp = SDL_CreateRGBSurface(0, p_w, p_h, 24, 0, 0, 0, 0);
			//SDL_SetColorKey(tmp, true, SDL_MapRGB(tmp->format, p_trans_col.r, p_trans_col.g, p_trans_col.b));

			SDL_BlitSurface(full_surface, &tmpRectangle, tmp, nullptr);

			result.push_back(surface_to_texture(rnd, tmp, p_destroy_surface));
		}

	return(result);
}

SDL_Texture* klib::gfx::surface_to_texture(SDL_Renderer* p_rnd, SDL_Surface* p_srf, bool p_destroy_surface) {
	SDL_Texture* result = SDL_CreateTextureFromSurface(p_rnd, p_srf);
	if (p_destroy_surface)
		SDL_FreeSurface(p_srf);
	return(result);
}

void klib::gfx::put_pixel(SDL_Surface* srf, int x, int y, Uint32 pixel) {
	SDL_LockSurface(srf);

	int bpp = srf->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8* p = (Uint8*)srf->pixels + y * srf->pitch + x * bpp;

	switch (bpp) {
	case 1:
		*p = pixel;
		break;

	case 2:
		*(Uint16*)p = pixel;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		}
		else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;

	case 4:
		*(Uint32*)p = pixel;
		break;

	default:
		break;           /* shouldn't happen, but avoids warnings */
	}

	SDL_UnlockSurface(srf);
}

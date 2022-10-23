#include "SP_Gfx.h"
#include <tuple>

bool spgfx::save_bmp(const SP_Image& p_image, const SP_Palette& p_palette, const std::string& p_filename) {
	SDL_Surface* l_bmp = SDL_CreateRGBSurface(0, p_image.get_w(),
		p_image.get_h(), 8, 0, 0, 0, 0);

	SDL_Color out_palette[256];
	for (int i{ 0 }; i < static_cast<int>(p_palette.get_size()); ++i)
		out_palette[i] = SDL_Color{ std::get<0>(p_palette.get_color(i)),
	std::get<1>(p_palette.get_color(i)),
	std::get<2>(p_palette.get_color(i)) };

	SDL_SetPaletteColors(l_bmp->format->palette, out_palette, 0, 256);

	int file_status = SDL_SaveBMP(l_bmp, p_filename.c_str());

	SDL_FreeSurface(l_bmp);
	return (file_status != -1);

	return true;
}

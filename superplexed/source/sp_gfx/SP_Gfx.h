#ifndef SP_GFX_H
#define SP_GFX_H

#include <SDL.h>
#include "SP_Image.h"
#include "SP_Palette.h"
#include <string>

namespace spgfx {

	bool save_bmp(const SP_Image& p_image, const SP_Palette& p_palette, const std::string& p_filename);

}

#endif

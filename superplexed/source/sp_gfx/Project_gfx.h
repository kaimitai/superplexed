#ifndef SP_PROJECT_GFX_H
#define SP_PROJECT_GFX_H

#include <SDL.h>
#include <map>
#include <string>
#include <vector>
#include "SP_Palette.h"
#include "SP_Image.h"

class Project_gfx {

	std::vector<SDL_Texture*> m_static;
	std::vector<std::vector<SDL_Texture*>> m_animations;
	std::vector<SP_Palette> m_palettes;
	std::map<std::string, SP_Image> m_image_files;

	bool save_bmp(const SP_Image& p_image, const SP_Palette& p_palette, const std::string& p_filename);

public:
	Project_gfx(void);
	~Project_gfx(void);
	SDL_Texture* get_static(std::size_t p_texture_no);

	bool save_bmp(const std::string& p_filename);
};

#endif

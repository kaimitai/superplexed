#ifndef SP_GRAPHICS_WINDOW_H
#define SP_GRAPHICS_WINDOW_H

#include "./../SP_Config.h"
#include "./../sp_gfx/Project_gfx.h"
#include <string>

class Graphics_window {

	std::string m_selected_file;

public:
	Graphics_window(void) = default;
	void draw_ui(SDL_Renderer* p_rnd, Project_gfx& p_gfx, SP_Config& p_config);
	void set_selected_file(const std::string& p_filename);
};

#endif

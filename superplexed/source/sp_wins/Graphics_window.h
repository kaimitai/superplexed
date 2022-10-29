#ifndef SP_GRAPHICS_WINDOW_H
#define SP_GRAPHICS_WINDOW_H

#include "./../SP_Config.h"
#include "./../sp_gfx/Project_gfx.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <array>
#include <string>
#include <vector>

class Graphics_window {

	std::string m_selected_file;
	std::vector<std::vector<std::array<float, 4>>> m_palette_cache;
	int m_selected_palette;

	void set_project_gfx_palette(SDL_Renderer* p_rnd, Project_gfx& p_gfx);

public:
	Graphics_window(void);
	void draw_ui(SDL_Renderer* p_rnd, Project_gfx& p_gfx, SP_Config& p_config);
	void set_selected_file(const std::string& p_filename);
	void set_palette_cache(const Project_gfx& p_gfx);
};

#endif

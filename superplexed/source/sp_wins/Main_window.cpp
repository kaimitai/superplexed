#include "Main_window.h"
#include "Level_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"

Main_window::Main_window(SDL_Renderer* p_rnd, const SP_Config& p_config) :
	m_lvl_win{ p_rnd, p_config },
	m_gfx{ p_rnd, p_config },
	m_current_window{ 1 }
{ }

void Main_window::Main_window::move(int p_delta_ms, const klib::User_input& p_input, int p_w, int p_h) {
	switch (m_current_window) {
	case 1:
		m_lvl_win.move(p_delta_ms, p_input, p_w, p_h);
		break;
	default:
		break;
	}
}

void Main_window::draw(SDL_Renderer* p_rnd, const klib::User_input& p_input, int p_w, int p_h) {
	SDL_SetRenderDrawColor(p_rnd, 126, 126, 255, 0);
	SDL_RenderClear(p_rnd);
	switch (m_current_window) {
	case 1:
		m_lvl_win.draw(p_rnd, m_gfx, p_w, p_h);
		break;
	default:
		break;
	}

	draw_ui(p_input);
}

void Main_window::draw_ui(const klib::User_input& p_input) {
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	switch (m_current_window) {
	case 1:
		m_lvl_win.draw_ui(m_gfx, p_input);
		break;
	default:
		break;
	}

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

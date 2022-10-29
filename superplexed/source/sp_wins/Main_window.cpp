#include "Main_window.h"
#include "Level_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"

Main_window::Main_window(SDL_Renderer* p_rnd, SP_Config& p_config) :
	m_lvl_win{ p_rnd, p_config },
	m_gfx{ p_rnd, p_config },
	m_current_window{ 1 }
{
	// initialize selectable windows
	m_selectable_windows = { "Graphics", "Levels" };

	// initialize first filename in the gfx window for combobox aesthetics :)
	// cannot do that in the constructor of Graphics_window without refactoring
	m_gfx_win.set_selected_file(m_gfx.get_meta().begin()->first);
	m_gfx_win.set_palette_cache(m_gfx);

	// initialize output
	p_config.add_message("Read the documentation for the best experience!");
	p_config.add_message("Homepage: https://github.com/kaimitai/superplexed");
	p_config.add_message("Welcome to Superplexed by Kai E. Froeland");
}

void Main_window::Main_window::move(int p_delta_ms, const klib::User_input& p_input, SP_Config& p_config, int p_w, int p_h) {
	switch (m_current_window) {
	case 1:
		m_lvl_win.move(p_delta_ms, p_input, p_config, p_w, p_h);
		break;
	default:
		break;
	}
}

void Main_window::draw(SDL_Renderer* p_rnd, const klib::User_input& p_input, SP_Config& p_config, int p_w, int p_h) {
	SDL_SetRenderDrawColor(p_rnd, 126, 126, 255, 0);
	SDL_RenderClear(p_rnd);
	switch (m_current_window) {
	case 1:
		m_lvl_win.draw(p_rnd, m_gfx, p_w, p_h);
		break;
	default:
		break;
	}

	draw_ui(p_rnd, p_input, p_config);
}

void Main_window::draw_ui(SDL_Renderer* p_rnd, const klib::User_input& p_input, SP_Config& p_config) {
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	// draw the main window, this will be visible on all other screens
	ImGui::Begin("Main Window");

	if (ImGui::BeginCombo("Editor Mode", m_selectable_windows[m_current_window].c_str(), 0)) {
		for (std::size_t i{ 0 }; i < 2; ++i) {
			bool is_selected = (m_current_window == i);
			if (ImGui::Selectable(m_selectable_windows[i].c_str(), is_selected))
				m_current_window = i;
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();
	ImGui::Text("Output Messages");
	ImGui::Separator();
	for (const auto& msg : p_config.get_messages())
		ImGui::Text(msg.c_str());
	ImGui::End();

	switch (m_current_window) {
	case 0:
		m_gfx_win.draw_ui(p_rnd, m_gfx, p_config);
		break;
	case 1:
		m_lvl_win.draw_ui(m_gfx, p_input, p_config);
		break;
	default:
		break;
	}

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

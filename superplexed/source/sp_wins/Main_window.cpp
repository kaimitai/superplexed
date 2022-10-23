#include "Main_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"

Main_window::Main_window(SDL_Renderer* p_rnd) :
	m_gfx{ p_rnd },
	m_current_level{ 1 }
{
	auto l_bytes = klib::file::read_file_as_bytes("./gamedata/LEVELS.DAT");

	for (std::size_t i{ 0 }; i < l_bytes.size(); i += 1536)
		m_levels.push_back(SP_Level(std::vector<byte>(begin(l_bytes) + i, begin(l_bytes) + i + 1536)));
}

void Main_window::Main_window::move(int p_delta_ms, const klib::User_input& p_input) {
	if (p_input.mw_down() && m_current_level > 0)
		--m_current_level;
	else if (p_input.mw_up() && m_current_level < m_levels.size() - 1)
		++m_current_level;
}

void Main_window::draw(SDL_Renderer* p_rnd, int p_w, int p_h) {
	SDL_RenderClear(p_rnd);

	for (int i = 0; i < 60; ++i)
		for (int j = 0; j < 24; ++j)
			klib::gfx::blit(p_rnd, m_gfx.get_static(m_levels.at(m_current_level).get_tile_no(i, j)), i * 16, j * 16);

	auto l_spos = m_levels.at(m_current_level).get_start_pos();
	klib::gfx::blit(p_rnd, m_gfx.get_static(3), 16 * l_spos.first, 16 * l_spos.second);

	draw_ui();
}

void Main_window::draw_ui(void) {
	ImGui_ImplSDLRenderer_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	auto l_lvl_label{ "Level " + m_levels.at(m_current_level).get_title() + "###levels" };
	ImGui::Begin(l_lvl_label.c_str());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

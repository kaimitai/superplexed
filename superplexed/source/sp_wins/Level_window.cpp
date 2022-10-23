#include "Level_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <algorithm>

Level_window::Level_window(SDL_Renderer* p_rnd) :
	m_current_level{ 1 }, m_cam_x{ 0 }
{
	m_texture = SDL_CreateTexture(p_rnd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 60 * 16, 24 * 16);
	auto l_bytes = klib::file::read_file_as_bytes("./gamedata/LEVELS.DAT");

	for (std::size_t i{ 0 }; i < l_bytes.size(); i += 1536)
		m_levels.push_back(SP_Level(std::vector<byte>(begin(l_bytes) + i, begin(l_bytes) + i + 1536)));
}

void Level_window::move(int p_delta_ms, const klib::User_input& p_input) {
	if (p_input.mw_down() && m_current_level > 0)
		--m_current_level;
	else if (p_input.mw_up() && m_current_level < m_levels.size() - 1)
		++m_current_level;
}

void Level_window::draw(SDL_Renderer* p_rnd, const Project_gfx& p_gfx, int p_w, int p_h) {
	regenerate_texture(p_rnd, p_gfx);
	SDL_RenderClear(p_rnd);

	// get tile pixel width
	int l_tpw = get_tile_pixel_w(p_h);
	// get screen tile widht
	int l_stw = p_w / l_tpw + 1;
	// get rest tile count
	int l_stw_tc = std::min<int>(60 - m_cam_x, l_stw);

	klib::gfx::blit_full_spec(p_rnd, m_texture,
		0, 0, l_stw_tc * l_tpw, p_h,
		16 * static_cast<int>(m_cam_x), 0, 16 * l_stw_tc, 16 * 24);
}

void Level_window::regenerate_texture(SDL_Renderer* p_rnd, const Project_gfx& p_gfx) {
	SDL_SetRenderTarget(p_rnd, m_texture);
	SDL_SetRenderDrawColor(p_rnd, 0, 0, 0, 0);
	SDL_RenderClear(p_rnd);

	for (int i = 0; i < 60; ++i)
		for (int j = 0; j < 24; ++j)
			klib::gfx::blit(p_rnd, p_gfx.get_static(m_levels.at(m_current_level).get_tile_no(i, j)), i * 16, j * 16);

	auto l_spos = m_levels.at(m_current_level).get_start_pos();
	klib::gfx::blit(p_rnd, p_gfx.get_static(3), 16 * l_spos.first, 16 * l_spos.second);

	SDL_SetRenderTarget(p_rnd, nullptr);
}


void Level_window::draw_ui(void) {
	auto l_lvl_label{ "Level " + m_levels.at(m_current_level).get_title() + "###levels" };
	ImGui::Begin(l_lvl_label.c_str());
	ImGui::End();
}

int Level_window::get_tile_pixel_w(int p_screen_pixel_h) const {
	return static_cast<int>(static_cast<float>(p_screen_pixel_h) / 24.0f);
}

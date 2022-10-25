#include "Level_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/klib/klib_util.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include <algorithm>

Level_window::Level_window(SDL_Renderer* p_rnd) :
	m_current_level{ 1 }, m_current_gp{ 1 }, m_cam_x{ 0 },
	m_ui_show_grid{ false }, m_ui_animate{ true },
	m_sel_x{ 0 }, m_sel_y{ 0 }, m_sel_x2{ -1 }, m_sel_y2{ 0 },
	m_sel_tile{ 0 },
	m_timer{ klib::Timer(6, 250) }
{
	m_texture = SDL_CreateTexture(p_rnd, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 60 * 16, 24 * 16);
	auto l_bytes = klib::file::read_file_as_bytes("./gamedata/LEVELS.DAT");

	for (std::size_t i{ 0 }; i < l_bytes.size(); i += 1536)
		m_levels.push_back(SP_Level(std::vector<byte>(begin(l_bytes) + i, begin(l_bytes) + i + 1536)));
}

void Level_window::move(int p_delta_ms, const klib::User_input& p_input, int p_w, int p_h) {
	bool l_shift = p_input.is_shift_pressed();
	bool l_ctrl = p_input.is_ctrl_pressed();
	m_timer.move(p_delta_ms);

	// handle mouse/keyboard if no imgui windows are in focus
	if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
		return;

	// handle keyboard
	if (p_input.is_pressed(SDL_SCANCODE_DELETE))
		delete_selection();
	else if (l_ctrl && p_input.is_pressed(SDL_SCANCODE_C))
		copy_to_clipboard();
	else if (l_ctrl && p_input.is_pressed(SDL_SCANCODE_V)) {
		if (selection_fits())
			paste_from_clipboard();
	}
	else if (l_ctrl && p_input.is_pressed(SDL_SCANCODE_X))
		cut_selection();
	else if (p_input.is_pressed(SDL_SCANCODE_F))
		flip_selection(l_shift);
	else if (l_ctrl && p_input.is_pressed(SDL_SCANCODE_A))
		select_all();
	else if (l_shift && p_input.is_pressed(SDL_SCANCODE_V))
		show_clipboard_destination();
	else if (p_input.is_pressed(SDL_SCANCODE_R))
		rotate_selection(l_shift);

	if (p_input.mouse_held()) {
		auto tcoords = mouse_coords_to_tile(p_input.mx(), p_input.my(), p_h);

		if (l_ctrl) {
			m_sel_tile = m_levels.at(get_current_level_idx()).get_tile_no(tcoords.first, tcoords.second);
		}
		if (!l_ctrl && l_shift) {
			m_sel_x2 = tcoords.first;
			m_sel_y2 = tcoords.second;
		}
		else {
			clear_selection();
			m_sel_x = tcoords.first;
			m_sel_y = tcoords.second;
		}
	}
	else if (p_input.mouse_held(false)) {
		auto tcoords = mouse_coords_to_tile(p_input.mx(), p_input.my(), p_h);
		if (m_sel_tile == 3)
			m_levels.at(get_current_level_idx()).set_player_start(tcoords.first, tcoords.second);
		else
			m_levels.at(get_current_level_idx()).set_tile_value(tcoords.first, tcoords.second, m_sel_tile);
	}
	else if (p_input.mw_down()) {
		if (l_shift && get_current_level_idx() > 0)
			--m_current_level;
		else if (!l_shift && m_cam_x > 0)
			--m_cam_x;
	}
	else if (p_input.mw_up()) {
		if (l_shift && get_current_level_idx() < m_levels.size() - 1)
			++m_current_level;
		else if (!l_shift && m_cam_x < 40)
			++m_cam_x;
	}
}

void Level_window::draw(SDL_Renderer* p_rnd, const Project_gfx& p_gfx, int p_w, int p_h) {
	regenerate_texture(p_rnd, p_gfx);

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
		for (int j = 0; j < 24; ++j) {
			byte l_tile_no = m_levels.at(get_current_level_idx()).get_tile_no(i, j);
			klib::gfx::blit(p_rnd,
				m_ui_animate ? p_gfx.get_animated(l_tile_no, m_timer.get_frame()) :
				p_gfx.get_static(l_tile_no),
				i * 16, j * 16);
		}

	auto l_spos = m_levels.at(get_current_level_idx()).get_start_pos();
	klib::gfx::blit(p_rnd,
		m_ui_animate ? p_gfx.get_animated(3, m_timer.get_frame()) :
		p_gfx.get_static(3),
		16 * l_spos.first, 16 * l_spos.second);

	auto l_rect = this->get_selection_rectangle();
	klib::gfx::draw_rect(p_rnd, 16 * l_rect.x, 16 * l_rect.y, 16 * (l_rect.w + 1), 16 * (l_rect.h + 1),
		SDL_Color{ 200, 200, 50 }, 3);

	SDL_SetRenderTarget(p_rnd, nullptr);
}

int Level_window::get_tile_pixel_w(int p_screen_pixel_h) const {
	return static_cast<int>(static_cast<float>(p_screen_pixel_h) / 24.0f);
}

std::size_t Level_window::get_current_level_idx(void) const {
	return static_cast<std::size_t>(m_current_level) - 1;
}

// selections
bool Level_window::has_selection(void) const {
	return (m_sel_x2 != -1);
}

void Level_window::clear_selection(void) {
	m_sel_x2 = -1;
}

SDL_Rect Level_window::get_selection_rectangle(void) const {
	if (has_selection()) {
		int l_x = std::min(m_sel_x, m_sel_x2);
		int l_y = std::min(m_sel_y, m_sel_y2);
		int l_w = std::max(m_sel_x2, m_sel_x) - l_x;
		int l_h = std::max(m_sel_y2, m_sel_y) - l_y;

		return SDL_Rect{ l_x, l_y, l_w, l_h };
	}
	else
		return SDL_Rect{ m_sel_x, m_sel_y, 0, 0 };
}

bool Level_window::is_selection_empty(void) const {
	auto l_rect = get_selection_rectangle();

	for (int j{ l_rect.y }; j <= l_rect.y + l_rect.h; ++j)
		for (int i{ l_rect.x }; i <= l_rect.x + l_rect.w; ++i)
			if (m_levels.at(get_current_level_idx()).get_tile_no(i, j) != 0)
				return false;

	return true;
}

void Level_window::flip_selection(bool p_vertical) {
	auto l_rect = get_selection_rectangle();

	// vertical flip
	if (p_vertical) {
		for (int j{ 0 }; j <= l_rect.w; ++j)
			for (int i{ 0 }; i <= l_rect.h / 2; ++i) {
				byte l_tmp = m_levels.at(get_current_level_idx()).get_tile_no(l_rect.x + j, l_rect.y + i);
				m_levels.at(get_current_level_idx()).set_tile_value(l_rect.x + j, l_rect.y + i,
					m_levels.at(get_current_level_idx()).get_tile_no(l_rect.x + j, l_rect.y + l_rect.h - i));
				m_levels.at(get_current_level_idx()).set_tile_value(l_rect.x + j, l_rect.y + l_rect.h - i, l_tmp);
			}
	}
	// horizontal flip
	else {
		for (int j{ 0 }; j <= l_rect.w / 2; ++j)
			for (int i{ 0 }; i <= l_rect.h; ++i) {
				byte l_tmp = m_levels.at(get_current_level_idx()).get_tile_no(l_rect.x + j, l_rect.y + i);
				m_levels.at(get_current_level_idx()).set_tile_value(l_rect.x + j, l_rect.y + i,
					m_levels.at(get_current_level_idx()).get_tile_no(l_rect.x + l_rect.w - j, l_rect.y + i));
				m_levels.at(get_current_level_idx()).set_tile_value(l_rect.x + l_rect.w - j, l_rect.y + i, l_tmp);
			}
	}
}

void Level_window::select_all(void) {
	m_sel_x = 0;
	m_sel_y = 0;
	m_sel_x2 = 59;
	m_sel_y2 = 23;
}

void Level_window::show_clipboard_destination(void) {
	if (!m_clipboard.empty() && selection_fits()) {
		m_sel_x2 = m_sel_x + static_cast<int>(m_clipboard[0].size()) - 1;
		m_sel_y2 = m_sel_y + static_cast<int>(m_clipboard.size()) - 1;
	}
}

bool Level_window::selection_fits(void) const {
	return (m_sel_y + m_clipboard.size() <= 24) && (m_clipboard.size() == 0 || (m_sel_x + m_clipboard[0].size() <= 60));
}

void Level_window::copy_to_clipboard(void) {
	auto l_rect = get_selection_rectangle();
	m_clipboard.clear();

	for (int j{ l_rect.y }; j <= l_rect.y + l_rect.h; ++j) {
		std::vector<byte> l_row;
		for (int i{ l_rect.x }; i <= l_rect.x + l_rect.w; ++i)
			l_row.push_back(m_levels.at(get_current_level_idx()).get_tile_no(i, j));
		m_clipboard.push_back(l_row);
	}

}

void Level_window::paste_from_clipboard(void) {
	for (int j{ 0 }; j < m_clipboard.size(); ++j)
		for (int i{ 0 }; i < m_clipboard.at(j).size(); ++i)
			m_levels.at(get_current_level_idx()).set_tile_value(
				m_sel_x + i, m_sel_y + j, m_clipboard[j][i]);
}

void Level_window::delete_selection(void) {
	auto l_rect = get_selection_rectangle();

	for (int j{ l_rect.y }; j <= l_rect.y + l_rect.h; ++j)
		for (int i{ l_rect.x }; i <= l_rect.x + l_rect.w; ++i)
			m_levels.at(get_current_level_idx()).set_tile_value(i, j, 0);
}

void Level_window::cut_selection(void) {
	if (!is_selection_empty()) {
		copy_to_clipboard();
		delete_selection();
	}
}

void Level_window::rotate_selection(bool p_clockwise) {
	if (m_clipboard.empty())
		return;

	std::vector<std::vector<byte>> result;

	if (p_clockwise) {
		for (int j{ static_cast<int>(m_clipboard[0].size()) - 1 }; j >= 0; --j) {
			std::vector<byte> l_row;
			for (int i{ 0 }; i < m_clipboard.size(); ++i) {
				l_row.push_back(m_clipboard[i][j]);
			}
			result.push_back(l_row);
		}
	}
	else {
		for (int j{ 0 }; j < static_cast<int>(m_clipboard[0].size()); ++j) {
			std::vector<byte> l_row;
			for (int i{ 0 }; i < m_clipboard.size(); ++i) {
				l_row.push_back(m_clipboard[static_cast<std::size_t>(m_clipboard.size() - i - 1)][j]);
			}
			result.push_back(l_row);
		}
	}

	m_clipboard = result;
}

std::pair<int, int> Level_window::mouse_coords_to_tile(int p_mouse_x, int p_mouse_y, int p_screen_h) const {
	int l_tpw = get_tile_pixel_w(p_screen_h);

	int l_tx = m_cam_x + p_mouse_x / l_tpw;
	int l_ty = p_mouse_y / l_tpw;

	return std::make_pair(
		klib::util::validate(l_tx, 0, 59),
		klib::util::validate(l_ty, 0, 23)
	);
}

#include "Main_window.h"
#include "Level_window.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"
#include "./../SP_Constants.h"
#include <filesystem>

Main_window::Main_window(SDL_Renderer* p_rnd, SP_Config& p_config) :
	m_lvl_win{ p_rnd, p_config },
	m_gfx{ p_rnd, p_config },
	m_savefile_win{ p_config },
	m_current_window{ 1 },
	m_selected_file{ SP_Config::get_default_levels_filename() }
{
	// initialize selectable windows
	m_selectable_windows = { "Graphics", "Levels", "Savefiles" };

	// initialize first filename in the gfx window for combobox aesthetics :)
	// cannot do that in the constructor of Graphics_window without refactoring
	m_gfx_win.set_selected_file(m_gfx.get_meta().begin()->first);
	m_gfx_win.set_palette_cache(m_gfx);

	// scan directory for levelset files
	m_levelset_files = get_levelset_files(p_config);

	// initialize output
	p_config.add_message("Build Date: " + std::string(__DATE__) + ' ' +
		std::string(__TIME__) + " CET");
	p_config.add_message("Homepage: https://github.com/kaimitai/superplexed");
	p_config.add_message("Read the documentation for efficient usage tips!");
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
	ImGui::SetNextWindowPos(ImVec2(c::WIN_MAIN_X, c::WIN_MAIN_Y), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(c::WIN_MAIN_W, c::WIN_MAIN_H), ImGuiCond_FirstUseEver);
	window_start("Main Window", c::COL_WHITE, c::COL_BLUE_DARK, c::COL_BLUE_LIGHT, c::COL_BLUE_LIGHT);

	if (ImGui::BeginCombo("Editor Mode", m_selectable_windows[m_current_window].c_str(), 0)) {
		for (std::size_t i{ 0 }; i < m_selectable_windows.size(); ++i) {
			bool is_selected = (m_current_window == i);
			if (ImGui::Selectable(m_selectable_windows[i].c_str(), is_selected))
				m_current_window = i;
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();

	if (ImGui::BeginCombo("Levelset File", m_selected_file.c_str(), 0)) {
		for (const auto& l_filename : m_levelset_files) {
			bool is_selected = (l_filename == m_selected_file);
			if (ImGui::Selectable(l_filename.c_str(), is_selected)) {
				m_selected_file = l_filename;
				p_config.set_level_list_filename(m_selected_file);
			}
		}

		ImGui::EndCombo();
	}
	if (ImGui::Button("Refresh File List")) {
		m_levelset_files = get_levelset_files(p_config);
		if (std::find(begin(m_levelset_files), end(m_levelset_files), m_selected_file)
			== end(m_levelset_files)) {
			m_selected_file = SP_Config::get_default_levels_filename();
			p_config.set_level_list_filename(m_selected_file);
		}

		p_config.add_message("Refreshed levelset file list");
	}

	ImGui::SameLine();

	if (ImGui::Button("Reload Configuration"))
		p_config.load_configuration();

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
	case 2:
		m_savefile_win.draw_ui(p_config);
		break;
	default:
		break;
	}

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

void Main_window::set_application_icon(SDL_Window* p_win) const {
	SDL_Surface* l_icon_srf{ m_gfx.create_application_icon() };
	SDL_SetWindowIcon(p_win, l_icon_srf);
	SDL_FreeSurface(l_icon_srf);
}

void Main_window::window_start(const std::string& p_title, c::SP_Color p_text, c::SP_Color p_active, c::SP_Color p_inactive, c::SP_Color p_collapsed) {
	ImGui::PushStyleColor(ImGuiCol_TitleBg, sp_color_to_imgui(p_inactive));
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, sp_color_to_imgui(p_active));
	ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, sp_color_to_imgui(p_collapsed));
	ImGui::PushStyleColor(ImGuiCol_Text, sp_color_to_imgui(p_text));
	ImGui::Begin(p_title.c_str());
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}

constexpr ImU32 Main_window::sp_color_to_imgui(c::SP_Color p_color) {
	return IM_COL32(p_color.r, p_color.g, p_color.b, 255);
}

// read current directory and get candidate filenames
std::vector<std::string> Main_window::get_levelset_files(const SP_Config& p_config) {
	std::vector<std::string> result;

	for (const auto& ll_file :
		std::filesystem::directory_iterator(p_config.get_project_folder())) {
		if (ll_file.is_regular_file()) {
			std::filesystem::path l_file{ ll_file.path() };
			std::string l_ext{ l_file.extension().string() };
			std::string l_name{ l_file.filename().string() };
			std::transform(begin(l_ext), end(l_ext), begin(l_ext), ::toupper);
			std::transform(begin(l_name), end(l_name), begin(l_name), ::toupper);

			if (l_name.substr(0, l_name.find_first_of('.')) == c::FILENAME_LEVELS &&
				l_ext.size() == 4 &&
				l_ext[1] == 'D' &&
				(l_ext[2] >= '0' && l_ext[2] <= '9') &&
				(l_ext[3] >= '0' && l_ext[3] <= '9'))
				result.push_back(l_name);
		}
	}
	std::sort(begin(result), end(result));
	result.insert(begin(result), SP_Config::get_default_levels_filename());

	return result;
}

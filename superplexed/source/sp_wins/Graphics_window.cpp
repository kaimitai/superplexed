#include <SDL.h>
#include "Graphics_window.h"
#include "./../common/imgui/imgui.h"
#include "./../common/imgui/imgui_impl_sdl.h"
#include "./../common/imgui/imgui_impl_sdlrenderer.h"

void Graphics_window::set_selected_file(const std::string& p_filename) {
	m_selected_file = p_filename;
}

void Graphics_window::draw_ui(SDL_Renderer* p_rnd, Project_gfx& p_gfx, SP_Config& p_config) {

	ImGui::Begin("Images");
	const auto& l_meta = p_gfx.get_meta();
	if (ImGui::BeginCombo("Image File", m_selected_file.c_str(), 0)) {
		for (const auto& kv : l_meta) {
			bool is_selected = (kv.first == m_selected_file);
			if (ImGui::Selectable(kv.first.c_str(), is_selected))
				m_selected_file = kv.first;
		}

		ImGui::EndCombo();
	}

	if (ImGui::Button("Load DAT")) {
		try {
			p_gfx.load_image_data_from_file(p_rnd, m_selected_file, p_config);
			p_config.add_message("Loaded " + p_config.get_dat_full_path(m_selected_file));
		}
		catch (const std::exception& ex) {
			p_config.add_message("Could not load file: " + std::string(ex.what()));
		}
	}

	if (ImGui::Button("Save DAT")) {
		try {
			p_gfx.save_dat(m_selected_file, p_config);
			p_config.add_message("Saved " + p_config.get_dat_full_path(m_selected_file));
		}
		catch (const std::exception& ex) {
			p_config.add_message("Exception: " + std::string(ex.what()));
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Export bmp")) {
		if (p_gfx.save_bmp(m_selected_file, p_config))
			p_config.add_message("Saved " + p_config.get_bmp_full_path(m_selected_file));
		else
			p_config.add_message("Could not save bmp");
	}

	ImGui::End();


	ImGui::Begin("Image Preview");

	SDL_Texture* l_txt = p_gfx.get_image_texture(m_selected_file);
	if (l_txt == nullptr)
		ImGui::Text("Image not loaded");
	else {
		ImVec2 l_wmin = ImGui::GetWindowContentRegionMin();
		ImVec2 l_wmax = ImGui::GetWindowContentRegionMax();

		ImGui::Image(l_txt, { l_wmax.x - l_wmin.x, l_wmax.y - l_wmin.y });
	}

	ImGui::End();
}

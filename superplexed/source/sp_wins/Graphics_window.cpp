#include <SDL.h>
#include <tuple>
#include "Graphics_window.h"
#include "./../SP_Constants.h"

void Graphics_window::set_project_gfx_palette(Project_gfx& p_gfx) {
	std::vector<SP_Palette> l_palettes;

	for (std::size_t i{ 0 }; i < c::PALETTE_COUNT; ++i) {
		std::vector<byte> l_palette;

		for (const auto& fcol : m_palette_cache.at(i)) {
			for (int j{ 0 }; j < 4; ++j)
				l_palette.push_back(static_cast<byte>(fcol[j] * 16.0f));
		}
		l_palettes.push_back(SP_Palette(l_palette));
	}

	p_gfx.set_palettes(l_palettes);
}

Graphics_window::Graphics_window(void) : m_selected_palette{ 1 }
{}

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

	ImGui::Separator();
	ImGui::Text("Image Metadata");
	ImGui::Separator();

	int l_using_palette{ l_meta.at(m_selected_file).m_palette_no };
	bool l_binary_img{ l_meta.at(m_selected_file).m_binary };
	std::string l_meta_str{ "Dimensions (w,h): " + std::to_string(l_meta.at(m_selected_file).m_width) +
	'x' + std::to_string(l_meta.at(m_selected_file).m_height) + "\nPalette: " +
	(l_using_palette >= 4 ? "Fixed" : std::to_string(l_using_palette + 1)) +
		"\nBits per pixel: " + (l_binary_img ? '1' : '4')
	};

	ImGui::Text(l_meta_str.c_str());

	ImGui::Separator();
	ImGui::Text("File Operations");
	ImGui::Separator();

	if (ImGui::Button("Load DAT")) {
		try {
			p_gfx.load_image_data_from_file(p_rnd, m_selected_file, p_config);
			p_config.add_message("Loaded " + p_config.get_dat_full_path(m_selected_file));
		}
		catch (const std::exception& ex) {
			p_config.add_message("Could not load file: " + std::string(ex.what()));
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Load bmp")) {
		try {
			p_gfx.load_bmp(p_rnd, p_config, m_selected_file);
			p_config.add_message("Loaded and recolored " + p_config.get_bmp_full_path(m_selected_file));
		}
		catch (const std::exception& ex) {
			p_config.add_message(std::string(ex.what()));
			p_config.add_message("Could not load " + p_config.get_bmp_full_path(m_selected_file));
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
	if (ImGui::Button("Save bmp")) {
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

	ImGui::Begin("Palettes");
	ImGui::SliderInt("Palette", &m_selected_palette, 1, c::PALETTE_COUNT);

	ImGui::Separator();
	ImGui::Text("Palette Colors");
	ImGui::Separator();

	for (int i{ 0 }; i < 4; ++i) {
		for (int j{ 0 }; j < 4; ++j) {
			std::string label = "Color #" + std::to_string(i * 4 + j + 1);
			ImGui::ColorEdit3(label.c_str(), &m_palette_cache[m_selected_palette - 1][i * 4 + j][0],
				ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_NoInputs);
			ImGui::SameLine();
		}
		ImGui::NewLine();
	}

	if (ImGui::Button("Save DAT")) {
		std::string l_out_file = p_config.get_dat_full_path("PALETTES");

		try {
			p_gfx.save_palettes_dat(p_config);
			p_config.add_message("Saved palettes to " + l_out_file);
		}
		catch (const std::exception& ex) {
			p_config.add_message("Could not save " + l_out_file);
		}
	} if (ImGui::Button("Apply Palettes")) {
		set_project_gfx_palette(p_gfx);
		set_palette_cache(p_gfx);
		p_config.add_message("Applied palettes");
	}

	ImGui::End();
}

// set palette cache
void Graphics_window::set_palette_cache(const Project_gfx& p_gfx) {
	const auto& palettes = p_gfx.get_palettes();
	m_palette_cache.clear();

	for (const auto& pal : palettes) {
		std::vector<std::array<float, 4>> l_row;

		for (int i{ 0 }; i < pal.get_size(); ++i) {
			const auto& l_col = pal.get_color(i);
			l_row.push_back(std::array<float, 4>({
				static_cast<float>(std::get<0>(l_col)) / 255.0f,
				static_cast<float>(std::get<1>(l_col)) / 255.0f,
				static_cast<float>(std::get<2>(l_col)) / 255.0f,
				static_cast<float>(std::get<3>(l_col)) / 255.0f
				}
			));
		}

		m_palette_cache.push_back(l_row);
	}

}

#include "Project_gfx.h"
#include "./../common/pugixml/pugixml.hpp"
#include "./../common/pugixml/pugiconfig.hpp"
#include "./../common/klib/klib_util.h"
#include "./../SP_Constants.h"
#include "./../SP_Config.h"
#include <filesystem>
#include <stdexcept>

void Project_gfx::save_palette_xml(const SP_Config& p_config) const {
	pugi::xml_document doc;
	auto n_comments = doc.append_child(pugi::node_comment);
	n_comments.set_value(c::XML_COMMENTS_PALETTE);

	auto n_metadata = doc.append_child(c::XML_TAG_META);
	n_metadata.append_attribute(c::XML_ATTR_APP_VERSION);
	n_metadata.attribute(c::XML_ATTR_APP_VERSION).set_value(c::APP_VERSION);

	auto n_palettes = n_metadata.append_child(c::XML_TAG_PALETTES);
	for (std::size_t i{ 0 }; i < c::PALETTE_COUNT; ++i) {
		auto n_palette = n_palettes.append_child(c::XML_TAG_PALETTE);

		for (std::size_t j{ 0 }; j < m_palettes[i].get_size(); ++j) {
			auto n_col = n_palette.append_child(c::XML_TAG_COLOR);
			const auto& l_color = m_palettes[i].get_color(static_cast<int>(j));

			n_col.append_attribute(c::XML_ATTR_NO);
			n_col.attribute(c::XML_ATTR_NO).set_value(j + 1);
			n_col.append_attribute(c::XML_ATTR_R);
			n_col.attribute(c::XML_ATTR_R).set_value(std::get<0>(l_color) / 16);
			n_col.append_attribute(c::XML_ATTR_G);
			n_col.attribute(c::XML_ATTR_G).set_value(std::get<1>(l_color) / 16);
			n_col.append_attribute(c::XML_ATTR_B);
			n_col.attribute(c::XML_ATTR_B).set_value(std::get<2>(l_color) / 16);
			n_col.append_attribute(c::XML_ATTR_A);
			n_col.attribute(c::XML_ATTR_A).set_value(std::get<3>(l_color) / 16);

		}
	}

	std::filesystem::create_directory(p_config.get_gfx_xml_folder());
	if (!doc.save_file(p_config.get_gfx_xml_full_path("PALETTES").c_str()))
		throw std::runtime_error("Could not save XML");
}

void Project_gfx::save_image_xml(const SP_Config& p_config, const std::string& p_filename) const {
	if (m_image_files.find(p_filename) == end(m_image_files))
		throw std::runtime_error("Image not loaded");

	pugi::xml_document doc;
	auto n_comments = doc.append_child(pugi::node_comment);
	n_comments.set_value(c::XML_COMMENTS_IMAGE);

	auto n_metadata = doc.append_child(c::XML_TAG_META);
	n_metadata.append_attribute(c::XML_ATTR_APP_VERSION);
	n_metadata.attribute(c::XML_ATTR_APP_VERSION).set_value(c::APP_VERSION);

	auto n_image = n_metadata.append_child(c::XML_TAG_IMAGE);
	n_image.append_attribute(c::XML_ATTR_IMGTYPE);
	n_image.attribute(c::XML_ATTR_IMGTYPE).set_value(
		m_image_files.at(p_filename).is_binary() ? c::XML_VAL_BINARY : c::XML_VAL_PLANAR);
	n_image.append_attribute(c::XML_ATTR_UNKNOWN);
	n_image.attribute(c::XML_ATTR_UNKNOWN).set_value(
		klib::util::string_join(m_image_files.at(p_filename).get_unknown_bytes(), ',').c_str());
	n_image.append_attribute(c::XML_ATTR_WIDTH);
	n_image.attribute(c::XML_ATTR_WIDTH).set_value(m_image_files.at(p_filename).get_w());
	n_image.append_attribute(c::XML_ATTR_HEIGHT);
	n_image.attribute(c::XML_ATTR_HEIGHT).set_value(m_image_files.at(p_filename).get_h());

	auto n_pixel_rows = n_image.append_child(c::XML_TAG_PIXEL_ROWS);
	for (int i{ 0 }; i < m_image_files.at(p_filename).get_h(); ++i) {
		std::vector<byte> l_pixel_row;
		for (int j{ 0 }; j < m_image_files.at(p_filename).get_w(); ++j)
			l_pixel_row.push_back(m_image_files.at(p_filename).get_palette_index(j, i));
		auto n_pixel_row = n_pixel_rows.append_child(c::XML_TAG_PIXEL_ROW);
		n_pixel_row.append_attribute(c::XML_ATTR_NO);
		n_pixel_row.attribute(c::XML_ATTR_NO).set_value(i + 1);
		n_pixel_row.append_attribute(c::XML_ATTR_VALUE);
		auto l_string_value{ klib::util::string_join<byte>(l_pixel_row, ',') };
		n_pixel_row.attribute(c::XML_ATTR_VALUE).set_value(l_string_value.c_str());
	}

	std::filesystem::create_directory(p_config.get_gfx_xml_folder());
	if (!doc.save_file(p_config.get_gfx_xml_full_path(p_filename).c_str()))
		throw std::runtime_error("Could not save XML");
}

void Project_gfx::load_palette_xml(SDL_Renderer* p_rnd, const SP_Config& p_config) {
	pugi::xml_document doc;
	if (!doc.load_file(p_config.get_gfx_xml_full_path("PALETTES").c_str()))
		throw std::runtime_error("Could not load xml");

	std::vector<SP_Palette> l_palettes;
	pugi::xml_node n_meta = doc.child(c::XML_TAG_META);
	pugi::xml_node n_palettes = n_meta.child(c::XML_TAG_PALETTES);

	for (auto n_palette = n_palettes.child(c::XML_TAG_PALETTE);
		n_palette; n_palette = n_palette.next_sibling(c::XML_TAG_PALETTE)) {
		std::vector<byte> l_pal_bytes;
		for (auto n_color = n_palette.child(c::XML_TAG_COLOR); n_color;
			n_color = n_color.next_sibling(c::XML_TAG_COLOR)) {
			l_pal_bytes.push_back(n_color.attribute(c::XML_ATTR_R).as_int());
			l_pal_bytes.push_back(n_color.attribute(c::XML_ATTR_G).as_int());
			l_pal_bytes.push_back(n_color.attribute(c::XML_ATTR_B).as_int());
			l_pal_bytes.push_back(n_color.attribute(c::XML_ATTR_A).as_int());
		}
		l_palettes.push_back(SP_Palette(l_pal_bytes));
	}

	m_palettes = l_palettes;
	load_fixed_palettes();
	regenerate_all_textures(p_rnd);
}

void Project_gfx::load_image_xml(SDL_Renderer* p_rnd, const SP_Config& p_config, const std::string& p_filename) {
	pugi::xml_document doc;
	if (!doc.load_file(p_config.get_gfx_xml_full_path(p_filename).c_str()))
		throw std::runtime_error("Could not load xml");

	pugi::xml_node n_meta = doc.child(c::XML_TAG_META);
	auto n_image = n_meta.child(c::XML_TAG_IMAGE);
	bool l_binary = strcmp(c::XML_VAL_BINARY, n_image.attribute(c::XML_ATTR_IMGTYPE).as_string()) == 0;
	std::vector<byte> l_unknown_bytes =
		klib::util::string_split<byte>(n_image.attribute(c::XML_ATTR_UNKNOWN).as_string(), ',');

	std::vector<std::vector<byte>> l_pixels;
	auto n_pixel_rows = n_image.child(c::XML_TAG_PIXEL_ROWS);

	for (auto n_pixels = n_pixel_rows.child(c::XML_TAG_PIXEL_ROW);
		n_pixels;
		n_pixels = n_pixels.next_sibling(c::XML_TAG_PIXEL_ROW)) {
		l_pixels.push_back(klib::util::string_split<byte>(
			n_pixels.attribute(c::XML_ATTR_VALUE).as_string(), ','
			));
	}

	auto l_sp_image = SP_Image(l_pixels, l_unknown_bytes, l_binary);
	m_image_files.erase(p_filename);
	m_image_files.insert(std::make_pair(p_filename, l_sp_image));

	regenerate_texture(p_rnd, p_filename);
}

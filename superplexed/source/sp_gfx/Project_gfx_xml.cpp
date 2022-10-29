#include "Project_gfx.h"
#include "./../common/pugixml/pugixml.hpp"
#include "./../common/pugixml/pugiconfig.hpp"
#include "./../common/klib/klib_util.h"
#include "./../SP_Constants.h"
#include "./../SP_Config.h"
#include <filesystem>

void Project_gfx::save_image_xml(const SP_Config& p_config, const std::string& p_filename) const {
	if (m_image_files.find(p_filename) == end(m_image_files))
		throw std::exception("Image not loaded");

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

	std::filesystem::create_directory(p_config.get_xml_folder());
	if (!doc.save_file(p_config.get_xml_full_path(p_filename).c_str()))
		throw std::exception("Could not save XML");
}

void Project_gfx::load_image_xml(SDL_Renderer* p_rnd, const SP_Config& p_config, const std::string& p_filename) {
	pugi::xml_document doc;
	if (!doc.load_file(p_config.get_xml_full_path(p_filename).c_str()))
		throw std::exception("Could not load xml");

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

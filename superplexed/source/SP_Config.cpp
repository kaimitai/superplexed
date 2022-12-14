#include "SP_Config.h"
#include "SP_Constants.h"
#include "SP_Constants.h"
#include "./common/pugixml/pugixml.hpp"
#include "./common/pugixml/pugiconfig.hpp"
#include "./common/klib/klib_util.h"
#include <algorithm>
#include <stdexcept>

SP_Config::SP_Config(const std::string& p_command_line_file_path) :
	m_undo_history_size{ c::UNDO_HISTORY_COUNT } {
	if (p_command_line_file_path.empty())
		load_configuration();
	else {
		generate_level_filedata_cache(p_command_line_file_path, true);
	}

	load_pixel_art_config(false);
	add_message("Level File: " + get_level_file_full_path());
}

std::string SP_Config::get_default_levels_filename(void) {
	return get_full_filename(c::FILENAME_LEVELS, c::SUFFIX_DAT);
}

void SP_Config::load_configuration(void) {
	std::string l_project_folder;
	std::string l_project_filename;

	try {
		pugi::xml_document doc;
		if (!doc.load_file(c::SPCONFIG_XML_FILENAME))
			throw std::runtime_error("Could not load configuration xml");
		else {
			auto n_meta = doc.child(c::XML_TAG_META);
			l_project_folder = n_meta.attribute(c::XML_ATTR_PROJECT_FOLDER).as_string();
			if (l_project_folder.empty())
				l_project_folder = c::DEFAULT_PROJECT_FOLDER;
			unsigned int l_undo_history_size = n_meta.attribute(c::XML_ATTR_UNDO_HISTORY_SIZE).as_int();

			m_undo_history_size = std::max<unsigned int>(l_undo_history_size, c::UNDO_HISTORY_COUNT);

			auto n_predefined = n_meta.child(c::XML_TAG_PREDEFINED);
			if (n_predefined) {
				m_predefined_levelset = Predefined_levelset(
					n_predefined.attribute(c::XML_ATTR_FILENAME).as_string(),
					n_predefined.attribute(c::XML_ATTR_LEVEL_COUNT).as_int()
				);

				for (auto n_set_file = n_predefined.child(c::XML_TAG_LEVEL_FILE);
					n_set_file;
					n_set_file = n_set_file.next_sibling(c::XML_TAG_LEVEL_FILE)) {
					m_predefined_levelset.add_file(
						n_set_file.attribute(c::XML_ATTR_FILEPATH).as_string(),
						n_set_file.attribute(c::XML_ATTR_LEVEL_NO).as_int() - 1);
				}
			}
		}
		add_message("Configuration loaded");
	}
	catch (...) {
		l_project_folder = c::DEFAULT_PROJECT_FOLDER;
		add_message(std::string(c::SPCONFIG_XML_FILENAME) + " not found; using default configuration");
	}

	if (has_predefined_levelset()) {
		l_project_filename = m_predefined_levelset.m_levelset_filename;
		if (!is_valid_level_filename(l_project_filename))
			l_project_filename = get_full_filename(l_project_filename, to_lowercase(c::SUFFIX_DAT));
	}
	else
		l_project_filename = SP_Config::get_default_levels_filename();

	this->generate_level_filedata_cache(get_path_combine(l_project_folder, l_project_filename), true);
}

void SP_Config::load_pixel_art_config(bool p_add_message) {
	try {
		pugi::xml_document doc;
		if (!doc.load_file(c::SPCONFIG_XML_FILENAME))
			throw std::runtime_error("Could not load configuration xml");
		else {
			auto n_meta = doc.child(c::XML_TAG_META);

			auto n_pixel_art_map = n_meta.child(c::XML_TAG_PIXEL_ART_MAP);
			if (n_pixel_art_map) {
				m_pixel_art_map.clear();

				for (auto n_pixel_art = n_pixel_art_map.child(c::XML_TAG_PIXEL_ART);
					n_pixel_art; n_pixel_art = n_pixel_art.next_sibling(c::XML_TAG_PIXEL_ART)) {
					auto l_cols = klib::util::string_split<byte>(n_pixel_art.attribute(c::XML_ATTR_RGB).as_string(), ',');
					byte l_tile_no = static_cast<byte>(n_pixel_art.attribute(c::XML_ATTR_TILE_NO).as_int());
					m_pixel_art_map.insert(std::make_pair(
						std::make_tuple(l_cols.at(0), l_cols.at(1), l_cols.at(2)),
						l_tile_no));
				}
			}

			if (p_add_message)
				add_message("Loaded pixel art configuration with " + std::to_string(m_pixel_art_map.size()) + " tiles");
		}
	}
	catch (...) {
		if (p_add_message)
			add_message(std::string(c::SPCONFIG_XML_FILENAME) + " not found; using default configuration");
	}
}

void SP_Config::add_message(const std::string& p_msg, bool p_prevent_repeat) {
	if (m_messages.empty() || (!p_prevent_repeat || m_messages.front() != p_msg)) {
		m_messages.push_front(p_msg);
		if (m_messages.size() > c::MESSAGES_MAX_SIZE)
			m_messages.pop_back();
	}
}

const std::deque<std::string>& SP_Config::get_messages(void) const {
	return m_messages;
}

const std::map<std::tuple<byte, byte, byte>, byte>& SP_Config::get_pixel_art_map(void) const {
	return m_pixel_art_map;
}

const SP_Config::Predefined_levelset& SP_Config::get_predefined_levelset(void) const {
	return m_predefined_levelset;
}

bool SP_Config::has_predefined_levelset(void) const {
	return !m_predefined_levelset.empty();
}

SP_Config::SP_file_type SP_Config::get_file_type_from_path(const std::string& p_file_path) {
	std::string l_ext = to_uppercase(p_file_path.substr(p_file_path.find_last_of('.') + 1));

	if (l_ext == to_uppercase(c::SUFFIX_XML))
		return SP_file_type::xml;
	else if (l_ext == to_uppercase(c::SUFFIX_SP))
		return SP_file_type::sp;
	else if (l_ext == to_uppercase(c::SUFFIX_DAT) ||
		(l_ext.size() == 3 && l_ext[0] == 'D'
			&& l_ext[1] >= '0' && l_ext[1] <= '9' &&
			l_ext[2] >= '0' && l_ext[2] <= '9'
			))
		return SP_file_type::dat;
	else
		return SP_file_type::unknown;
}

unsigned SP_Config::get_undo_history_size(void) const {
	return m_undo_history_size;
}

#include "SP_Config.h"
#include "SP_Constants.h"
#include "SP_Constants.h"
#include "./common/pugixml/pugixml.hpp"
#include "./common/pugixml/pugiconfig.hpp"
#include "./common/klib/klib_util.h"

SP_Config::SP_Config(void) {
	load_configuration();
}

void SP_Config::load_configuration(void) {
	try {
		pugi::xml_document doc;
		if (!doc.load_file(c::SPCONFIG_XML_FILENAME))
			throw std::exception("Could not load xml");
		else {
			auto n_meta = doc.child(c::XML_TAG_META);
			m_project_folder = n_meta.attribute(c::XML_ATTR_PROJECT_FOLDER).as_string();
			m_levels_filename = n_meta.attribute(c::XML_ATTR_LEVELS_DAT_FILE).as_string();
			m_level_list_filename = n_meta.attribute(c::XML_ATTR_LEVEL_LST_FILE).as_string();
		}
		add_message("Configuration loaded");
	}
	catch (...) {
		m_levels_filename = c::FILENAME_LEVELS;
		m_level_list_filename = c::FILENAME_LEVEL;
		m_project_folder = c::DEFAULT_PROJECT_FOLDER;
		add_message(std::string(c::SPCONFIG_XML_FILENAME) + " not found; using default configuration");
	}

	add_message("Level File: " + get_levels_dat_full_path());
}

std::string SP_Config::get_level_lst_full_path(void) const {
	return get_full_path(m_level_list_filename, c::SUFFIX_LST);
}

std::string SP_Config::get_lst_full_path(const std::string& p_filename) const {
	return get_full_path(p_filename, c::SUFFIX_LST);
}

std::string SP_Config::get_dat_full_path(const std::string& p_filename) const {
	return get_full_path(p_filename, c::SUFFIX_DAT);
}

std::string SP_Config::get_levels_dat_full_path(void) const {
	return get_full_path(m_levels_filename, c::SUFFIX_DAT);
}

std::string SP_Config::get_full_path(const std::string& p_filename, const std::string& p_extension) const {
	return m_project_folder + '/' + p_filename + '.' + p_extension;
}

void SP_Config::add_message(const std::string& p_msg) {
	m_messages.push_front(p_msg);
	if (m_messages.size() > c::MESSAGES_MAX_SIZE)
		m_messages.pop_back();
}

const std::deque<std::string>& SP_Config::get_messages(void) const {
	return m_messages;
}

std::string SP_Config::get_folder(const std::string& p_subfolder) const {
	return m_project_folder + '/' + p_subfolder;
}

std::string SP_Config::get_xml_folder(void) const {
	return get_folder(c::SUFFIX_XML);
}

std::string SP_Config::get_bmp_folder(void) const {
	return get_folder(c::SUFFIX_BMP);
}

std::string SP_Config::get_SP_folder(void) const {
	return get_folder(c::SUFFIX_SP);
}

std::string SP_Config::get_xml_full_path(const std::string& p_filename) const {
	return get_xml_folder() + '/' + p_filename + ".xml";
}

std::string SP_Config::get_bmp_full_path(const std::string& p_dat_filename) const {
	return get_bmp_folder() + '/' + p_dat_filename + ".bmp";
}

// files derived from individual levels

std::string SP_Config::get_xml_full_path(std::size_t p_level_no) const {
	return get_xml_full_path(m_levels_filename + '-' + klib::util::stringnum(p_level_no + 1));
}

std::string SP_Config::get_SP_full_path(std::size_t p_level_no) const {
	return get_SP_folder() + '/' + m_levels_filename + '-' + klib::util::stringnum(p_level_no + 1) + ".SP";
}

std::string SP_Config::get_bmp_full_path(std::size_t p_level_no) const {
	return get_bmp_full_path(m_levels_filename + '-' + klib::util::stringnum(p_level_no + 1));
}

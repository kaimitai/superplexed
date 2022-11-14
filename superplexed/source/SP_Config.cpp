#include "SP_Config.h"
#include "SP_Constants.h"
#include "SP_Constants.h"
#include "./common/pugixml/pugixml.hpp"
#include "./common/pugixml/pugiconfig.hpp"
#include "./common/klib/klib_util.h"
#include <algorithm>
#include <stdexcept>

SP_Config::SP_Config(void) {
	load_configuration();
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
						n_set_file.attribute(c::XML_ATTR_LEVEL_NO).as_int());
				}
			}
		}
		add_message("Configuration loaded");
	}
	catch (...) {
		l_project_folder = c::DEFAULT_PROJECT_FOLDER;
		add_message(std::string(c::SPCONFIG_XML_FILENAME) + " not found; using default configuration");
	}

	this->generate_level_filedata_cache(get_path_combine(l_project_folder, get_default_levels_filename()), true);

	add_message("Level File: " + get_level_dat_full_path());
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

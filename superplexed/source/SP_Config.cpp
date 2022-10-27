#include "SP_Config.h"
#include "SP_Constants.h"
#include "SP_Constants.h"
#include "./common/pugixml/pugixml.hpp"
#include "./common/pugixml/pugiconfig.hpp"

SP_Config::SP_Config(void) {
	try {
		pugi::xml_document doc;
		if (!doc.load_file(c::SPCONFIG_XML_FILENAME))
			throw std::exception("Could not load xml");
		else {
			auto n_meta = doc.child(c::XML_TAG_META);
			m_project_folder = n_meta.attribute(c::XML_ATTR_PROJECT_FOLDER).as_string();
			m_levels_filename = n_meta.attribute(c::XML_ATTR_LEVELS_DAT_FILE).as_string();
			m_levels_list_filename = n_meta.attribute(c::XML_ATTR_LEVEL_LST_FILE).as_string();
		}
	}
	catch (...) {
		m_levels_filename = c::DEFAULT_LVL_FILE;
		m_levels_list_filename = c::DEFAULT_LVL_LIST_FILE;
		m_project_folder = c::DEFAULT_PROJECT_FOLDER;
	}
}

std::string SP_Config::get_levels_dat_full_path(void) const {
	return get_dat_full_path(m_levels_filename);
}

std::string SP_Config::get_dat_full_path(const std::string& p_filename) const {
	return m_project_folder + "/" + p_filename + ".DAT";
}

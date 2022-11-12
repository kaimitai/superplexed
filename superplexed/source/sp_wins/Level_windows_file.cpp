#include "Level_window.h"
#include "./../common/pugixml/pugixml.hpp"
#include "./../common/pugixml/pugiconfig.hpp"
#include "./../common/klib/klib_util.h"
#include "./../sp_levels/SP_Level.h"
#include "./../SP_Config.h"
#include "./../SP_Constants.h"
#include <algorithm>
#include <stdexcept>
#include <tuple>

void Level_window::load_predefined_levelset(const SP_Config& p_config) {
	const auto& l_defs{ p_config.get_predefined_levelset() };

	for (const auto& l_file : l_defs.m_files) {
		std::string l_filepath = l_file.first;
		std::size_t l_lvl_no = l_file.second;

		if (SP_Config::get_file_type_from_path(l_filepath) == SP_Config::SP_file_type::xml)
			m_levels.push_back(level_xml_from_file(l_filepath));
		else if (SP_Config::get_file_type_from_path(l_filepath) == SP_Config::SP_file_type::sp)
			m_levels.push_back(level_sp_from_file(l_filepath));
		else if (SP_Config::get_file_type_from_path(l_filepath) == SP_Config::SP_file_type::dat)
			m_levels.push_back(level_dat_from_file(l_filepath, l_lvl_no));
	}

	while (m_levels.size() < l_defs.m_level_count)
		m_levels.push_back(SP_Level());
}

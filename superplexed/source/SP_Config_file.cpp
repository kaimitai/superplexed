#include "SP_Config.h"
#include <algorithm>
#include <filesystem>
#include <string>
#include <stdexcept>

bool SP_Config::is_valid_level_filename(const std::string& p_filename) {
	return get_file_type_from_path(p_filename) != SP_file_type::unknown;
}

void SP_Config::generate_level_filedata_cahce(const std::string& p_filepath,
	bool p_generate_project_folder) {

	SP_file_type m_current_file_type = get_file_type_from_path(p_filepath);
	if (m_current_file_type == SP_file_type::unknown)
		throw std::runtime_error("Not a valid level(s) file type");

	std::filesystem::path l_in_path{ p_filepath };

	std::string m_extension = l_in_path.filename().extension().string().substr(1);
	std::string m_filename = l_in_path.filename().string();

	if (p_generate_project_folder) {
		std::string m_project_fold = p_filepath.substr(0, p_filepath.size() - m_filename.size() - 1);
	}

	m_filename = m_filename.substr(0, m_filename.size() - m_extension.size() - 1);
}


bool SP_Config::has_savefiles(void) const {
	return is_levelset_file();
}

bool SP_Config::is_levelset_file(void) const {
	return m_cf_extension == SP_file_type::dat;
}

std::string SP_Config::to_uppercase(const std::string& p_input) {
	std::string result{ p_input };
	std::transform(begin(result), end(result), begin(result), ::toupper);
	return result;
}

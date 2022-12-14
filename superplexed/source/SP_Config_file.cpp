#include "SP_Config.h"
#include "SP_Constants.h"
#include "./common/klib/klib_util.h"
#include <algorithm>
#include <filesystem>
#include <set>
#include <string>
#include <stdexcept>

bool SP_Config::is_valid_level_filename(const std::string& p_filename) {
	// keep a list of uppercase filenames we do not want to populate the level(s) file dropdown in the main window
	static const std::set<std::string> invalid_files{
		get_full_filename(c::FILENAME_BACK, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_CHARS6, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_CHARS8, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_CONTROLS, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_FIXED, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_GFX, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_MENU, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_MOVING, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_PANEL, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_PALETTES, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_TITLE, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_TITLE1, c::SUFFIX_DAT),
		get_full_filename(c::FILENAME_TITLE2, c::SUFFIX_DAT),
		to_uppercase(c::SPCONFIG_XML_FILENAME)
	};

	if (invalid_files.find(to_uppercase(p_filename)) != end(invalid_files))
		return false;
	else
		return get_file_type_from_path(p_filename) != SP_file_type::unknown;
}

void SP_Config::generate_level_filedata_cache(const std::string& p_filepath,
	bool p_generate_project_folder) {

	m_cf_extension = get_file_type_from_path(p_filepath);
	if (m_cf_extension == SP_file_type::unknown)
		throw std::runtime_error("Not a valid level(s) file type");

	std::filesystem::path l_in_path{ p_filepath };

	m_cf_true_extension = l_in_path.filename().extension().string().substr(1);
	m_cf_filename = l_in_path.filename().string();

	if (p_generate_project_folder)
		m_cf_folder = p_filepath.substr(0, p_filepath.size() - m_cf_filename.size() - 1);

	m_cf_filename = m_cf_filename.substr(0, m_cf_filename.size() - m_cf_true_extension.size() - 1);

	if (is_levelset_file()) {
		if (to_uppercase(m_cf_true_extension) == to_uppercase(c::SUFFIX_DAT))
			m_cf_lst_extension = (m_cf_true_extension[0] == 'D' ? c::SUFFIX_LST : to_lowercase(c::SUFFIX_LST));
		else {
			m_cf_lst_extension = m_cf_true_extension;
			m_cf_lst_extension[0] = (m_cf_true_extension[0] == 'D' ? 'L' : 'l');
		}
	}

	m_cf_lst_extension_uc = to_uppercase(m_cf_lst_extension);
	m_cf_true_extension_uc = to_uppercase(m_cf_true_extension);
}

std::string SP_Config::get_project_folder(void) const {
	return m_cf_folder;
}

void SP_Config::set_project_file(const std::string& p_folder) {
	generate_level_filedata_cache(p_folder, false);
}

std::string SP_Config::get_dat_label(void) const {
	return m_cf_true_extension_uc;
}

std::string SP_Config::get_lst_label(void) const {
	return m_cf_lst_extension_uc;
}

SP_Config::SP_file_type SP_Config::get_extension(void) const {
	return m_cf_extension;
}

std::string SP_Config::get_loaded_file_name(void) const {
	return get_full_filename(m_cf_filename, m_cf_true_extension);
}

bool SP_Config::has_savefiles(void) const {
	return is_levelset_file() && to_uppercase(m_cf_filename) == c::FILENAME_LEVELS;
}

bool SP_Config::is_levelset_file(void) const {
	return m_cf_extension == SP_file_type::dat;
}

std::string SP_Config::to_uppercase(const std::string& p_input) {
	std::string result{ p_input };
	std::transform(begin(result), end(result), begin(result), ::toupper);
	return result;
}

std::string SP_Config::to_lowercase(const std::string& p_input) {
	std::string result{ p_input };
	std::transform(begin(result), end(result), begin(result), ::tolower);
	return result;
}

std::string SP_Config::get_full_filename(const std::string& p_filename, const std::string& p_extension) {
	return p_filename + '.' + p_extension;
}

std::string SP_Config::get_path_combine(const std::string& p_path, const std::string& p_subfolder_or_file) {
	return p_path + '/' + p_subfolder_or_file;
}

std::string SP_Config::get_filename_postfix(std::size_t p_no) const {
	std::string result;
	if (m_cf_true_extension_uc != c::SUFFIX_DAT)
		result = m_cf_true_extension;
	// add 1 to the input number, we count from 1 in the output/input files
	result += "-" + klib::util::stringnum(p_no + 1, 3);
	return result;
}

std::string SP_Config::get_full_filepath(const std::string& p_path, const std::string& p_filename, const std::string& p_extension) {
	return get_path_combine(p_path, get_full_filename(p_filename, p_extension));
}

// folders for level files
std::string SP_Config::get_level_bmp_folder(void) const {
	return get_path_combine(m_cf_folder, c::SUFFIX_BMP);
}

std::string SP_Config::get_level_xml_folder(void) const {
	if (m_cf_extension == SP_file_type::dat)
		return get_path_combine(m_cf_folder, c::SUFFIX_XML);
	else
		return m_cf_folder;
}

std::string SP_Config::get_level_sp_folder(void) const {
	if (m_cf_extension == SP_file_type::dat)
		return get_path_combine(m_cf_folder, c::SUFFIX_SP);
	else
		return m_cf_folder;
}

std::string SP_Config::get_level_dat_folder(void) const {
	return m_cf_folder;
}

// folders for gfx files
std::string SP_Config::get_gfx_bmp_folder(void) const {
	// gfx and level files end up in the same folder
	return get_level_bmp_folder();
}

std::string SP_Config::get_gfx_xml_folder(void) const {
	return get_path_combine(m_cf_folder, c::SUFFIX_XML);
}

std::string SP_Config::get_gfx_dat_folder(void) const {
	return m_cf_folder;
}

// full file paths to level files
std::string SP_Config::get_level_bmp_full_path(std::size_t p_level_no) const {
	std::string l_filename{ m_cf_filename };
	if (is_levelset_file())
		l_filename += get_filename_postfix(p_level_no);

	return get_full_filepath(get_level_bmp_folder(), l_filename, c::SUFFIX_BMP);
}

std::string SP_Config::get_level_xml_full_path(std::size_t p_level_no) const {
	std::string l_filename{ m_cf_filename };
	if (is_levelset_file())
		l_filename += get_filename_postfix(p_level_no);

	return get_full_filepath(get_level_xml_folder(), l_filename,
		m_cf_extension == SP_file_type::xml ? m_cf_true_extension : c::SUFFIX_XML);
}

std::string SP_Config::get_level_sp_full_path(std::size_t p_level_no) const {
	std::string l_filename{ m_cf_filename };
	if (is_levelset_file())
		l_filename += get_filename_postfix(p_level_no);

	return get_full_filepath(get_level_sp_folder(), l_filename,
		m_cf_extension == SP_file_type::sp ? m_cf_true_extension : c::SUFFIX_SP);
}

std::string SP_Config::get_level_file_full_path(void) const {
	return get_full_filepath(get_level_dat_folder(), m_cf_filename, m_cf_true_extension);
}

// full file paths to gfx files
std::string SP_Config::get_gfx_bmp_full_path(const std::string& p_filename) const {
	return get_full_filepath(get_gfx_bmp_folder(), p_filename, c::SUFFIX_BMP);
}

std::string SP_Config::get_gfx_xml_full_path(const std::string& p_filename) const {
	return get_full_filepath(get_gfx_xml_folder(), p_filename, c::SUFFIX_XML);
}

std::string SP_Config::get_gfx_dat_full_path(const std::string& p_filename) const {
	return get_full_filepath(get_gfx_dat_folder(), p_filename, c::SUFFIX_DAT);
}

// full path to levels.lst / levels.lxx
std::string SP_Config::get_level_list_full_path(void) const {
	return get_full_filepath(m_cf_folder, m_cf_filename.substr(0, m_cf_filename.size() - 1), m_cf_lst_extension);
}

// full paths to savefiles
std::string SP_Config::get_hallfame_full_path(void) const {
	return get_full_filepath(m_cf_folder, c::FILENAME_HALLFAME, m_cf_lst_extension);
}

std::string SP_Config::get_player_db_full_path(void) const {
	return get_full_filepath(m_cf_folder, c::FILENAME_PLAYER, m_cf_lst_extension);
}

std::string SP_Config::get_pixel_art_bmp_full_path(void) const {
	return get_full_filepath(m_cf_folder, c::FILENAME_PIXEL_ART, c::SUFFIX_BMP);
}

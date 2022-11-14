#include "SP_Config.h"
#include "SP_Constants.h"
#include "SP_Constants.h"
#include "./common/pugixml/pugixml.hpp"
#include "./common/pugixml/pugiconfig.hpp"
#include "./common/klib/klib_util.h"
#include <algorithm>
#include <stdexcept>

// #include "./common/klib/klib_file.h"

SP_Config::SP_Config(void) :
	m_levelset_no{ -1 }
{
	load_configuration();
	/*
	generate_level_filedata_cache("c:/supaplex/levels.d51", true);
	constexpr char TEST_FILE[]{ "test.txt" };

	klib::file::append_string_to_file("internal variables\n", TEST_FILE);

	klib::file::append_string_to_file(m_cf_filename + "\n", TEST_FILE);
	klib::file::append_string_to_file(m_cf_folder + "\n", TEST_FILE);
	klib::file::append_string_to_file("> " + m_cf_lst_extension + "\n", TEST_FILE);
	klib::file::append_string_to_file("> " + m_cf_lst_extension_uc + "\n", TEST_FILE);
	klib::file::append_string_to_file(m_cf_true_extension + "\n", TEST_FILE);
	klib::file::append_string_to_file("> " + m_cf_true_extension_uc + "\n", TEST_FILE);

	klib::file::append_string_to_file("levelfile folders\n", TEST_FILE);

	klib::file::append_string_to_file(get_level_bmp_folder() + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_level_xml_folder() + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_level_sp_folder() + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_level_dat_folder() + "\n", TEST_FILE);

	klib::file::append_string_to_file("gfx folders\n", TEST_FILE);

	klib::file::append_string_to_file(get_gfx_bmp_folder() + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_gfx_xml_folder() + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_gfx_dat_folder() + "\n", TEST_FILE);

	klib::file::append_string_to_file("Level files (lvl=10)\n", TEST_FILE);

	klib::file::append_string_to_file(get_level_bmp_full_path(10) + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_level_xml_full_path(10) + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_level_sp_full_path(10) + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_level_dat_full_path() + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_level_list_full_path() + "\n", TEST_FILE);

	klib::file::append_string_to_file("Gfx files (file=BACK)\n", TEST_FILE);

	klib::file::append_string_to_file(get_gfx_bmp_full_path("BACK") + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_gfx_xml_full_path("BACK") + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_gfx_dat_full_path("BACK") + "\n", TEST_FILE);

	klib::file::append_string_to_file("Savefiles:\n", TEST_FILE);

	klib::file::append_string_to_file(get_hallfame_full_path() + "\n", TEST_FILE);
	klib::file::append_string_to_file(get_player_db_full_path() + "\n", TEST_FILE);
	*/
}

std::string SP_Config::get_project_folder(void) const {
	return m_project_folder;
}

std::string SP_Config::get_default_levels_filename(void) {
	return get_full_filename(c::FILENAME_LEVELS, c::SUFFIX_DAT);
}

void SP_Config::load_configuration(void) {
	try {
		pugi::xml_document doc;
		if (!doc.load_file(c::SPCONFIG_XML_FILENAME))
			throw std::runtime_error("Could not load configuration xml");
		else {
			auto n_meta = doc.child(c::XML_TAG_META);
			m_project_folder = n_meta.attribute(c::XML_ATTR_PROJECT_FOLDER).as_string();

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
		m_project_folder = c::DEFAULT_PROJECT_FOLDER;
		add_message(std::string(c::SPCONFIG_XML_FILENAME) + " not found; using default configuration");
	}

	add_message("Level File: " + get_levels_dat_full_path());
}

std::string SP_Config::get_extension(char p_first_letter) const {
	if (m_levelset_no != -1)
		return p_first_letter + strnum();
	else
		return (p_first_letter == 'D' ? c::SUFFIX_DAT : c::SUFFIX_LST);
}

std::string SP_Config::get_levelset_lst_filename() const {
	return get_full_filename(c::FILENAME_LEVEL,
		get_extension('L'));
}

std::string SP_Config::get_levelset_dat_filename(void) const {
	return get_full_filename(c::FILENAME_LEVELS,
		get_extension('D'));
}

std::string SP_Config::get_prefix(void) const {
	if (m_levelset_no == -1)
		return c::FILENAME_LEVELS;
	else
		return c::FILENAME_LEVELS + strnum();
}

std::string SP_Config::strnum(void) const {
	return klib::util::stringnum(m_levelset_no, 2);
}

std::string SP_Config::get_full_path_ignore_extension(const std::string& p_full_filename) const {
	return m_project_folder + '/' + p_full_filename;
}

std::string SP_Config::get_level_lst_full_path(void) const {
	return get_full_path_ignore_extension(get_levelset_lst_filename());
}

std::string SP_Config::get_player_lst_full_path(void) const {
	return get_full_path_ignore_extension(
		get_full_filename(c::FILENAME_PLAYER,
			m_levelset_no == -1 ? c::SUFFIX_LST : get_extension('L'))
	);
}

std::string SP_Config::get_hallfame_lst_full_path(void) const {
	return get_full_path_ignore_extension(
		get_full_filename(c::FILENAME_HALLFAME,
			m_levelset_no == -1 ? c::SUFFIX_LST : get_extension('L'))
	);
}

std::string SP_Config::get_dat_full_path(const std::string& p_filename) const {
	return get_full_path(p_filename, c::SUFFIX_DAT);
}

std::string SP_Config::get_levels_dat_full_path(void) const {
	return get_full_path_ignore_extension(get_levelset_dat_filename());
}

std::string SP_Config::get_full_path(const std::string& p_filename, const std::string& p_extension) const {
	return m_project_folder + '/' + p_filename + '.' + p_extension;
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
	return get_xml_full_path(get_prefix() + '-' + klib::util::stringnum(p_level_no + 1));
}

std::string SP_Config::get_SP_full_path(std::size_t p_level_no) const {
	return get_SP_folder() + '/' + get_prefix() + '-' + klib::util::stringnum(p_level_no + 1) + ".SP";
}

std::string SP_Config::get_bmp_full_path(std::size_t p_level_no) const {
	return get_bmp_full_path(get_prefix() + '-' + klib::util::stringnum(p_level_no + 1));
}

void SP_Config::set_level_list_filename(const std::string& p_filename) {
	if (p_filename == get_default_levels_filename())
		m_levelset_no = -1;
	else
		m_levelset_no = atoi(p_filename.substr(p_filename.find_last_of('.') + 2).c_str());
}

SP_Config::SP_file_type SP_Config::get_file_type_from_path(const std::string& p_file_path) {
	std::string l_ext = p_file_path.substr(p_file_path.find_last_of('.') + 1);
	std::transform(begin(l_ext), end(l_ext), begin(l_ext), ::toupper);

	if (l_ext == c::SUFFIX_XML_UC)
		return SP_file_type::xml;
	else if (l_ext == c::SUFFIX_SP_UC)
		return SP_file_type::sp;
	else if (l_ext == c::SUFFIX_DAT_UC ||
		(l_ext.size() == 3 && l_ext[0] == 'D'
			&& l_ext[1] >= '0' && l_ext[1] <= '9' &&
			l_ext[2] >= '0' && l_ext[2] <= '9'
			))
		return SP_file_type::dat;
	else
		return SP_file_type::unknown;
}

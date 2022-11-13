#ifndef SP_CONFIG_H
#define SP_CONFIG_H

#include <deque>
#include <string>
#include <vector>
#include <utility>

class SP_Config {
public:

	enum class SP_file_type { bmp, dat, sp, xml, unknown };

	struct Predefined_levelset {
		std::string m_levelset_filename;
		int m_level_count;

		std::vector<std::pair<std::string, int>> m_files;
		Predefined_levelset(void) : m_level_count{ 0 }
		{}
		Predefined_levelset(const std::string& p_lvlset_filename, int p_lvl_count) :
			m_levelset_filename{ p_lvlset_filename }, m_level_count{ p_lvl_count }
		{}
		void add_file(const std::string& p_path, int dat_lvl_no) {
			m_files.push_back(std::make_pair(p_path, dat_lvl_no));
		}
		bool empty(void) const {
			return m_files.empty();
		}
	};

	SP_Config(void);
	void add_message(const std::string& p_msg, bool p_prevent_repeat = false);
	const std::deque<std::string>& get_messages(void) const;
	const Predefined_levelset& get_predefined_levelset(void) const;
	bool has_predefined_levelset(void) const;

	std::string get_extension(char p_first_letter) const;

	std::string get_xml_folder(void) const;
	std::string get_bmp_folder(void) const;
	std::string get_SP_folder(void) const;

	std::string get_levels_dat_full_path(void) const;
	std::string get_level_lst_full_path(void) const;
	std::string get_player_lst_full_path(void) const;
	std::string get_hallfame_lst_full_path(void) const;

	std::string get_dat_full_path(const std::string& p_filename) const;

	void load_configuration(void);

	// project folder
	std::string get_project_folder(void) const;
	// dat conversion bmp
	std::string get_bmp_full_path(const std::string& p_dat_filename) const;
	// image xml
	std::string get_xml_full_path(const std::string& p_filename) const;

	// level image bmp
	std::string get_bmp_full_path(std::size_t p_level_no) const;
	// level xml
	std::string get_xml_full_path(std::size_t p_level_no) const;
	// level sp
	std::string get_SP_full_path(std::size_t p_level_no) const;

	// default level filename LEVELS.DAT
	static std::string get_default_levels_filename(void);
	void set_level_list_filename(const std::string& p_filename);

	static SP_file_type get_file_type_from_path(const std::string& p_file_path);

	// rewrite
	static bool is_valid_level_filename(const std::string& p_filename);

	void generate_level_filedata_cahce(const std::string& p_filepath,
		bool p_generate_project_folder = false);

private:
	SP_Config::Predefined_levelset m_predefined_levelset;
	std::deque<std::string> m_messages;
	std::string m_project_folder;
	// levelset number: -1 is LEVELS.DAT and LEVEL.LST, 00 to 99 is LEVELS.Dxx, LEVEL.Lxx etc
	int m_levelset_no;

	// rewrite - BEGIN

	// rewrite variables
	SP_file_type m_cf_extension;      // current file extension, internal
	std::string m_cf_true_extension,  // cf true extension, use when writing back to same file
		m_cf_filename,                // cf true filename, use when writing back to any file
		m_cf_folder,                  // folder of current file
		m_cf_hallfame_lst_full_name,  // HALLFAME.Lxx
		m_cf_player_lst_full_name;    // PLAYER.Lxx

	bool has_savefiles(void) const;
	bool is_levelset_file(void) const;

	// utility functions
	static std::string to_uppercase(const std::string& p_input);

	// rewrite - END

	std::string get_folder(const std::string& p_subfolder) const;
	std::string get_full_path(const std::string& p_filename, const std::string& p_extension) const;

	static std::string get_full_filename(const std::string& p_filename, const std::string& p_suffix);

	std::string get_full_path_ignore_extension(const std::string& p_full_filename) const;
	std::string get_levelset_lst_filename(void) const;
	std::string get_levelset_dat_filename(void) const;
	std::string get_prefix(void) const;
	std::string strnum(void) const;

};

#endif

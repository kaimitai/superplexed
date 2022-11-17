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

	SP_Config(const std::string& p_command_line_file_path);
	void add_message(const std::string& p_msg, bool p_prevent_repeat = false);
	const std::deque<std::string>& get_messages(void) const;
	const Predefined_levelset& get_predefined_levelset(void) const;
	bool has_predefined_levelset(void) const;

	static std::string get_default_levels_filename(void);

	bool has_savefiles(void) const;
	bool is_levelset_file(void) const;
	unsigned int get_undo_history_size(void) const;

	// utility
	static bool is_valid_level_filename(const std::string& p_filename);
	static SP_file_type get_file_type_from_path(const std::string& p_file_path);
	static std::string get_path_combine(const std::string& p_path, const std::string& p_subfolder_or_file);

	// settings regularly needed by the GUI
	std::string get_project_folder(void) const;
	std::string get_dat_label(void) const;
	std::string get_lst_label(void) const;
	SP_file_type get_extension(void) const;
	std::string get_loaded_file_name(void) const;

	// get folders for level files (these folders may need to be created by the callers)
	std::string get_level_bmp_folder(void) const;
	std::string get_level_xml_folder(void) const;
	std::string get_level_sp_folder(void) const;
	std::string get_level_dat_folder(void) const;

	// get folders for graphics files (these folders may need to be created by the callers)
	std::string get_gfx_bmp_folder(void) const;
	std::string get_gfx_xml_folder(void) const;
	std::string get_gfx_dat_folder(void) const;

	// get full file paths to level files (used for read/write by the callers)
	std::string get_level_bmp_full_path(std::size_t p_level_no) const;
	std::string get_level_xml_full_path(std::size_t p_level_no) const;
	std::string get_level_sp_full_path(std::size_t p_level_no) const;
	std::string get_level_dat_full_path(void) const;
	std::string get_level_list_full_path(void) const;

	// get full file paths to gfx files (used for read/write by the callers)
	std::string get_gfx_bmp_full_path(const std::string& p_filename) const;
	std::string get_gfx_xml_full_path(const std::string& p_filename) const;
	std::string get_gfx_dat_full_path(const std::string& p_filename) const;

	// get full paths to savefiles (used for read/write by the caller)
	std::string get_hallfame_full_path(void) const;
	std::string get_player_db_full_path(void) const;

	// setters
	void set_project_file(const std::string& p_filename);

private:
	// load configuration from xml and initialize the project
	void load_configuration(void);

	SP_Config::Predefined_levelset m_predefined_levelset;
	std::deque<std::string> m_messages;
	void generate_level_filedata_cache(const std::string& p_filepath,
		bool p_generate_project_folder = false);
	unsigned int m_undo_history_size;

	SP_file_type m_cf_extension;      // current file extension, internal
	std::string m_cf_true_extension,  // cf true extension, use when writing back to same file
		m_cf_true_extension_uc,	      // cf true extension uppercase, use for button labels
		m_cf_filename,                // cf true filename, use when writing back to any file
		m_cf_folder,                  // folder of current file
		m_cf_lst_extension,           // lst extension for savefiles
		m_cf_lst_extension_uc;		  // lst extension uppercase, for button labels

	// utility functions
	static std::string to_uppercase(const std::string& p_input);
	static std::string to_lowercase(const std::string& p_input);
	static std::string get_full_filename(const std::string& p_filename, const std::string& p_extension);
	static std::string get_full_filepath(const std::string& p_path, const std::string& p_filename, const std::string& p_extension);
	std::string get_filename_postfix(std::size_t p_no) const;
};

#endif

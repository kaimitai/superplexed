#ifndef SP_CONFIG_H
#define SP_CONFIG_H

#include <deque>
#include <string>

class SP_Config {

	std::deque<std::string> m_messages;
	std::string m_project_folder, m_levels_dat_filename;

	std::string get_folder(const std::string& p_subfolder) const;
	std::string get_full_path(const std::string& p_filename, const std::string& p_extension) const;

	static std::string get_full_filename(const std::string& p_filename, const std::string& p_suffix);
	std::string get_full_path_ignore_extension(const std::string& p_full_filename) const;
	std::string get_levelset_lst_filename(void) const;

public:
	SP_Config(void);
	void add_message(const std::string& p_msg);
	const std::deque<std::string>& get_messages(void) const;

	std::string get_xml_folder(void) const;
	std::string get_bmp_folder(void) const;
	std::string get_SP_folder(void) const;

	std::string get_levels_dat_full_path(void) const;
	std::string get_level_lst_full_path(void) const;
	std::string get_lst_full_path(const std::string& p_filename) const;
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
};

#endif

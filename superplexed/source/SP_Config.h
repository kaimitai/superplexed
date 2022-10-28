#ifndef SP_CONFIG_H
#define SP_CONFIG_H

#include <deque>
#include <string>

class SP_Config {

	std::deque<std::string> m_messages;
	std::string m_project_folder, m_levels_filename, m_levels_list_filename;

public:
	SP_Config(void);
	void add_message(const std::string& p_msg);
	const std::deque<std::string>& get_messages(void) const;

	std::string get_levels_dat_full_path(void) const;
	std::string get_dat_full_path(const std::string& p_filename) const;
};

#endif

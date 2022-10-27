#ifndef SP_CONFIG_H
#define SP_CONFIG_H

#include <string>

class SP_Config {

	std::string m_project_folder, m_levels_filename, m_levels_list_filename;

public:
	SP_Config(void);

	std::string get_levels_dat_full_path(void) const;
};

#endif

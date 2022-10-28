#ifndef SP_CONSTANTS_H
#define SP_CONSTANTS_H

namespace c {
	// application constants
	constexpr char APP_TITLE[]{ "Superplexed v0.1" };
	constexpr char APP_VERSION[]{ "0.1" };

	// filenames
	constexpr char SPCONFIG_XML_FILENAME[]{ "spconfig.xml" };
	constexpr char DEFAULT_LVL_FILE[]{ "LEVELS" };
	constexpr char DEFAULT_LVL_LIST_FILE[]{ "LEVEL" };
	constexpr char DEFAULT_PROJECT_FOLDER[]{ "." };

	// output messages
	constexpr unsigned int MESSAGES_MAX_SIZE{ 25 };

	// xml tags, attributes and values
	// configuration XML
	constexpr char XML_ATTR_PROJECT_FOLDER[]{ "project_folder" };
	constexpr char XML_ATTR_LEVELS_DAT_FILE[]{ "level_file" };
	constexpr char XML_ATTR_LEVEL_LST_FILE[]{ "level_list_file" };

	// level file XMLs
	constexpr char XML_COMMENTS[]{ " Supaplex Level created by Superplexed (https://github.com/kaimitai/superplexed) " };
	constexpr char XML_TAG_META[]{ "superplexed" };
	constexpr char XML_TAG_LEVEL[]{ "level" };
	constexpr char XML_TAG_TILE_ROWS[]{ "tile_rows" };
	constexpr char XML_TAG_TILE_ROW[]{ "tile_row" };
	constexpr char XML_TAG_GPS[]{ "special_ports" };
	constexpr char XML_TAG_GP[]{ "special_port" };
	constexpr char XML_TAG_SOLUTION[]{ "speedfix_solution" };

	constexpr char XML_ATTR_APP_VERSION[]{ "app_version" };
	constexpr char XML_ATTR_TITLE[]{ "title" };
	constexpr char XML_ATTR_PLAYER_X[]{ "player_x" };
	constexpr char XML_ATTR_PLAYER_Y[]{ "player_y" };
	constexpr char XML_ATTR_INFOTRONS[]{ "infotrons_needed" };
	constexpr char XML_ATTR_GRAV[]{ "gravity" };
	constexpr char XML_ATTR_FZ[]{ "freeze_zonks" };
	constexpr char XML_ATTR_NO[]{ "no" };
	constexpr char XML_ATTR_VALUE[]{ "value" };
	constexpr char XML_ATTR_SF_V[]{ "speedfix_version" };
	constexpr char XML_ATTR_SF_DB[]{ "speedfix_demo_bytes" };
	constexpr char XML_ATTR_UNKNOWN[]{ "unknown_bytes" };

	constexpr char XML_ATTR_POS_X[]{ "pos_x" };
	constexpr char XML_ATTR_POS_Y[]{ "pos_y" };
	constexpr char XML_ATTR_FE[]{ "freeze_enemies" };

	constexpr char XML_VAL_TRUE[]{ "true" };
	constexpr char XML_VAL_FALSE[]{ "false" };
}

#endif

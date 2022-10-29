#ifndef SP_CONSTANTS_H
#define SP_CONSTANTS_H

namespace c {
	// application constants
	constexpr char APP_TITLE[]{ "Superplexed v0.1" };
	constexpr char APP_VERSION[]{ "0.1" };
	constexpr int APP_WIN_W_INITIAL{ 1024 };
	constexpr int APP_WIN_H_INITIAL{ 768 };

	// filenames
	constexpr char SPCONFIG_XML_FILENAME[]{ "spconfig.xml" };
	constexpr char DEFAULT_LVL_FILE[]{ "LEVELS" };
	constexpr char DEFAULT_LVL_LIST_FILE[]{ "LEVEL" };
	constexpr char DEFAULT_PROJECT_FOLDER[]{ "." };

	// folders and file extensions
	constexpr char SUFFIX_DAT[]{ "DAT" };
	constexpr char SUFFIX_BMP[]{"bmp"};
	constexpr char SUFFIX_LST[]{ "LST" };
	constexpr char SUFFIX_SP[]{ "SP" };
	constexpr char SUFFIX_XML[]{ "xml" };

	// output messages
	constexpr unsigned int MESSAGES_MAX_SIZE{ 25 };

	// xml tags, attributes and values
	// configuration XML
	constexpr char XML_ATTR_PROJECT_FOLDER[]{ "project_folder" };
	constexpr char XML_ATTR_LEVELS_DAT_FILE[]{ "level_file" };
	constexpr char XML_ATTR_LEVEL_LST_FILE[]{ "level_list_file" };

	// level file XMLs
	constexpr char XML_COMMENTS[]{ " Supaplex Level created by Superplexed (https://github.com/kaimitai/superplexed) " };
	constexpr char XML_COMMENTS_IMAGE[]{ " Supaplex Image created by Superplexed (https://github.com/kaimitai/superplexed) " };
	constexpr char XML_COMMENTS_PALETTE[]{ " Supaplex Palettes created by Superplexed (https://github.com/kaimitai/superplexed) " };
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

	// image file xmls
	constexpr char XML_TAG_IMAGE[]{"image"};
	constexpr char XML_ATTR_WIDTH[]{ "width" };
	constexpr char XML_ATTR_HEIGHT[]{ "height" };
	constexpr char XML_ATTR_IMGTYPE[]{ "image_type" };
	constexpr char XML_VAL_BINARY[]{ "binary" };
	constexpr char XML_VAL_PLANAR[]{ "planar" };
	constexpr char XML_TAG_PIXEL_ROWS[]{ "pixel_rows" };
	constexpr char XML_TAG_PIXEL_ROW[]{ "pixel_row" };

	// palette file xmls
	constexpr char XML_TAG_PALETTES[]{ "palettes" };
	constexpr char XML_TAG_PALETTE[]{ "palette" };
	constexpr char XML_TAG_COLOR[]{ "color" };
	constexpr char XML_ATTR_R[]{ "r" };
	constexpr char XML_ATTR_G[]{ "g" };
	constexpr char XML_ATTR_B[]{ "b" };

	// gfx constants
	constexpr unsigned int TILE_W{ 16 };
	constexpr unsigned int PALETTE_COUNT{ 4 };

	// level file constants
	constexpr unsigned int LEVEL_W{ 60 }, LEVEL_H{ 24 },
		OFFSET_TILES{ 0 }, OFFSET_GRAVITY{ 1444 }, OFFSET_UNUSED_BYTES{ 1440 },
		OFFSET_SF_VERSION{ 1445 },
		OFFSET_TITLE{ 1446 }, OFFSET_FREEZE_ZONKS{ 1469 }, OFFSET_IT_COUNT{ 1470 },
		OFFSET_GP_COUNT{ 1471 }, OFFSET_GP{ 1472 }, OFFSET_SF_DEMO_BYTES{ 1532 },
		LENGTH_TITLE{ 23 }, LENGTH_GP{ 6 }, LENGTH_SF_DEMO_BYTES{ 4 },
		LENGTH_UNUSED_BYTES{ 4 },
		LVL_DATA_BYTE_SIZE{ 1536 };
}

#endif

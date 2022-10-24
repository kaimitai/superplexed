#include "Level_window.h"
#include "./../common/pugixml/pugixml.hpp"
#include "./../common/pugixml/pugiconfig.hpp"
#include <filesystem>

constexpr char XML_COMMENTS[]{ " Supaplex Level created by Superplexed (https://github.com/kaimitai/superplexed) " };
constexpr char XML_TAG_META[]{ "superplexed" };
constexpr char XML_TAG_LEVEL[]{ "level" };

constexpr char XML_ATTR_PLAYER_X[]{ "player_x" };
constexpr char XML_ATTR_PLAYER_Y[]{ "player_y" };


void Level_window::save_xml(std::size_t p_level_no) const {
	const auto& l_lvl = m_levels.at(p_level_no);

	pugi::xml_document doc;
	auto n_comments = doc.append_child(pugi::node_comment);
	n_comments.set_value(XML_COMMENTS);

	auto n_metadata = doc.append_child(XML_TAG_META);

	auto n_level = n_metadata.append_child(XML_TAG_LEVEL);
	n_level.append_attribute(XML_ATTR_PLAYER_X);
	n_level.append_attribute(XML_ATTR_PLAYER_Y);

	n_level.attribute(XML_ATTR_PLAYER_X).set_value(l_lvl.get_start_pos().first);
	n_level.attribute(XML_ATTR_PLAYER_Y).set_value(l_lvl.get_start_pos().second);

	if (!doc.save_file("./gamedata/LEVEL.xml"))
		throw std::exception("Could not save XML");
}

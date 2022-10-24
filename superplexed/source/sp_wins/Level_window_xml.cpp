#include "Level_window.h"
#include "./../common/pugixml/pugixml.hpp"
#include "./../common/pugixml/pugiconfig.hpp"
#include "./../common/klib/klib_util.h"
#include <filesystem>

constexpr char XML_COMMENTS[]{ " Supaplex Level created by Superplexed (https://github.com/kaimitai/superplexed) " };
constexpr char XML_TAG_META[]{ "superplexed" };
constexpr char XML_TAG_LEVEL[]{ "level" };
constexpr char XML_TAG_TILE_ROWS[]{ "tile_rows" };
constexpr char XML_TAG_TILE_ROW[]{ "tile_row" };
constexpr char XML_TAG_GPS[]{ "gravity_ports" };
constexpr char XML_TAG_GP[]{ "gravity_port" };

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


void Level_window::save_xml(std::size_t p_level_no) const {
	const auto& l_lvl = m_levels.at(p_level_no);

	pugi::xml_document doc;
	auto n_comments = doc.append_child(pugi::node_comment);
	n_comments.set_value(XML_COMMENTS);

	auto n_metadata = doc.append_child(XML_TAG_META);

	auto n_level = n_metadata.append_child(XML_TAG_LEVEL);
	n_level.append_attribute(XML_ATTR_TITLE);
	n_level.append_attribute(XML_ATTR_PLAYER_X);
	n_level.append_attribute(XML_ATTR_PLAYER_Y);
	n_level.append_attribute(XML_ATTR_INFOTRONS);
	n_level.append_attribute(XML_ATTR_GRAV);
	n_level.append_attribute(XML_ATTR_FZ);
	n_level.append_attribute(XML_ATTR_SF_V);
	n_level.append_attribute(XML_ATTR_SF_DB);
	n_level.append_attribute(XML_ATTR_UNKNOWN);

	n_level.attribute(XML_ATTR_TITLE).set_value(l_lvl.get_title().c_str());
	n_level.attribute(XML_ATTR_PLAYER_X).set_value(l_lvl.get_start_pos().first);
	n_level.attribute(XML_ATTR_PLAYER_Y).set_value(l_lvl.get_start_pos().second);
	n_level.attribute(XML_ATTR_INFOTRONS).set_value(l_lvl.get_solve_it_count());
	n_level.attribute(XML_ATTR_GRAV).set_value(l_lvl.get_gravity() ? XML_VAL_TRUE : XML_VAL_FALSE);
	n_level.attribute(XML_ATTR_FZ).set_value(l_lvl.get_freeze_zonks() ? XML_VAL_TRUE : XML_VAL_FALSE);

	n_level.attribute(XML_ATTR_SF_V).set_value(static_cast<int>(l_lvl.get_speedfix_version()));
	n_level.attribute(XML_ATTR_SF_DB).set_value(
		klib::util::string_join(l_lvl.get_sf_demo_bytes(), ',').c_str()
	);
	n_level.attribute(XML_ATTR_UNKNOWN).set_value(
		klib::util::string_join(l_lvl.get_unused_bytes(), ',').c_str()
	);

	auto n_trows = n_level.append_child(XML_TAG_TILE_ROWS);
	const auto& l_tile_data = l_lvl.get_tile_data();

	for (std::size_t i{ 0 }; i < l_tile_data.size(); ++i) {
		auto n_trow = n_trows.append_child(XML_TAG_TILE_ROW);
		n_trow.append_attribute(XML_ATTR_NO);
		n_trow.append_attribute(XML_ATTR_VALUE);

		n_trow.attribute(XML_ATTR_NO).set_value(i + 1);
		n_trow.attribute(XML_ATTR_VALUE).set_value(klib::util::string_join<byte>(
			l_tile_data[i], ',').c_str());
	}

	auto n_gps = n_level.append_child(XML_TAG_GPS);

	for (int i{ 0 }; i < l_lvl.get_gravity_port_count(); ++i) {
		auto n_gp = n_gps.append_child(XML_TAG_GP);

		n_gp.append_attribute(XML_ATTR_NO);
		n_gp.append_attribute(XML_ATTR_POS_X);
		n_gp.append_attribute(XML_ATTR_POS_Y);
		n_gp.append_attribute(XML_ATTR_GRAV);
		n_gp.append_attribute(XML_ATTR_FZ);
		n_gp.append_attribute(XML_ATTR_FE);
		n_gp.append_attribute(XML_ATTR_UNKNOWN);

		n_gp.attribute(XML_ATTR_NO).set_value(i + 1);
		n_gp.attribute(XML_ATTR_POS_X).set_value(l_lvl.get_gp_x(i));
		n_gp.attribute(XML_ATTR_POS_Y).set_value(l_lvl.get_gp_y(i));
		n_gp.attribute(XML_ATTR_GRAV).set_value(l_lvl.get_gp_gravity(i) ?
			XML_VAL_TRUE : XML_VAL_FALSE);
		n_gp.attribute(XML_ATTR_FZ).set_value(l_lvl.get_gp_freeze_zonks(i) ?
			XML_VAL_TRUE : XML_VAL_FALSE);
		n_gp.attribute(XML_ATTR_FE).set_value(l_lvl.get_gp_freeze_enemies(i) ?
			XML_VAL_TRUE : XML_VAL_FALSE);
		n_gp.attribute(XML_ATTR_UNKNOWN).set_value(static_cast<int>(l_lvl.get_gp_unknown(i)));
	}

	std::filesystem::create_directory("xml");
	if (!doc.save_file(get_level_xml_filename(p_level_no).c_str()))
		throw std::exception("Could not save XML");
}

SP_Level Level_window::load_xml(std::size_t p_level_no) const {
	pugi::xml_document doc;
	if (!doc.load_file(get_level_xml_filename(p_level_no).c_str()))
		throw std::exception("Could not load xml");

	pugi::xml_node n_meta = doc.child(XML_TAG_META);
	auto n_level = n_meta.child(XML_TAG_LEVEL);

	std::string l_title{ n_level.attribute(XML_ATTR_TITLE).as_string() };
	int l_px{ n_level.attribute(XML_ATTR_PLAYER_X).as_int() };
	int l_py{ n_level.attribute(XML_ATTR_PLAYER_Y).as_int() };
	int l_it{ n_level.attribute(XML_ATTR_INFOTRONS).as_int() };
	bool l_grav{ n_level.attribute(XML_ATTR_GRAV).as_bool() };
	bool l_fz{ n_level.attribute(XML_ATTR_FZ).as_bool() };
	int l_sf_v{ n_level.attribute(XML_ATTR_SF_V).as_int() };
	std::vector<byte> l_sf_db{ klib::util::string_split<byte>(
		n_level.attribute(XML_ATTR_SF_DB).as_string(), ',') };
	std::vector<byte> l_unknown{ klib::util::string_split<byte>(
	n_level.attribute(XML_ATTR_UNKNOWN).as_string(), ',') };

	auto n_trows = n_level.child(XML_TAG_TILE_ROWS);

	std::vector<std::vector<byte>> l_tile_data;
	for (auto n_trow = n_trows.child(XML_TAG_TILE_ROW);
		n_trow; n_trow = n_trow.next_sibling(XML_TAG_TILE_ROW))
		l_tile_data.push_back(klib::util::string_split<byte>(n_trow.attribute(XML_ATTR_VALUE).as_string(), ','));

	auto l_lvl = SP_Level(l_title, l_tile_data, l_px, l_py, l_it, l_grav, l_fz,
		static_cast<byte>(l_sf_v), l_sf_db, l_unknown);

	auto n_gps = n_level.child(XML_TAG_GPS);
	for (auto n_gp = n_gps.child(XML_TAG_GP); n_gp; n_gp = n_gp.next_sibling(XML_TAG_GP)) {
		int l_gp_x = n_gp.attribute(XML_ATTR_POS_X).as_int();
		int l_gp_y = n_gp.attribute(XML_ATTR_POS_Y).as_int();
		bool l_gp_grav = n_gp.attribute(XML_ATTR_GRAV).as_bool();
		bool l_gp_fz = n_gp.attribute(XML_ATTR_FZ).as_bool();
		bool l_gp_fe = n_gp.attribute(XML_ATTR_FE).as_bool();
		byte l_gp_unk = static_cast<byte>(n_gp.attribute(XML_ATTR_UNKNOWN).as_int());

		l_lvl.add_gravity_port(l_gp_x, l_gp_y, l_gp_grav, l_gp_fz, l_gp_fe, l_gp_unk);
	}

	return l_lvl;
}

std::string Level_window::get_level_xml_filename(std::size_t p_level_no) const {
	std::string result{ "./xml/LEVELS-" };
	std::string suffix{ std::to_string(p_level_no + 1) };
	while (suffix.size() < 3)
		suffix.insert(begin(suffix), '0');
	return (result + suffix + ".xml");
}

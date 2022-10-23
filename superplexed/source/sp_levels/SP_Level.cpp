#include "SP_Level.h"

using byte = unsigned char;

constexpr unsigned int LEVEL_W{ 60 }, LEVEL_H{ 24 },
OFFSET_TILES{ 0 }, OFFSET_GRAVITY{ 1444 }, OFFSET_UNUSED_BYTES{ 1440 },
OFFSET_SF_VERSION{ 1445 },
OFFSET_TITLE{ 1446 }, OFFSET_FREEZE_ZONKS{ 1469 }, OFFSET_IT_COUNT{ 1470 },
OFFSET_GP_COUNT{ 1471 }, OFFSET_GP{ 1472 }, OFFSET_SF_DEMO_BYTES{ 1532 },
LENGTH_TITLE{ 23 }, LENGTH_GP{ 6 }, LENGTH_SF_DEMO_BYTES{ 4 },
LENGTH_UNUSED_BYTES{ 4 };

SP_Level::SP_Level(const std::vector<byte>& p_bytes) :
	m_player_x{ 0 }, m_player_y{ 0 } {

	// initialize level map and player start
	for (std::size_t y{ 0 }; y < LEVEL_H; ++y) {
		std::vector<byte> l_row;
		for (std::size_t x{ 0 }; x < LEVEL_W; ++x) {
			byte l_value = p_bytes.at(OFFSET_TILES + LEVEL_W * y + x);
			if (l_value == 3) {
				m_player_x = static_cast<byte>(x);
				m_player_y = static_cast<byte>(y);
				l_row.push_back(0);
			}
			else
				l_row.push_back(l_value);
		}
		m_tiles.push_back(l_row);
	}

	// initialize other metadata
	m_gravity = (p_bytes.at(OFFSET_GRAVITY) == 1);
	m_sf_version = p_bytes.at(OFFSET_SF_VERSION);
	m_title = std::string(begin(p_bytes) + OFFSET_TITLE,
		begin(p_bytes) + OFFSET_TITLE + LENGTH_TITLE);
	m_freeze_zonks = (p_bytes.at(OFFSET_FREEZE_ZONKS) == 2);
	m_solve_it_count = p_bytes.at(OFFSET_IT_COUNT);

	m_unused_bytes = std::vector<byte>(begin(p_bytes) + OFFSET_UNUSED_BYTES,
		begin(p_bytes) + OFFSET_UNUSED_BYTES + LENGTH_UNUSED_BYTES);
	m_sf_demo_bytes = std::vector<byte>(begin(p_bytes) + OFFSET_SF_DEMO_BYTES,
		begin(p_bytes) + OFFSET_SF_DEMO_BYTES + LENGTH_SF_DEMO_BYTES);

	// initialize gravity ports
	byte l_gp_count = p_bytes.at(OFFSET_GP_COUNT);
	for (std::size_t i{ 0 }; i < l_gp_count; ++i) {
		std::size_t l_offset = OFFSET_GP + LENGTH_GP * i;
		m_gravity_ports.push_back(std::vector<byte>(begin(p_bytes) + l_offset,
			begin(p_bytes) + l_offset + LENGTH_GP));
	}
}

std::vector<byte> SP_Level::get_bytes(void) const {
	std::vector<byte> result;
	// add tile data
	for (int i{ 0 }; i < LEVEL_H; ++i)
		result.insert(end(result), begin(m_tiles.at(i)), end(m_tiles.at(i)));
	// update tile data with player start
	result.at(static_cast<std::size_t>(m_player_y * LEVEL_W + m_player_x)) = 3;

	result.insert(end(result), begin(m_unused_bytes), end(m_unused_bytes));
	result.push_back(m_gravity ? 1 : 0);
	result.push_back(m_sf_version);
	result.insert(end(result), begin(m_title), end(m_title));
	result.push_back(m_freeze_zonks ? 2 : 0);
	result.push_back(m_solve_it_count);

	// add gravity ports
	result.push_back(static_cast<byte>(m_gravity_ports.size()));
	for (const auto& gp : m_gravity_ports) {
		auto l_gp_bytes = gp.get_bytes();
		result.insert(end(result), begin(l_gp_bytes), end(l_gp_bytes));
	}

	for (std::size_t i{ 10 }; i > m_gravity_ports.size(); --i)
		for (int j{ 0 }; j < LENGTH_GP; ++j)
			result.push_back(0);

	result.insert(end(result), begin(m_sf_demo_bytes), end(m_sf_demo_bytes));

	return result;
}

// gravity port implementation
SP_Level::Gravity_port::Gravity_port(const std::vector<byte>& p_bytes) {
	m_x = p_bytes.at(0);
	m_y = p_bytes.at(1);
	m_gravity = (p_bytes.at(2) == 1);
	m_freeze_zonks = (p_bytes.at(3) == 2);
	m_freeze_enemies = (p_bytes.at(4) == 1);
	m_unknown = p_bytes.at(5);
}

std::vector<byte> SP_Level::Gravity_port::get_bytes(void) const {
	std::vector<byte> result;

	result.push_back(m_x);
	result.push_back(m_y);
	result.push_back(m_gravity ? 1 : 0);
	result.push_back(m_freeze_zonks ? 2 : 0);
	result.push_back(m_freeze_enemies ? 1 : 0);
	result.push_back(m_unknown);

	return result;
}

byte SP_Level::get_tile_no(int p_x, int p_y) const {
	return m_tiles.at(p_y).at(p_x);
}

std::pair<byte, byte> SP_Level::get_start_pos(void) const {
	return std::make_pair(m_player_x, m_player_y);
}

const std::string& SP_Level::get_title(void) const {
	return m_title;
}

int SP_Level::get_solve_it_count(void) const {
	return static_cast<int>(m_solve_it_count);
}

bool SP_Level::SP_Level::get_gravity(void) const {
	return m_gravity;
}

bool SP_Level::SP_Level::get_freeze_zonks(void) const {
	return m_freeze_zonks;
}

// setters
void SP_Level::set_title(const std::string& p_title) {
	m_title = p_title;

	while (m_title.size() < LENGTH_TITLE)
		m_title.push_back(' ');
	while (m_title.size() > LENGTH_TITLE)
		m_title.pop_back();
}

void SP_Level::set_solve_it_count(int p_count) {
	m_solve_it_count = static_cast<byte>(p_count);
}

void SP_Level::set_gravity(bool p_param) {
	m_gravity = p_param;
}

void SP_Level::set_freeze_zonks(bool p_param) {
	m_freeze_zonks = p_param;
}

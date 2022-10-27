#include "SP_Level.h"

using byte = unsigned char;

constexpr unsigned int LEVEL_W{ 60 }, LEVEL_H{ 24 },
OFFSET_TILES{ 0 }, OFFSET_GRAVITY{ 1444 }, OFFSET_UNUSED_BYTES{ 1440 },
OFFSET_SF_VERSION{ 1445 },
OFFSET_TITLE{ 1446 }, OFFSET_FREEZE_ZONKS{ 1469 }, OFFSET_IT_COUNT{ 1470 },
OFFSET_GP_COUNT{ 1471 }, OFFSET_GP{ 1472 }, OFFSET_SF_DEMO_BYTES{ 1532 },
LENGTH_TITLE{ 23 }, LENGTH_GP{ 6 }, LENGTH_SF_DEMO_BYTES{ 4 },
LENGTH_UNUSED_BYTES{ 4 },
OFFSET_SF_SOLUTION{ 1536 };

SP_Level::SP_Level(const std::vector<byte>& p_bytes) :
	m_player_x{ 0 }, m_player_y{ 0 } {

	// it is possible to have more than one player start
	// only the first found will be used, the rest will turn into "decorative" murphys
	bool l_player_found{ false };

	// initialize level map and player start
	for (std::size_t y{ 0 }; y < LEVEL_H; ++y) {
		std::vector<byte> l_row;
		for (std::size_t x{ 0 }; x < LEVEL_W; ++x) {
			byte l_value = p_bytes.at(OFFSET_TILES + LEVEL_W * y + x);
			if (l_value == 3 && !l_player_found) {
				m_player_x = static_cast<byte>(x);
				m_player_y = static_cast<byte>(y);
				l_row.push_back(0);
				l_player_found = true;
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

	// if a solution is appended, store it
	// only possible for individual SP-files
	m_sf_solution_bytes = std::vector<byte>(
		begin(p_bytes) + OFFSET_SF_SOLUTION,
		end(p_bytes)
		);
}

SP_Level::SP_Level(const std::string& p_title,
	const std::vector<std::vector<byte>>& p_tile_data,
	unsigned int p_px, unsigned int p_py, int p_solve_it_count, bool p_grav, bool p_fz,
	byte p_sf_version, const std::vector<byte>& p_sf_demo_bytes,
	const std::vector<byte>& p_unknown_bytes,
	const std::vector<byte>& p_solution_bytes) :
	m_title{ p_title }, m_tiles{ p_tile_data },
	m_player_x{ p_px }, m_player_y{ p_py }, m_solve_it_count{ static_cast<byte>(p_solve_it_count) },
	m_gravity{ p_grav }, m_freeze_zonks{ p_fz },
	m_sf_version{ p_sf_version }, m_sf_demo_bytes{ p_sf_demo_bytes },
	m_unused_bytes{ p_unknown_bytes }, m_sf_solution_bytes{ p_solution_bytes }
{ }

std::vector<byte> SP_Level::get_bytes(bool p_include_demo) const {
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

	// add a solution demo, should only be done when creating SP-files
	if (p_include_demo)
		result.insert(end(result),
			begin(m_sf_solution_bytes),
			end(m_sf_solution_bytes));

	return result;
}

// gravity port implementation
SP_Level::Gravity_port::Gravity_port(const std::vector<byte>& p_bytes) {
	int l_pos = (p_bytes.at(0) * 256 + p_bytes.at(1)) / 2;

	m_x = static_cast<byte>(l_pos % LEVEL_W);
	m_y = static_cast<byte>(l_pos / LEVEL_W);
	m_gravity = (p_bytes.at(2) == 1);
	m_freeze_zonks = (p_bytes.at(3) == 2);
	m_freeze_enemies = (p_bytes.at(4) == 1);
	m_unknown = p_bytes.at(5);
}

SP_Level::Gravity_port::Gravity_port(int p_x, int p_y, bool p_grav, bool p_fz, bool p_fe, byte p_unknown) :
	m_x{ static_cast<byte>(p_x) }, m_y{ static_cast<byte>(p_y) },
	m_gravity{ p_grav }, m_freeze_zonks{ p_fz }, m_freeze_enemies{ p_fe },
	m_unknown{ p_unknown }
{ }

std::vector<byte> SP_Level::Gravity_port::get_bytes(void) const {
	std::vector<byte> result;
	int l_pos = 2 * (m_x + LEVEL_W * m_y);

	result.push_back(static_cast<byte>(l_pos / 256));
	result.push_back(static_cast<byte>(l_pos % 256));
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

const std::vector<std::vector<byte>>& SP_Level::get_tile_data(void) const {
	return m_tiles;
}

const std::vector<byte>& SP_Level::get_sf_demo_bytes(void) const {
	return m_sf_demo_bytes;
}

const std::vector<byte>& SP_Level::get_unused_bytes(void) const {
	return m_unused_bytes;
}

const std::vector<byte>& SP_Level::get_solution_bytes(void) const {
	return m_sf_solution_bytes;
}

byte SP_Level::get_speedfix_version(void) const {
	return m_sf_version;
}

// statistics - count each tile type
std::map<byte, int> SP_Level::get_tile_counts(void) const {
	std::map<byte, int> result;

	for (const auto& row : m_tiles)
		for (byte v : row)
			++result[v];

	return result;
}

// gravity port getters
int SP_Level::get_gravity_port_count(void) const {
	return static_cast<int>(m_gravity_ports.size());
}

int SP_Level::get_gp_x(int p_gp_no) const {
	return static_cast<int>(m_gravity_ports.at(p_gp_no).m_x);
}

int SP_Level::get_gp_y(int p_gp_no) const {
	return static_cast<int>(m_gravity_ports.at(p_gp_no).m_y);
}

byte SP_Level::get_gp_unknown(int p_gp_no) const {
	return m_gravity_ports.at(p_gp_no).m_unknown;
}

bool SP_Level::get_gp_gravity(int p_gp_no) const {
	return m_gravity_ports.at(p_gp_no).m_gravity;
}

bool SP_Level::get_gp_freeze_zonks(int p_gp_no) const {
	return m_gravity_ports.at(p_gp_no).m_freeze_zonks;
}

bool SP_Level::get_gp_freeze_enemies(int p_gp_no) const {
	return m_gravity_ports.at(p_gp_no).m_freeze_enemies;
}


// setters
void SP_Level::set_tile_value(int p_x, int p_y, byte p_value) {
	m_tiles.at(p_y).at(p_x) = p_value;
}

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

void SP_Level::set_player_start(int p_x, int p_y) {
	m_player_x = p_x;
	m_player_y = p_y;
}

// gravity port setters
void SP_Level::set_gp_x(int p_gp_no, int p_x) {
	m_gravity_ports.at(p_gp_no).m_x = static_cast<byte>(p_x);
}

void SP_Level::set_gp_y(int p_gp_no, int p_y) {
	m_gravity_ports.at(p_gp_no).m_y = static_cast<byte>(p_y);
}

void SP_Level::set_gp_gravity(int p_gp_no, bool p_param) {
	m_gravity_ports.at(p_gp_no).m_gravity = p_param;
}

void SP_Level::set_gp_freeze_zonks(int p_gp_no, bool p_param) {
	m_gravity_ports.at(p_gp_no).m_freeze_zonks = p_param;
}

void SP_Level::set_gp_freeze_enemies(int p_gp_no, bool p_param) {
	m_gravity_ports.at(p_gp_no).m_freeze_enemies = p_param;
}

void SP_Level::delete_gravity_port(int p_gp_no) {
	m_gravity_ports.erase(begin(m_gravity_ports) + p_gp_no);
}

void SP_Level::add_gravity_port(int p_x, int p_y, bool p_grav, bool p_fz, bool p_fe, byte p_unknown) {
	m_gravity_ports.push_back(Gravity_port(p_x, p_y, p_grav, p_fz, p_fe, p_unknown));
}

// returns index of gravity port at the given position, -1 if there is none
int SP_Level::has_gp_at_pos(int p_x, int p_y) {
	for (std::size_t i{ 0 }; i < m_gravity_ports.size(); ++i)
		if (m_gravity_ports[i].m_x == p_x && m_gravity_ports[i].m_y == p_y)
			return static_cast<int>(i);
	return -1;
}

// check if a gravity port psoition is using a gravity port tile (#13-#16)
bool SP_Level::get_gp_status(int p_gp_no) const {
	byte l_tile_no = get_tile_no(get_gp_x(p_gp_no), get_gp_y(p_gp_no));
	return l_tile_no >= 13 && l_tile_no <= 16;
}

// static members
std::vector<std::string> SP_Level::sm_descriptions = {
  "Empty Space",
  "Zonk",
  "Base",
  "Player Start",
  "Infotron",
  "RAM Chip - Chip",
  "Wall",
  "Exit",
  "Floppy - Orange",
  "Port - Right",
  "Port - Down",
  "Port - Left",
  "Port - Up",
  "Gravity Port - Right",
  "Gravity Port - Down",
  "Gravity Port - Left",
  "Gravity Port - Up",
  "Snik Snak",
  "Floppy - Yellow",
  "Terminal",
  "Floppy - Red",
  "Port - Two-Way Vertical",
  "Port - Two-Way Horizontal",
  "Port - Four-Way",
  "Electron",
  "Bug",
  "RAM Chip - Left",
  "RAM Chip - Right",
  "Hardware 01",
  "Hardware 02",
  "Hardware 03",
  "Hardware 04",
  "Hardware 05",
  "Hardware 06",
  "Hardware 07",
  "Hardware 08",
  "Hardware 09",
  "Hardware 10",
  "RAM Chip - Top",
  "RAM Chip - Bottom"
};

std::string& SP_Level::get_description(int p_tile_no) {
	return sm_descriptions.at(p_tile_no);
}

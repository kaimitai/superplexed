#include "SP_Level.h"
#include "./../SP_Constants.h"

using byte = unsigned char;

SP_Level::SP_Level(const std::vector<byte>& p_bytes) :
	m_player_x{ 0 }, m_player_y{ 0 } {

	// it is possible to have more than one player start
	// only the first found will be used, the rest will turn into "decorative" murphys
	bool l_player_found{ false };

	// initialize level map and player start
	for (std::size_t y{ 0 }; y < c::LEVEL_H; ++y) {
		std::vector<byte> l_row;
		for (std::size_t x{ 0 }; x < c::LEVEL_W; ++x) {
			byte l_value = p_bytes.at(c::OFFSET_TILES + c::LEVEL_W * y + x);
			if (l_value == c::TILE_NO_MURPHY && !l_player_found) {
				m_player_x = static_cast<byte>(x);
				m_player_y = static_cast<byte>(y);
				l_row.push_back(0);
				l_player_found = true;
			}
			else
				l_row.push_back(l_value > c::TILE_NO_INVISIBLE ? c::TILE_NO_INVISIBLE : l_value);
		}
		m_tiles.push_back(l_row);
	}

	// initialize other metadata
	m_gravity = (p_bytes.at(c::OFFSET_GRAVITY) == 1);
	m_sf_version = p_bytes.at(c::OFFSET_SF_VERSION);
	m_title = std::string(begin(p_bytes) + c::OFFSET_TITLE,
		begin(p_bytes) + c::OFFSET_TITLE + c::LENGTH_TITLE);
	m_freeze_zonks = (p_bytes.at(c::OFFSET_FREEZE_ZONKS) == 2);
	m_solve_it_count = p_bytes.at(c::OFFSET_IT_COUNT);

	m_unused_bytes = std::vector<byte>(begin(p_bytes) + c::OFFSET_UNUSED_BYTES,
		begin(p_bytes) + c::OFFSET_UNUSED_BYTES + c::LENGTH_UNUSED_BYTES);
	m_sf_demo_bytes = std::vector<byte>(begin(p_bytes) + c::OFFSET_SF_DEMO_BYTES,
		begin(p_bytes) + c::OFFSET_SF_DEMO_BYTES + c::LENGTH_SF_DEMO_BYTES);

	// initialize gravity ports
	byte l_gp_count = p_bytes.at(c::OFFSET_GP_COUNT);
	for (std::size_t i{ 0 }; i < l_gp_count; ++i) {
		std::size_t l_offset = c::OFFSET_GP + c::LENGTH_GP * i;
		m_gravity_ports.push_back(std::vector<byte>(begin(p_bytes) + l_offset,
			begin(p_bytes) + l_offset + c::LENGTH_GP));
	}

	// if a solution is appended, store it
	// only possible for individual SP-files
	m_sf_solution_bytes = std::vector<byte>(
		begin(p_bytes) + c::LVL_DATA_BYTE_SIZE,
		end(p_bytes)
		);
}

// default level
SP_Level::SP_Level(void) :
	m_title{ c::DEFAULT_LVL_TITLE }, m_solve_it_count{ 1 },
	m_sf_version{ c::DEFAULT_LVL_SF_VERSION }, m_gravity{ false }, m_freeze_zonks{ false },
	m_sf_demo_bytes{ std::vector<byte>(4, 0) }, m_unused_bytes{ std::vector<byte>(4, 0) }
{
	m_tiles = std::vector<std::vector<byte>>(c::LEVEL_H, std::vector<byte>(c::LEVEL_W, c::TILE_NO_BASE));
	set_tile_value(c::LEVEL_W / 2, c::LEVEL_H / 2, c::TILE_NO_INFOTRON);
	set_tile_value(c::LEVEL_W / 2 + 1, c::LEVEL_H / 2, c::TILE_NO_EXIT);
	set_player_start(c::LEVEL_W / 2 - 1, c::LEVEL_H / 2);
	apply_wall_border();
}

void SP_Level::apply_wall_border(void) {
	for (int i{ 0 }; i < c::LEVEL_W; ++i) {
		m_tiles.at(0).at(i) = c::TILE_NO_WALL;
		m_tiles.at(static_cast<std::size_t>(c::LEVEL_H - 1)).at(i) = c::TILE_NO_WALL;
	}
	for (int i{ 0 }; i < c::LEVEL_H; ++i) {
		m_tiles.at(i).at(0) = c::TILE_NO_WALL;
		m_tiles.at(i).at(static_cast<std::size_t>(c::LEVEL_W - 1)) = c::TILE_NO_WALL;
	}
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
	for (int i{ 0 }; i < c::LEVEL_H; ++i)
		result.insert(end(result), begin(m_tiles.at(i)), end(m_tiles.at(i)));
	// update tile data with player start
	result.at(static_cast<std::size_t>(m_player_y * c::LEVEL_W + m_player_x)) = 3;

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

	const std::string l_FILE_SIGNATURE{ c::FILE_SIGNATURE };
	auto iter = begin(l_FILE_SIGNATURE);
	for (std::size_t i{ c::MAX_GP_COUNT }; i > m_gravity_ports.size(); --i)
		for (int j{ 0 }; j < c::LENGTH_GP; ++j) {
			//result.push_back(0);
			result.push_back(*iter++);
			if (iter == end(l_FILE_SIGNATURE))
				iter = begin(l_FILE_SIGNATURE);
		}

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

	m_x = static_cast<byte>(l_pos % c::LEVEL_W);
	m_y = static_cast<byte>(l_pos / c::LEVEL_W);
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
	int l_pos = 2 * (m_x + c::LEVEL_W * m_y);

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
std::vector<int> SP_Level::get_tile_counts(void) const {
	std::vector<int> result(c::TILE_COUNT, 0);

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
	int l_gp_index{ has_gp_at_pos(p_x, p_y) };

	if (is_special_port(p_value)) {
		if (l_gp_index == -1 &&
			m_gravity_ports.size() < c::MAX_GP_COUNT)
			add_gravity_port(p_x, p_y);
	}
	else if (l_gp_index != -1)
		delete_gravity_port(l_gp_index);

	m_tiles.at(p_y).at(p_x) = p_value;
}

void SP_Level::set_title(const std::string& p_title) {
	m_title = sanitize_sp_string(p_title, c::LENGTH_TITLE);
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

std::set<std::pair<int, int>> SP_Level::get_gp_positions(void) const {
	std::set<std::pair<int, int>> result;
	for (const auto& gp : m_gravity_ports)
		result.insert(std::make_pair(gp.m_x, gp.m_y));
	return result;
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
	return is_special_port(l_tile_no);
}

// static members
std::vector<std::string> SP_Level::sm_descriptions = {
c::TILE_DESC_EMPTY, c::TILE_DESC_ZONK, c::TILE_DESC_BASE, c::TILE_DESC_MURPHY, c::TILE_DESC_INFOTRON, c::TILE_DESC_RAMCHIP, c::TILE_DESC_WALL, c::TILE_DESC_EXIT, c::TILE_DESC_FLOPPY_O, c::TILE_DESC_PORT_RIGHT, c::TILE_DESC_PORT_DOWN,
c::TILE_DESC_PORT_LEFT, c::TILE_DESC_PORT_UP, c::TILE_DESC_GP_RIGHT, c::TILE_DESC_FP_DOWN, c::TILE_DESC_GP_LEFT, c::TILE_DESC_GP_UP, c::TILE_DESC_SNIKSNAK, c::TILE_DESC_FLOPPY_Y, c::TILE_DESC_TERMINAL, c::TILE_DESC_FLOPPY_R,
c::TILE_DESC_PORT2WAY_V, c::TILE_DESC_PORT2WAY_H, c::TILE_DESC_PORT4WAY, c::TILE_DESC_ELECTRON, c::TILE_DESC_BUG, c::TILE_DESC_RAM_LEFT, c::TILE_DESC_RAM_RIGHT, c::TILE_DESC_HW01, c::TILE_DESC_HW02, c::TILE_DESC_HW03, c::TILE_DESC_HW04,
c::TILE_DESC_HW05, c::TILE_DESC_HW06, c::TILE_DESC_HW07, c::TILE_DESC_HW08, c::TILE_DESC_HW09, c::TILE_DESC_HW10, c::TILE_DESC_RAM_TOP, c::TILE_DESC_RAM_BOTTOM, c::TILE_DESC_INVISIBLE_TILE, c::TILE_DESC_PLAYER_START
};

std::string& SP_Level::get_description(int p_tile_no) {
	return sm_descriptions.at(p_tile_no);
}

std::string SP_Level::sanitize_sp_string(const std::string& p_str, int p_size,
	unsigned char p_cmin, unsigned char p_cmax) {
	std::string result;

	for (char c : p_str) {
		if (c >= 'a' && c <= 'z')
			result.push_back(c - ('a' - 'A'));
		else if (c < p_cmin || c > p_cmax)
			result.push_back('?');
		else
			result.push_back(c);
	}

	// if the input is too long, we drop the tail end
	while (result.size() > p_size)
		result.pop_back();

	// if the input is too short, we make the string the required length
	bool l_prepend{ true };
	while (result.size() < p_size) {
		if (p_size == c::LENGTH_TITLE) {
			if (l_prepend)
				result.insert(begin(result), '-');
			else
				result.push_back('-');
			l_prepend = !l_prepend;
		}
		else
			result.insert(begin(result), ' ');
	}

	return result;
}

bool SP_Level::is_special_port(byte p_tile_no) {
	return p_tile_no >= c::TILE_NO_GP_RIGHT && p_tile_no <= c::TILE_NO_GP_UP;
}

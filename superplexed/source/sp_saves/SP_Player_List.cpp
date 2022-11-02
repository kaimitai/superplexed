#include <algorithm>
#include "SP_Player_list.h"
#include "./../SP_Constants.h"

SP_Player_list::SP_Player::SP_Player(void) :
	m_name{ c::EMPTY_PLAYER_NAME },
	m_hrs{ 0 }, m_mins{ 0 }, m_secs{ 0 }, // m_current_level{ 0 }
	m_level_status{ std::vector<byte>(c::DEFAULT_LEVEL_COUNT, 0x00) }
{ }

SP_Player_list::SP_Player::SP_Player(const std::vector<byte>& p_bytes) {
	m_name = std::string(begin(p_bytes), begin(p_bytes) + c::PLAYER_NAME_SIZE);
	// skip next byte
	m_hrs = p_bytes.at(c::PLAYER_NAME_SIZE + 1);
	m_mins = p_bytes.at(c::PLAYER_NAME_SIZE + 2);
	m_secs = p_bytes.at(c::PLAYER_NAME_SIZE + 3);
	// read all level status
	for (int i{ 0 }; i < c::DEFAULT_LEVEL_COUNT; ++i)
		m_level_status.push_back(p_bytes.at(c::PLAYER_NAME_SIZE + 4 + i));
	// only the third of the last four bytes are in use
	// even this is not in use probably, as it is calculated by the game - we will just calculate it to when requesting bytes
	// m_current_level = p_bytes.at(c::PLAYER_ENTRY_SIZE - 2);
}

std::vector<byte> SP_Player_list::SP_Player::to_bytes(void) const {
	std::vector<byte> result(begin(m_name), end(m_name));
	byte l_cur_level = static_cast<byte>(get_current_level());

	result.push_back(0x00); // unused
	result.push_back(m_hrs);
	result.push_back(m_mins);
	result.push_back(m_secs);
	result.insert(end(result), begin(m_level_status), end(m_level_status));
	result.push_back(0x00); // unused
	result.push_back(0x00); // unused
	result.push_back(0x00); // unused
	result.push_back(l_cur_level);
	result.push_back(0x00); // unused

	return result;
}

SP_Player_list::SP_Player_list(void) {
	m_players = std::vector<SP_Player>(c::PLAYER_COUNT, SP_Player());
}

SP_Player_list::SP_Player_list(const std::vector<byte>& p_bytes) {
	for (std::size_t i{ 0 }; i < p_bytes.size(); i += c::PLAYER_ENTRY_SIZE)
		m_players.push_back(SP_Player(std::vector<byte>(
			begin(p_bytes) + i, begin(p_bytes) + i + c::PLAYER_ENTRY_SIZE
			)));
}

std::vector<byte> SP_Player_list::to_bytes(void) const {
	std::vector<byte> result;
	for (const auto& p : m_players) {
		auto l_entry_bytes = p.to_bytes();
		result.insert(end(result), begin(l_entry_bytes), end(l_entry_bytes));
	}
	return result;
}

// getters
byte SP_Player_list::get_status(int p_player_no, int p_level_no) const {
	return m_players.at(p_player_no).m_level_status.at(p_level_no);
}

int SP_Player_list::SP_Player::get_current_level(void) const {
	if (m_level_status[0] == 0 && m_name == c::EMPTY_PLAYER_NAME)
		return 0;

	for (std::size_t i{ 0 }; i < m_level_status.size(); ++i)
		if (m_level_status[i] == 0)
			return static_cast<int>(i + 1);

	return 113; // if all levels are solved, this is what is written to file for some reason
}

int SP_Player_list::get_current_level(int p_player_no) const {
	return m_players.at(p_player_no).get_current_level();
}

std::string SP_Player_list::get_name(int p_player_no) const {
	return m_players.at(p_player_no).m_name;
}

int SP_Player_list::get_hrs(int p_player_no) const {
	return m_players.at(p_player_no).m_hrs;
}

int SP_Player_list::get_mins(int p_player_no) const {
	return m_players.at(p_player_no).m_mins;
}

int SP_Player_list::get_secs(int p_player_no) const {
	return m_players.at(p_player_no).m_secs;
}

// setters
void SP_Player_list::toggle_status(int p_player_no, int p_level_no) {
	++m_players.at(p_player_no).m_level_status.at(p_level_no);
	m_players.at(p_player_no).m_level_status.at(p_level_no) %= 3;
}

void SP_Player_list::set_name(int p_player_no, const std::string& p_name) {
	m_players.at(p_player_no).m_name = p_name;
}

void SP_Player_list::set_hrs(int p_player_no, int p_hrs) {
	m_players.at(p_player_no).m_hrs = p_hrs;
}

void SP_Player_list::set_mins(int p_player_no, int p_mins) {
	m_players.at(p_player_no).m_mins = p_mins;
}

void SP_Player_list::set_secs(int p_player_no, int p_secs) {
	m_players.at(p_player_no).m_secs = p_secs;
}

void SP_Player_list::delete_player(int p_player_no) {
	m_players.erase(begin(m_players) + p_player_no);
	m_players.push_back(SP_Player());
}

void SP_Player_list::move_player(int p_player_no, bool p_up) {
	std::swap(m_players.at(p_player_no), m_players.at(p_player_no + (p_up ? -1 : 1)));
}

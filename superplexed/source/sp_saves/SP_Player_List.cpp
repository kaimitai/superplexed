#include "SP_Player_list.h"
#include "./../SP_Constants.h"

SP_Player_list::SP_Player::SP_Player(void) :
	m_name{ std::string(c::PLAYER_NAME_SIZE, '-') },
	m_hrs{ 0 }, m_mins{ 0 }, m_secs{ 0 }, m_current_level{ 0 }
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
	m_current_level = p_bytes.at(c::PLAYER_ENTRY_SIZE - 2);
}

std::vector<byte> SP_Player_list::SP_Player::to_bytes(void) const {
	std::vector<byte> result(begin(m_name), end(m_name));
	result.push_back(0x00); // unused
	result.push_back(m_hrs);
	result.push_back(m_mins);
	result.push_back(m_secs);
	result.insert(end(result), begin(m_level_status), end(m_level_status));
	result.push_back(0x00); // unused
	result.push_back(0x00); // unused
	result.push_back(0x00); // unused
	result.push_back(m_current_level);
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

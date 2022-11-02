#include "SP_Hallfame.h"
#include "./../SP_Constants.h"

SP_Hallfame::HF_Entry::HF_Entry(void) :
	m_hrs{ 0 }, m_mins{ 0 }, m_secs{ 0 },
	m_name{ std::string(' ', c::PLAYER_NAME_SIZE) }
{}

SP_Hallfame::HF_Entry::HF_Entry(const std::string& p_name,
	byte p_hrs, byte p_mins, byte p_secs) :
	m_hrs{ p_hrs }, m_mins{ p_mins }, m_secs{ p_secs },
	m_name{ p_name }
{ }

SP_Hallfame::SP_Hallfame(void) {
	for (int i{ 0 }; i < c::HALLFAME_ENTRY_COUNT; ++i)
		m_entries.push_back(HF_Entry());
}

SP_Hallfame::SP_Hallfame(const std::vector<byte>& p_bytes) {
	for (std::size_t i{ 0 }; i < p_bytes.size(); i += c::HALLFAME_ENTRY_SIZE) {
		std::string l_name(begin(p_bytes) + i, begin(p_bytes) + i + c::PLAYER_NAME_SIZE);
		// ignore the byte immediately after name
		byte l_hrs = p_bytes.at(i + c::PLAYER_NAME_SIZE + 1);
		byte l_mins = p_bytes.at(i + c::PLAYER_NAME_SIZE + 2);
		byte l_secs = p_bytes.at(i + c::PLAYER_NAME_SIZE + 3);
		m_entries.push_back(HF_Entry(l_name, l_hrs, l_mins, l_secs));
	}
}

std::vector<byte> SP_Hallfame::to_bytes(void) const {
	std::vector<byte> result;

	for (const auto& entry : m_entries) {
		result.insert(end(result), begin(entry.m_name), end(entry.m_name));
		result.push_back(0x00); // seems to be an unused byte
		result.push_back(entry.m_hrs);
		result.push_back(entry.m_mins);
		result.push_back(entry.m_secs);
	}

	return result;
}

std::string SP_Hallfame::get_name(int p_entry_no) const {
	return m_entries.at(p_entry_no).m_name;
}

byte SP_Hallfame::get_hrs(int p_entry_no) const {
	return m_entries.at(p_entry_no).m_hrs;
}

byte SP_Hallfame::get_mins(int p_entry_no) const {
	return m_entries.at(p_entry_no).m_mins;
}

byte SP_Hallfame::get_secs(int p_entry_no) const {
	return m_entries.at(p_entry_no).m_secs;
}

void SP_Hallfame::set_name(int p_entry_no, const std::string& p_name) {
	m_entries.at(p_entry_no).m_name = p_name;
}

void SP_Hallfame::set_hrs(int p_entry_no, byte p_hrs) {
	m_entries.at(p_entry_no).m_hrs = p_hrs;
}

void SP_Hallfame::set_mins(int p_entry_no, byte p_mins) {
	m_entries.at(p_entry_no).m_mins = p_mins;
}

void SP_Hallfame::set_secs(int p_entry_no, byte p_secs) {
	m_entries.at(p_entry_no).m_secs = p_secs;
}

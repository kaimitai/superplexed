#ifndef SP_PLAYER_LIST_H
#define SP_PLAYER_LIST_H

#include <string>
#include <vector>

using byte = unsigned char;

class SP_Player_list {

	struct SP_Player {
		std::string m_name;
		byte m_hrs, m_mins, m_secs, m_current_level;
		std::vector<byte> m_level_status;
		SP_Player(void);
		SP_Player(const std::vector<byte>& p_bytes);
		std::vector<byte> to_bytes(void) const;
	};

	std::vector<SP_Player> m_players;

public:
	SP_Player_list(void);
	SP_Player_list(const std::vector<byte>& p_bytes);
	std::vector<byte> to_bytes(void) const;
};

#endif

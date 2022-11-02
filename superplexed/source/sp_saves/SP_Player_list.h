#ifndef SP_PLAYER_LIST_H
#define SP_PLAYER_LIST_H

#include <string>
#include <vector>

using byte = unsigned char;

class SP_Player_list {

	struct SP_Player {
		std::string m_name;
		byte m_hrs, m_mins, m_secs; // , m_current_level;
		std::vector<byte> m_level_status;
		SP_Player(void);
		SP_Player(const std::vector<byte>& p_bytes);
		std::vector<byte> to_bytes(void) const;
		int get_current_level(void) const;
	};

	std::vector<SP_Player> m_players;

public:
	SP_Player_list(void);
	SP_Player_list(const std::vector<byte>& p_bytes);
	std::vector<byte> to_bytes(void) const;

	// getters
	byte get_status(int p_player_no, int p_level_no) const;
	int get_current_level(int p_player_no) const;
	std::string get_name(int p_player_no) const;
	int get_hrs(int p_player_no) const;
	int get_mins(int p_player_no) const;
	int get_secs(int p_player_no) const;

	// setters
	void toggle_status(int p_player_no, int p_level_no);
	void set_name(int p_player_no, const std::string& p_name);
	void set_hrs(int p_player_no, int p_hrs);
	void set_mins(int p_player_no, int p_mins);
	void set_secs(int p_player_no, int p_secs);
	void delete_player(int p_player_no);
	void move_player(int p_player_no, bool p_up);
};

#endif

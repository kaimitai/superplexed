#ifndef SP_LEVEL_H
#define SP_LEVEL_H

#include <string>
#include <vector>
#include <utility>

using byte = unsigned char;

class SP_Level {

	struct Gravity_port {
		byte m_x, m_y, m_unknown;
		bool m_gravity, m_freeze_zonks, m_freeze_enemies;
		Gravity_port(const std::vector<byte>& p_bytes);
		std::vector<byte> get_bytes(void) const;
	};

	std::string m_title;
	std::vector<std::vector<byte>> m_tiles;
	std::vector<Gravity_port> m_gravity_ports;
	std::vector<byte> m_unused_bytes, m_sf_demo_bytes;
	bool m_gravity, m_freeze_zonks;
	unsigned int m_player_x, m_player_y;
	byte m_solve_it_count, m_sf_version;

public:
	SP_Level(const std::vector<byte>& p_bytes);
	std::vector<byte> get_bytes(void) const;

	byte get_tile_no(int p_x, int p_y) const;
	std::pair<byte, byte> get_start_pos(void) const;
};

#endif

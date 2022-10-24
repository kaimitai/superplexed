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
		Gravity_port(int p_x, int p_y, bool p_grav, bool p_fz, bool p_fe, byte p_unknown = 0);
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
	SP_Level(const std::string& p_title,
		const std::vector<std::vector<byte>>& p_tile_data,
		unsigned int p_px, unsigned int p_py, int p_solve_it_count, bool p_grav, bool p_fz,
		byte p_sf_version, const std::vector<byte>& p_sf_demo_bytes,
		const std::vector<byte>& p_unknown_bytes);
	std::vector<byte> get_bytes(void) const;

	// getters
	byte get_tile_no(int p_x, int p_y) const;
	std::pair<byte, byte> get_start_pos(void) const;
	const std::string& get_title(void) const;
	int get_solve_it_count(void) const;
	bool get_gravity(void) const;
	bool get_freeze_zonks(void) const;
	const std::vector<std::vector<byte>>& get_tile_data(void) const;
	const std::vector<byte>& get_sf_demo_bytes(void) const;
	const std::vector<byte>& get_unused_bytes(void) const;
	byte get_speedfix_version(void) const;

	// gravity port getters
	int get_gravity_port_count(void) const;
	int get_gp_x(int p_gp_no) const;
	int get_gp_y(int p_gp_no) const;
	byte get_gp_unknown(int p_gp_no) const;
	bool get_gp_gravity(int p_gp_no) const;
	bool get_gp_freeze_zonks(int p_gp_no) const;
	bool get_gp_freeze_enemies(int p_gp_no) const;

	// setters
	void set_tile_value(int p_x, int p_y, byte p_value);
	void set_title(const std::string& p_title);
	void set_solve_it_count(int p_count);
	void set_gravity(bool p_param);
	void set_freeze_zonks(bool p_param);

	// gravity port setters
	void set_gp_x(int p_gp_no, int p_x);
	void set_gp_y(int p_gp_no, int p_x);
	void set_gp_gravity(int p_gp_no, bool p_param);
	void set_gp_freeze_zonks(int p_gp_no, bool p_param);
	void set_gp_freeze_enemies(int p_gp_no, bool p_param);
	void delete_gravity_port(int p_gp_no);
	void add_gravity_port(int p_x, int p_y, bool p_grav, bool p_fz, bool p_fe, byte p_unknown = 0);
};

#endif

#ifndef SP_LEVEL_UNDO_INTERFACE_H
#define SP_LEVEL_UNDO_INTERFACE_H

#include "SP_Level.h"

#include <deque>
#include <set>
#include <map>

using byte = unsigned char;

class SP_Level_undo_interface {

	std::deque<std::map<std::pair<byte, byte>, byte>> m_history, m_redo;
	std::map<std::pair<byte, byte>, byte> m_working_block;

	std::map<std::pair<byte, byte>, byte> create_opposite_block(const SP_Level& p_level, const std::map<std::pair<byte, byte>, byte>& p_block);
	static unsigned int ms_history_size;

public:
	SP_Level_undo_interface(void) = default;
	void commit_block(void);
	void set_tile_value(SP_Level& p_level, int p_x, int p_y, byte p_value, bool p_autocommit = false);
	static void set_undo_history_size(unsigned int p_history_size);

	bool apply_undo(SP_Level& p_level);
	bool apply_redo(SP_Level& p_level);
};

#endif

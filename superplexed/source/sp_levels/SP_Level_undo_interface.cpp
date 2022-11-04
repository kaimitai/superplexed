#include "SP_Level_undo_interface.h"
#include "./../SP_Constants.h"

void SP_Level_undo_interface::commit_block(void) {
	if (m_working_block.size() > 0) {
		m_history.push_back(m_working_block);
		m_working_block.clear();

		if (m_history.size() > c::UNDO_HISTORY_COUNT)
			m_history.pop_front();
	}
}

void SP_Level_undo_interface::set_tile_value(SP_Level& p_level, int p_x, int p_y, byte p_value, bool p_autocommit) {
	byte l_old = p_level.get_tile_no(p_x, p_y);
	if (l_old != p_value) {
		p_level.set_tile_value(p_x, p_y, p_value);
		m_working_block.insert(std::make_pair(std::make_pair(p_x, p_y), l_old));
	}
	if (p_autocommit)
		commit_block();
}

bool SP_Level_undo_interface::apply_undo(SP_Level& p_level) {
	if (!m_working_block.empty())
		throw std::exception("Tried to apply undo with uncommitted change block");

	if (m_history.empty())
		return false;
	else {
		for (const auto& kv : m_history.back())
			p_level.set_tile_value(kv.first.first, kv.first.second, kv.second);
		m_history.pop_back();
		return true;
	}
}

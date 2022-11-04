#include "SP_Level_undo_interface.h"

constexpr std::size_t UNDO_HISTORY_COUNT{ 150 };

void SP_Level_undo_interface::commit_block(void) {
	if (m_working_block.size() > 0) {
		m_history.push_back(m_working_block);
		m_working_block.clear();

		if (m_history.size() > UNDO_HISTORY_COUNT)
			m_history.pop_front();
	}
}

void SP_Level_undo_interface::set_tile_value(SP_Level& p_level, int p_x, int p_y, byte p_value) {
	byte l_old = p_level.get_tile_no(p_x, p_y);
	if (l_old != p_value) {
		p_level.set_tile_value(p_x, p_y, p_value);
		m_working_block.insert(std::make_tuple(p_x, p_y, p_value));
	}
}

bool SP_Level_undo_interface::apply_undo(SP_Level& p_level) {
	commit_block();
	if (m_history.empty())
		return false;
	else {
		for (const auto& p_triple : m_history.back())
			p_level.set_tile_value(std::get<0>(p_triple), std::get<1>(p_triple), std::get<2>(p_triple));
		m_history.pop_back();
		return true;
	}
}

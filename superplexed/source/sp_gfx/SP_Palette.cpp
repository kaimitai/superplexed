#include "SP_Palette.h"

SP_Palette::SP_Palette(const std::vector<byte>& p_bytes) {
	for (std::size_t i{ 0 }; i < p_bytes.size(); i += 4)
		m_palette.push_back(std::make_tuple(16 * p_bytes[i],
			16 * p_bytes[i + 1],
			16 * p_bytes[i + 2],
			16 * p_bytes[i + 3]));
}

const std::tuple<byte, byte, byte, byte>& SP_Palette::get_color(int p_color_no) const {
	return m_palette.at(p_color_no);
}

std::size_t SP_Palette::get_size(void) const {
	return m_palette.size();
}

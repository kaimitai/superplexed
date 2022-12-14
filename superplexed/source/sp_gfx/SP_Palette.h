#ifndef SP_PALETTE_H
#define SP_PALETTE_H

#include <vector>
#include <tuple>

using byte = unsigned char;

class SP_Palette {

	std::vector<std::tuple<byte, byte, byte, byte>> m_palette;

public:
	SP_Palette(const std::vector<byte>& p_bytes);
	const std::tuple<byte, byte, byte, byte>& get_color(int p_color_no) const;
	std::size_t get_size(void) const;
	std::vector<byte> to_bytes(void) const;

};

#endif

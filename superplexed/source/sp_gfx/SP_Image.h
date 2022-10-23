#ifndef SP_IMAGE_H
#define SP_IMAGE_H

#include <vector>

using byte = unsigned char;

class SP_Image {

	std::vector<std::vector<byte>> m_pixels;
	int get_bit(int, int) const;

public:
	SP_Image(const std::vector<byte>& p_bytes, int p_w);
	byte get_palette_index(int p_x, int p_y) const;
	int get_w(void) const;
	int get_h(void) const;
};

#endif

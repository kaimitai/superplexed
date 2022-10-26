#ifndef SP_IMAGE_H
#define SP_IMAGE_H

#include <vector>

using byte = unsigned char;

class SP_Image {

	bool m_binary;

	std::vector<std::vector<byte>> m_pixels;
	std::vector<byte> m_unknown_data;
	void parse_binary_image(const std::vector<byte>& p_bytes, int p_w);
	void parse_planar_image(const std::vector<byte>& p_bytes, int p_w);
	std::vector<byte> to_binary_bytes(void) const;
	std::vector<byte> to_planar_bytes(void) const;

public:
	SP_Image(const std::vector<byte>& p_bytes, int p_w, bool p_binary = false);
	byte get_palette_index(int p_x, int p_y) const;
	int get_w(void) const;
	int get_h(void) const;
	std::vector<byte> to_bytes(void) const;
};

#endif

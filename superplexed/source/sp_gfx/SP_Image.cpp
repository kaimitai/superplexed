#include "SP_Image.h"

SP_Image::SP_Image(const std::vector<byte>& p_bytes, int p_w, int p_h) {

	for (std::size_t y{ 0 }; y < p_h; ++y) {
		std::vector<byte> l_row;
		for (std::size_t index{ y * p_w / 2 }; index < (y + 1) * p_w / 2; ++index) {
			l_row.push_back(p_bytes.at(index) >> 4);
			l_row.push_back(p_bytes.at(index) & 0x0F);
		}
		m_pixels.push_back(l_row);
	}

}

byte SP_Image::get_palette_index(int p_x, int p_y) const {
	return m_pixels.at(p_y).at(p_x);
}

int SP_Image::get_w(void) const {
	return (m_pixels.size() > 0 ? static_cast<int>(m_pixels.at(0).size()) : 0);
}

int SP_Image::get_h(void) const {
	return static_cast<int>(m_pixels.size());
}

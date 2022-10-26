#include "SP_Image.h"

SP_Image::SP_Image(const std::vector<byte>& p_bytes, int p_w, bool p_binary) : m_binary{ p_binary } {

	if (p_binary) {
		parse_binary_image(p_bytes, p_w);
	}
	else {
		parse_planar_image(p_bytes, p_w);
	}
}

void SP_Image::parse_binary_image(const std::vector<byte>& p_bytes, int p_w) {

	int p_h = static_cast<int>(p_bytes.size()) * 8 / p_w;

	for (int j = 0; j < p_h; ++j) {
		std::vector<byte> l_row;
		for (int i = 0; i < p_w / 8; i++)
			for (int b = 0; b < 8; ++b)
				l_row.push_back((p_bytes.at(j * p_w / 8 + i) >> (7 - b)) & 1);
		m_pixels.push_back(l_row);
	}

}

void SP_Image::parse_planar_image(const std::vector<byte>& p_bytes, int p_w) {
	int p_h = 2 * static_cast<int>(p_bytes.size()) / p_w;

	m_pixels = std::vector<std::vector<byte>>(p_h, std::vector<byte>(p_w, 0));

	for (int y = 0; y < p_h; y++) {
		std::vector<byte> fileData = std::vector<byte>(begin(p_bytes) + y * p_w / 2, begin(p_bytes) + (y + 1) * p_w / 2);

		for (int x = 0; x < p_w; ++x) {
			uint16_t destPixelAddress = y * p_w + x;
			std::size_t sourcePixelAddress = x / 8;
			uint16_t sourcePixelBitPosition = 7 - (x % 8);

			uint8_t b = (fileData.at(sourcePixelAddress + 0 * (p_w / 2) / 4) >> sourcePixelBitPosition) & 0x1;
			uint8_t g = (fileData.at(sourcePixelAddress + 1 * (p_w / 2) / 4) >> sourcePixelBitPosition) & 0x1;
			uint8_t r = (fileData.at(sourcePixelAddress + 2 * (p_w / 2) / 4) >> sourcePixelBitPosition) & 0x1;
			uint8_t i = (fileData.at(sourcePixelAddress + 3 * (p_w / 2) / 4) >> sourcePixelBitPosition) & 0x1;

			uint8_t finalColor = ((b << 0)
				| (g << 1)
				| (r << 2)
				| (i << 3));

			m_pixels[y][x] = finalColor;
		}
	}

	// only one byte really, and only for some image files
	m_unknown_data = std::vector<byte>(begin(p_bytes) + p_w * p_h / 2, end(p_bytes));
}

byte SP_Image::get_palette_index(int p_x, int p_y) const {
	return m_pixels.at(p_y).at(p_x);
}

int SP_Image::get_w(void) const {
	return (m_pixels.size() > 0 ? static_cast<int>(m_pixels[0].size()) : 0);
}

int SP_Image::get_h(void) const {
	return static_cast<int>(m_pixels.size());
}

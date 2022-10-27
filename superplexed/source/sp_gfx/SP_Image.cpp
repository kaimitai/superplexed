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

std::vector<byte> SP_Image::to_bytes(void) const {
	if (m_binary)
		return to_binary_bytes();
	else
		return to_planar_bytes();
}

std::vector<byte> SP_Image::to_binary_bytes(void) const {
	std::vector<byte> result;

	// assume the image width is divisible by 8
	for (const auto& row : m_pixels)
		for (std::size_t i{ 0 }; i < row.size(); i += 8) {
			byte l_value{ 0 };
			for (std::size_t j{ 0 }; j < 8; ++j)
				l_value = 2 * l_value + row[i + j];
			result.push_back(l_value);
		}

	return result;
}

std::vector<byte> SP_Image::to_planar_bytes(void) const {

	int img_area = get_w() * get_h();
	int bit_count = img_area * 4;
	int byte_count = img_area / 2;
	int plane_offset = byte_count / 4;
	std::vector<byte> result(byte_count, 0);

	for (std::size_t j = 0; j < get_h(); ++j)
		for (std::size_t i = 0; i < get_w(); ++i) {
			byte p1 = m_pixels.at(j).at(i);
			int byte_offset = (j * get_w() + i) / 8;
			int bit_offset = 7 - ((j * get_w() + i)) % 8;

			set_bit(result.at(byte_offset), bit_offset, get_bit(p1, 0));
			set_bit(result.at(byte_offset + plane_offset), bit_offset, get_bit(p1, 1));
			set_bit(result.at(byte_offset + 2 * plane_offset), bit_offset, get_bit(p1, 2));
			set_bit(result.at(byte_offset + 3 * plane_offset), bit_offset, get_bit(p1, 3));
		}

	// add unknown byte
	result.insert(end(result), begin(m_unknown_data), end(m_unknown_data));
	return result;
}

void SP_Image::set_bit(byte& b, int e, byte val) const {
	b |= (val << e);
}

bool SP_Image::get_bit(byte b, int e) const {
	return (b >> e) & 1;
}

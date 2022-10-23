#include <iostream>
#include <vector>
#include "./sp_levels/SP_Level.h"
#include "./sp_gfx/SP_Image.h"
#include "./common/klib/klib_file.h"

int main() {
	auto l_file_bytes = klib::file::read_file_as_bytes("./gamedata/GFX.DAT");
	SP_Image gfx = SP_Image(l_file_bytes, 320, 200);

	std::vector<byte> out_file;

	for (int y{ 0 }; y < gfx.get_h(); ++y) {
		for (int x{ 0 }; x < gfx.get_w(); ++x) {
			out_file.push_back(gfx.get_palette_index(x, y) + 'a');
			out_file.push_back(gfx.get_palette_index(x, y) + 'a');
		}
		out_file.push_back(13);
		out_file.push_back(10);
	}

	klib::file::write_bytes_to_file(out_file, "./gamedata/MENU.DAZ");
}
/*
auto l_file_bytes = klib::file::read_file_as_bytes("./gamedata/LEVELS.DAT");
std::vector<SP_Level> levels;

for (auto iter = begin(l_file_bytes); iter != end(l_file_bytes); iter += 1536)
	levels.push_back(SP_Level(std::vector<byte>(iter, iter + 1536)));

std::vector<byte> level_bytes;
for (const auto& level : levels) {
	auto l_lvl_bytes = level.get_bytes();
	level_bytes.insert(end(level_bytes), begin(l_lvl_bytes), end(l_lvl_bytes));
}

klib::file::write_bytes_to_file(level_bytes, "./gamedata/LEVELZ.DAT");
*/

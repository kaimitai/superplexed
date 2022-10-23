#include <iostream>
#include <vector>
#include "./sp_levels/SP_Level.h"
#include "./sp_gfx/SP_Image.h"
#include "./sp_gfx/SP_Palette.h"
#include "./sp_gfx/SP_Gfx.h"
#include "./common/klib/klib_file.h"

using byte = unsigned char;

int main(int argc, char* args[]) {
	auto l_file_bytes = klib::file::read_file_as_bytes("./gamedata/FIXED.DAT");
	auto l_pal_bytes = klib::file::read_file_as_bytes("./gamedata/PALETTES.DAT");

	//l_file_bytes.erase(begin(l_file_bytes));
	int l_pal_no = l_file_bytes.back();

	SP_Image gfx = SP_Image(l_file_bytes, 640, 16);
	SP_Palette palette(std::vector<byte>(begin(l_pal_bytes) + 3 * 16 * 4, begin(l_pal_bytes) + 4 * 16 * 4));

	spgfx::save_bmp(gfx, palette, "out.bmp");

	return 0;
}

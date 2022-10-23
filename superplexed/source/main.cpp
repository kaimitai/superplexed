#include <iostream>
#include <vector>
#include "./sp_gfx/Project_gfx.h"

using byte = unsigned char;

int main(int argc, char* args[]) {
	Project_gfx gfx;
	gfx.save_bmp("FIXED");

	return 0;
}

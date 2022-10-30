#include "Project_gfx.h"
#include "./../common/klib/klib_gfx.h"
#include "./../common/klib/klib_file.h"
#include "./../SP_Constants.h"
#include <filesystem>
#include <tuple>

using byte = unsigned char;

SDL_Texture* Project_gfx::get_tile_texture(std::size_t p_texture_no, std::size_t p_frame_no) const {
	if (m_animations.find(p_texture_no) != end(m_animations))
		return m_tile_textures[m_animations.at(p_texture_no).at(p_frame_no)];
	else
		return m_tile_textures[p_texture_no];
}

SDL_Texture* Project_gfx::get_image_texture(const std::string& p_filename) const {
	auto iter = m_image_textures.find(p_filename);
	if (iter == end(m_image_textures))
		return nullptr;
	else
		return iter->second;
}

Project_gfx::~Project_gfx(void) {
	for (auto texture : m_tile_textures)
		if (texture != nullptr)
			SDL_DestroyTexture(texture);

	for (auto texture : m_font)
		if (texture != nullptr)
			SDL_DestroyTexture(texture);
}

void Project_gfx::blit_font(SDL_Renderer* p_rnd, std::size_t p_char_no, int p_x, int p_y, int p_w, int p_h, SDL_Color p_color) const {
	SDL_Texture* l_letter = m_font.at(p_char_no);
	SDL_SetTextureColorMod(l_letter, p_color.r, p_color.g, p_color.b);
	klib::gfx::blit_scale(p_rnd, l_letter, p_x, p_y, p_w, p_h);
}

void Project_gfx::regenerate_all_textures(SDL_Renderer* p_rnd) {
	for (const auto& kv : m_image_metadata)
		if (m_image_files.find(kv.first) != end(m_image_files)) {
			regenerate_texture(p_rnd, kv.first);
		}
}

void Project_gfx::regenerate_texture(SDL_Renderer* p_rnd, const std::string& p_filename) {
	if (get_image_texture(p_filename) != nullptr) {
		SDL_DestroyTexture(get_image_texture(p_filename));
		m_image_textures.erase(p_filename);
	}

	SDL_Surface* l_srf = sp_image_to_sdl_surface(m_image_files.at(p_filename),
		m_palettes.at(m_image_metadata.at(p_filename).m_palette_no));
	m_image_textures.insert(
		std::make_pair(p_filename, klib::gfx::surface_to_texture(p_rnd, l_srf))
	);
}

bool Project_gfx::load_image_data_from_file(SDL_Renderer* p_rnd, const std::string& p_filename, const SP_Config& p_config) {
	try {
		m_image_files.erase(p_filename);

		m_image_files.insert(std::make_pair(p_filename,
			SP_Image(klib::file::read_file_as_bytes(p_config.get_dat_full_path(p_filename)),
				m_image_metadata.at(p_filename).m_width,
				m_image_metadata.at(p_filename).m_binary)));
		regenerate_texture(p_rnd, p_filename);
	}
	catch (const std::exception&) {
		return false;
	}

	return true;
}

/* will load the tile definitions which will be used when generating tile data.
 supaplex tiles consist of images from both MOVING and FIXED, so we will work as
 if these two images were glue together, with FIXED on top. these pairs will
 define the top left positions of each pixel, as if they were coordinates on this
 imaginary image.
*/
void Project_gfx::generate_tile_definitions(void) {
	m_tile_definitions.clear();

	// first 40 tiles will represent the icons in the program
	for (int i{ 0 }; i < m_image_metadata.at(c::FILENAME_FIXED).m_width; i += c::TILE_W)
		m_tile_definitions.push_back(std::make_pair(i, 0)
		);

	// hard coded animations
	const std::vector<std::pair<int, int>> lc_hc_tiles{
		// player frames #40-45
		{176, 446 + c::TILE_W}, {176 + 16, 446 + c::TILE_W},{176 + 32, 446 + c::TILE_W},{176 + 48, 446 + c::TILE_W},{176 + 64, 446 + c::TILE_W},
		// "bug" frames #46-49
		{304, 64 + c::TILE_W}, { 304, 100 + c::TILE_W }, { 256, 196 + c::TILE_W }, { 272, 196 + c::TILE_W }, { 288, 196 + c::TILE_W }, { 304, 196 + c::TILE_W },
		// lightning frames #51-56
		{16, 405 + c::TILE_W}, {16 + 16, 405 + c::TILE_W}, {16 + 32, 405 + c::TILE_W}, {16 + 48, 405 + c::TILE_W}, {16 + 64, 405 + c::TILE_W}, {16 + 80, 405 + c::TILE_W},
		// scissors frames #57-58
		{32, 424 + c::TILE_W}, {32 + c::TILE_W, 424 + c::TILE_W} //, {32 + 2 * c::TILE_W, 424 + c::TILE_W}, {32 + 3 * c::TILE_W, 424 + c::TILE_W}, {32 + 4 * c::TILE_W, 424 + c::TILE_W}, {32 + 5 * c::TILE_W, 424 + c::TILE_W}
	};

	m_tile_definitions.insert(end(m_tile_definitions),
		begin(lc_hc_tiles), end(lc_hc_tiles));

	// set up animations
	m_animations.clear();
	m_animations[25] = { 49, 50, 46, 47, 48, 2 }; // "bug" enemy
	m_animations[24] = { 51, 52, 53, 54, 55, 56 }; // "lightning" enemy
	m_animations[17] = { 57, 58, 57, 58, 57, 58 }; // "scissors" enemy
	m_animations[40] = { 3, 40, 41, 42, 43, 44 }; // player
}

Project_gfx::Project_gfx(SDL_Renderer* p_rnd, const SP_Config& p_config) {
	// initialize image metadata
	m_image_metadata.insert(std::make_pair(c::FILENAME_BACK, Gfx_metadata(320, 200, 0)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_CONTROLS, Gfx_metadata(320, 200, 1, true)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_CHARS6, Gfx_metadata(512, 8, 4, false, true)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_CHARS8, Gfx_metadata(512, 8, 4, false, true)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_FIXED, Gfx_metadata(640, 16, 1)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_GFX, Gfx_metadata(320, 200, 1, true)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_MENU, Gfx_metadata(320, 200, 1)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_MOVING, Gfx_metadata(320, 462, 1)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_PANEL, Gfx_metadata(320, 24, 1)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_TITLE, Gfx_metadata(320, 200, 5, true)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_TITLE1, Gfx_metadata(320, 200, 6)));
	m_image_metadata.insert(std::make_pair(c::FILENAME_TITLE2, Gfx_metadata(320, 200, 7)));

	load_palettes(p_rnd, p_config);

	// read required image data for the program
	generate_tile_definitions();
	load_image_data_from_file(p_rnd, c::FILENAME_FIXED, p_config);
	load_image_data_from_file(p_rnd, c::FILENAME_MOVING, p_config);
	load_image_data_from_file(p_rnd, c::FILENAME_CHARS8, p_config);

	// create textures used by the editor
	std::vector<SDL_Rect> l_font_rect{ {311,0,8,8},{463,0,8,8},{295,0,8,8},{79,0,8,8} };

	m_font = klib::gfx::split_surface_specified(p_rnd,
		sp_image_to_sdl_surface(m_image_files.at(c::FILENAME_CHARS8),
			m_palettes.at(m_image_metadata.at(c::FILENAME_CHARS8).m_palette_no)),
		l_font_rect, true, true,
		sp_color_to_sdl(m_palettes.at(m_image_metadata.at(c::FILENAME_CHARS8).m_palette_no).get_color(0)));

	for (int i{ 0 }; i < static_cast<int>(m_tile_definitions.size()); ++i)
		m_tile_textures.push_back(create_tile_texture(p_rnd, i));
}

void Project_gfx::save_dat(const std::string& p_filename, SP_Config& p_config) const {
	klib::file::write_bytes_to_file(m_image_files.at(p_filename).to_bytes(),
		p_config.get_dat_full_path(p_filename));
}

SDL_Color Project_gfx::sp_color_to_sdl(const std::tuple<byte, byte, byte, byte>& p_col) const {
	return SDL_Color{
		std::get<0>(p_col),
		std::get<1>(p_col),
		std::get<2>(p_col),
		std::get<3>(p_col)
	};
}

SDL_Surface* Project_gfx::sp_image_to_sdl_surface(const SP_Image& p_image, const SP_Palette& p_palette, int p_transp_idx) const {
	SDL_Surface* l_bmp = SDL_CreateRGBSurface(0, p_image.get_w(),
		p_image.get_h(), 8, 0, 0, 0, 0);

	SDL_Color out_palette[256] = { 0, 0, 0 };
	for (int i{ 0 }; i < static_cast<int>(p_palette.get_size()); ++i)
		out_palette[i] = sp_color_to_sdl(p_palette.get_color(i));

	SDL_SetPaletteColors(l_bmp->format->palette, out_palette, 0, 256);

	for (int j{ 0 }; j < p_image.get_h(); ++j)
		for (int i{ 0 }; i < p_image.get_w(); ++i)
			klib::gfx::put_pixel(l_bmp, i, j, p_image.get_palette_index(i, j));

	if (p_transp_idx >= 0) {
		SDL_Color l_trans_rgb = sp_color_to_sdl(p_palette.get_color(p_transp_idx));
		SDL_SetColorKey(l_bmp, true, SDL_MapRGB(l_bmp->format, l_trans_rgb.r, l_trans_rgb.g, l_trans_rgb.b));
	}

	return l_bmp;
}

// image metadata
Project_gfx::Gfx_metadata::Gfx_metadata(int p_w, int p_h, int p_pal_no, bool p_extra, bool p_binary)
	: m_width{ p_w }, m_height{ p_h }, m_palette_no{ p_pal_no },
	m_extra{ p_extra }, m_binary{ p_binary }
{ }

std::pair<int, int> Project_gfx::get_image_dimensions(const std::string& p_filename) const {
	return std::make_pair(m_image_files.at(p_filename).get_w(),
		m_image_files.at(p_filename).get_h());
}

const std::map<std::string, Project_gfx::Gfx_metadata>& Project_gfx::get_meta(void) const {
	return m_image_metadata;
}

void Project_gfx::load_fixed_palettes(void) {
	// ***** add hardcoded palettes that do not come from a file *****
// palette for CHARS6 and CHARS8 (monochrome)
	m_palettes.push_back(SP_Palette({
		0xf,0x7,0xb,0x0, 0xf,0xf,0xf,0xf
		}));
	// palette for TITLE
	m_palettes.push_back(SP_Palette({
	0x02, 0x03, 0x05, 0x00, 0x0D, 0x0A, 0x04, 0x0C, 0x02, 0x06, 0x06, 0x02, 0x03, 0x09, 0x09, 0x03,
	0x0B, 0x08, 0x03, 0x06, 0x02, 0x07, 0x07, 0x0A, 0x08, 0x06, 0x0D, 0x09, 0x06, 0x04, 0x0B, 0x01,
	0x09, 0x01, 0x00, 0x04, 0x0B, 0x01, 0x00, 0x04, 0x0D, 0x01, 0x00, 0x0C, 0x0F, 0x01, 0x00, 0x0C,
	0x0F, 0x06, 0x04, 0x0C, 0x02, 0x05, 0x06, 0x08, 0x0F, 0x0C, 0x06, 0x0E, 0x0C, 0x0C, 0x0D, 0x0E
		}));
	// palette for TITLE1
	m_palettes.push_back(SP_Palette({
	0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x08, 0x08, 0x08, 0x08, 0x0A, 0x0A, 0x0A, 0x07,
	0x0A, 0x0A, 0x0A, 0x07, 0x0B, 0x0B, 0x0B, 0x07, 0x0E, 0x01, 0x01, 0x04, 0x09, 0x09, 0x09, 0x07,
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x00, 0x00, 0x04, 0x0B, 0x00, 0x00, 0x0C,
	0x08, 0x08, 0x08, 0x08, 0x05, 0x05, 0x05, 0x08, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x08,
		}));
	// palette for TITLE2
	m_palettes.push_back(SP_Palette({
		0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x06, 0x06, 0x06, 0x08, 0x0A, 0x0A, 0x0A, 0x07,
		0x0A, 0x0A, 0x0A, 0x07, 0x0B, 0x0B, 0x0B, 0x07, 0x0E, 0x01, 0x01, 0x04, 0x09, 0x09, 0x09, 0x07,
		0x01, 0x03, 0x07, 0x00, 0x08, 0x08, 0x08, 0x08, 0x09, 0x00, 0x00, 0x04, 0x0B, 0x00, 0x00, 0x0C,
		0x00, 0x02, 0x0A, 0x01, 0x05, 0x05, 0x05, 0x08, 0x06, 0x06, 0x06, 0x08, 0x08, 0x08, 0x08, 0x07,
		}));
}

void Project_gfx::load_palettes(SDL_Renderer* p_rnd, const SP_Config& p_config) {
	// ***** read palette data *****
// palette 0: bluescale (BLUE.DAT)
// palette 1: game objects and screens (apart from the title-screens and fonts)
// palette 2: ???
// palette 3: ???
	std::vector<byte> l_bytes = klib::file::read_file_as_bytes(p_config.get_dat_full_path(c::FILENAME_PALETTES));
	for (std::size_t i{ 0 }; i < 4; ++i)
		m_palettes.push_back(SP_Palette(std::vector<byte>(begin(l_bytes) + i * 4 * 16,
			begin(l_bytes) + (i + 1) * 4 * 16)));

	load_fixed_palettes();
	regenerate_all_textures(p_rnd);
}

const std::vector<SP_Palette>& Project_gfx::get_palettes(void) const {
	return m_palettes;
}

void Project_gfx::set_palettes(SDL_Renderer* p_rnd,
	const std::vector<SP_Palette>& p_palettes) {
	for (std::size_t i{ 0 }; i < c::PALETTE_COUNT; ++i)
		m_palettes.at(i) = p_palettes.at(i);

	regenerate_all_textures(p_rnd);
}

void Project_gfx::save_palettes_dat(const SP_Config& p_config) {
	std::vector<byte> l_out_bytes;
	for (std::size_t i{ 0 }; i < c::PALETTE_COUNT; ++i) {
		auto l_pal_bytes = m_palettes[i].to_bytes();
		l_out_bytes.insert(end(l_out_bytes), begin(l_pal_bytes), end(l_pal_bytes));
	}

	klib::file::write_bytes_to_file(l_out_bytes,
		p_config.get_dat_full_path(c::FILENAME_PALETTES));
}

#include "klib_file.h"

#include <fstream>

using byte = unsigned char;

std::vector<byte> klib::file::read_file_as_bytes(const std::string& p_file_name) {
	std::vector<byte> result;

	std::ifstream input(p_file_name, std::ios::binary | std::ios::in);

	if (!input.fail())
		return std::vector<byte>((std::istreambuf_iterator<char>(input)), (std::istreambuf_iterator<char>()));
	else
		throw std::exception(("Could not open " + p_file_name).c_str());

	return result;
}

void klib::file::write_bytes_to_file(const std::vector<byte>& inp, const std::string& file_name) {
	std::ofstream output(file_name, std::ios::binary | std::ios::out);

	if (!output.fail())
		output.write((const char*)(&inp[0]), inp.size());
	else
		throw std::exception("Could not write to file");

	output.close();
}

void klib::file::append_string_to_file(const std::string& p_text, const std::string& p_file_name) {
	std::ofstream output(p_file_name, std::ios::binary | std::ofstream::app);

	if (!output.fail())
		output << p_text;


	output.close();
}

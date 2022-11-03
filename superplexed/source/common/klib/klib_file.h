#ifndef KLIB_FILE_H
#define KLIB_FILE_H

#include <string>
#include <vector>

namespace klib {
	namespace file {

		using byte = unsigned char;

		std::vector<byte> read_file_as_bytes(const std::string& p_file_name);
		void write_bytes_to_file(const std::vector<byte>&, const std::string&);
		void append_string_to_file(const std::string& p_text, const std::string& p_file_name);

	}
}

#endif

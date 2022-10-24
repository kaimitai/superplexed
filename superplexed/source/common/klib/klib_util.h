#ifndef KLIB_UTIL_H
#define KLIB_UTIL_H

#include <string>
#include <vector>

namespace klib {

	namespace util {

		template <class T>
		std::vector<T> string_split(const std::string& p_values, char p_delimeter) {
			std::vector<T> result;

			std::size_t l_last = 0;
			std::size_t l_next = p_values.find(p_delimeter);

			while (l_next != std::string::npos) {
				auto l_next_num = p_values.substr(l_last, l_next - l_last);
				result.push_back(static_cast<T>(atoi(l_next_num.c_str())));

				l_last = l_next + 1;
				l_next = p_values.find(p_delimeter, l_last);
			}
			auto l_next_num = p_values.substr(l_last, l_next - l_last);
			if (!l_next_num.empty())
				result.push_back(static_cast<T>(atoi(l_next_num.c_str())));

			return result;
		}

		template <class T>
		std::string string_join(const std::vector<T>& p_values, char p_delimiter) {
			std::string result;

			for (std::size_t i{ 0 }; i < p_values.size(); ++i)
				result += std::to_string(p_values[i]) + p_delimiter;

			result.pop_back();

			return result;
		}

	}

}

#endif

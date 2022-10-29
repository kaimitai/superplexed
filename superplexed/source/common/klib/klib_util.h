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

			if (!result.empty())
				result.pop_back();

			return result;
		}

		template <class T>
		void rot_sq_matrix_ccw(std::vector<std::vector<T>>& p_input) {

			auto cyclic_roll = [](T& a, T& b, T& c, T& d) {
				T temp = a;
				a = b;
				b = c;
				c = d;
				d = temp;
			};

			int n{ static_cast<int>(p_input.size()) };

			for (int i{ 0 }; i < n / 2; i++)
				for (int j{ 0 }; j < (n + 1) / 2; j++)
					cyclic_roll(p_input[i][j], p_input[n - 1 - j][i], p_input[n - 1 - i][n - 1 - j], p_input[j][n - 1 - i]);
		}

		template <class T>
		T validate(T p_input, T p_min_val, T p_max_val) {
			if (p_input < p_min_val)
				return p_min_val;
			else if (p_input > p_max_val)
				return p_max_val;
			else
				return p_input;
		}

		template<class T>
		std::string stringnum(T p_num, std::size_t p_length = 3) {
			std::string result{ std::to_string(p_num) };

			while (result.size() < p_length)
				result.insert(begin(result), '0');

			return result;
		}

	}

}

#endif

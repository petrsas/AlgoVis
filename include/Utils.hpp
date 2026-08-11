#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <charconv>
#include <concepts>

class Utils {
public:
    static std::vector<std::string> split_string(const std::string& str, const std::string& delim);
    static std::size_t convert_to_idx(const std::string& str);
    static std::vector<std::string> load_file_lines(const std::string& fp);
    static void store_files_lines(const std::vector<std::string>& lines, const std::string& fp);

    template<typename T>
    requires std::integral<T>
    static std::vector<T> convert_to_integrals(const std::vector<std::string>& str_vec) {
        std::vector<T> vec;
        T n{};

        for (const std::string& w : str_vec) {
            auto [ptr, ec] = std::from_chars(w.data(), w.data() + w.size(), n);
            if (ec == std::errc()) {
                vec.emplace_back(n);
            }
        }
        return vec;
    }

    template<typename T, typename Alloc>
    static void print_vec(const std::vector<T, Alloc>& vec) {
        std::cout << "Printing vec:\n";
        for (const auto& el : vec) {
            std::cout << el << "\n";
        }
    }
};

#include "Utils.h"

#include <ranges>
#include <fstream>
#include <charconv>

std::vector<std::string> Utils::split_string(const std::string& str, const std::string& delim) {
    auto res = str
        | std::views::split(delim)
        | std::ranges::to<std::vector<std::string>>();
    return res;
}

std::size_t Utils::convert_to_idx(const std::string& str) {
    int n{};
    auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), n);
    if (ec == std::errc() && n >= 0) {
        return static_cast<std::size_t>(n);
    }
    return std::numeric_limits<std::size_t>::max(); //sentinel value 
}

std::vector<std::string> Utils::load_file_lines(const std::string& fp) {
    std::ifstream f(fp);
    if (!f) {
        std::cerr << "File failed to load: " << fp << "\n";
        return {};
    }
    std::vector<std::string> lines;
    std::string ln;
    while (std::getline(f, ln)) {
        lines.emplace_back(ln);
    }
    return lines;
}

void Utils::store_files_lines(const std::vector<std::string>& lines, const std::string& fp) {
    std::ofstream f(fp);
    if (!f) {
        std::cerr << "File failed to open or be created: " << fp << "\n";
        return;
    }

    for (const auto& line : lines) {
        f << line;
    }
}

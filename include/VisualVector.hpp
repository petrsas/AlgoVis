#pragma once
#include "Utils.h"
#include <vector>
#include <string>
#include <format>
#include <concepts>
#include <cstddef>
#include <utility>

#include <SFML/Graphics/RenderTarget.hpp>

template <typename T, typename Alloc = std::allocator<T>>
requires std::integral<T>
class VisualVector {
public:
    std::vector<T, Alloc> vec;
    sf::RenderTarget& target;
    std::vector<std::string> instrs;

public:
    // Constructor
    VisualVector(sf::RenderTarget& target, const std::vector<T, Alloc>& vec)
        : vec(vec), target(target) {

        std::string vec_str;
        for (std::size_t i = 0; i < this->vec.size(); ++i) {
            vec_str += std::format("{};", this->vec.at(i));
        }
        vec_str += "\n";

        this->instrs = {vec_str};
    }

    const std::vector<T, Alloc> GetDataVec() const {
        return this->vec;
    }

    const std::vector<std::string>& GetInstructions() const {
        return this->instrs;
    }

    std::size_t Size() const {
        return this->vec.size();
    }

    bool Swap(std::size_t idx_a, std::size_t idx_b) {
        if (idx_a >= this->vec.size() || idx_b >= this->vec.size()) {
            return false;
        }

        std::swap(this->vec.at(idx_a), this->vec.at(idx_b));

        this->instrs.emplace_back(std::format("S {} {}\n", idx_a, idx_b));
        return true;
    }

    // 0 > if a > b, 0 < if a < b, 0 if a == b
    int Cmp(std::size_t idx_a, std::size_t idx_b) {
        this->instrs.emplace_back(std::format("C {} {}\n", idx_a, idx_b));

        // unsigned underflow protection
        T a = this->vec.at(idx_a);
        T b = this->vec.at(idx_b);
        return (a > b) - (a < b);
    }

    void PrintInstructions() const {
        Utils::print_vec(this->instrs);
    }

    void LoadInstructions(const std::string& fp) {
        this->instrs = Utils::load_file_lines(fp);
    }

    void StoreInstructions(const std::string& fp) const {
        Utils::store_files_lines(this->instrs, fp);
    }
};

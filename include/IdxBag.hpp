#pragma once

#include <array>
#include <cstddef>

class IdxBag {
private:
    std::array<std::size_t, 10> arr{};
    std::size_t current_idx = 0;

public:
    void AddIdx(std::size_t idx);
    void Clear();
    
    std::size_t Size() const;
    std::size_t At(std::size_t idx) const;
};

#include "IdxBag.h"

void IdxBag::AddIdx(std::size_t idx) {
    if (current_idx < arr.size()) {
        arr[current_idx] = idx;
        current_idx++;
    }
}

void IdxBag::Clear() {
    current_idx = 0;
}

std::size_t IdxBag::Size() const {
    return current_idx;
}

std::size_t IdxBag::At(std::size_t idx) const {
    if (idx >= current_idx) {
        return 0; //sentinel on out of bounds
    }
    return arr[idx];
}

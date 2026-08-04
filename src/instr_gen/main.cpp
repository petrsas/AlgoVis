
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Window.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <cstddef>
#include <exception>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <format>
#include <ranges>


class VisualVector {
    std::vector<int> vec;
    //instructions
    std::vector<std::string> instrs;

public:
    VisualVector(std::vector<int>& vec)
        : vec(vec){
        std::string vec_str;

        for (size_t i=0; i<this->vec.size(); i++) {
            vec_str += std::format("{};", this->vec.at(i));
        }
        vec_str += "\n";

        std::cout<<"Const"<<std::endl;
        this->instrs = {vec_str};
    }
    size_t Size() {
        return this->vec.size();
    }

    bool Swap(size_t idx_a, size_t idx_b) {
        if (idx_a >= this->vec.size() || idx_b >= this->vec.size())
            return false;

        int tmp = this->vec.at(idx_a);
        this->vec.at(idx_a) = this->vec.at(idx_b);
        this->vec.at(idx_b) = tmp;

        this->instrs.emplace_back(std::format("S {} {}\n", idx_a, idx_b));
        return true;
    }
    //should highlight the looked at element
    int At(size_t idx) {
        this->instrs.emplace_back(std::format("H {}\n", idx));
        return this->vec.at(idx);
    }
    void PrintInstructions() {
        for (size_t i=0; i<this->instrs.size(); i++){
            std::cout<<instrs.at(i);
        }
    }
    void StoreInstructions(const std::string& fp) {
        std::ofstream f(fp);

        if (!f) {
            std::cerr<<"File failed to open or be created.\n";
            return;
        }

        for (size_t i=0; i<this->instrs.size(); i++) {
            f << this->instrs.at(i);
        }
    }
    void LoadInstructions(const std::string& fp) {
        std::ifstream f(fp);
        if (!f) {
            std::cerr<<"File failed to load.\n";
            return;
        }

        std::string ln;
        while(std::getline(f, ln)) {
            this->instrs.emplace_back(ln);
        }
    } 

};

int main() {
std::cout<<"Hello there"<<std::endl;
return 0;
}

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


#define WIDTH 1280
#define HEIGHT 720
#define MAIN_COLOR sf::Color::Magenta
#define HIGHLIGHT_COLOR sf::Color::Red
//#define OUTLINE_COLOR
class VisualVector {
    std::vector<int> vec;
    sf::RenderTarget& trg;
    //instructions
    std::vector<std::string> instrs;

    //bar chart info
    float start_x, end_x, start_y, end_y;

public:
    VisualVector(sf::RenderTarget& target, std::vector<int>& vec)
        : vec(vec), trg(target) {
        std::string vec_str;

        for (size_t i=0; i<this->vec.size(); i++) {
            vec_str += std::format("{};", this->vec.at(i));
        }
        vec_str += "\n";

        std::cout<<"Const"<<std::endl;
        this->instrs = {vec_str};

    }
    std::vector<int> GetVector() const{
        return this->vec;
    }
    std::vector<std::string> GetInstructions() const {
        return this->instrs;
    }
    size_t Size() const{
        return this->vec.size();
    }
    //should swap elems
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

//for Bar Chart
class Bar {
    float x, y, w, h;
    sf::RectangleShape shape;
    sf::RenderTarget& target;
    bool IsHigh;
public:
    Bar(sf::RenderTarget& target, float x, float y, float w, float h)
        :target(target),x(x), y(y), w(w), h(h) {
        shape.setPosition({x,y});
        shape.setSize({w,h});
        shape.setFillColor(MAIN_COLOR);
    }
    void Draw() const{
        this->target.draw(this->shape);
    }
    void Highlight(bool highlight) {
        IsHigh = highlight;
        if (IsHigh) {
            shape.setFillColor(HIGHLIGHT_COLOR);
        }
    }
};

class BarChart {
    std::vector<Bar> bars;
    sf::RenderTarget& target;

    std::vector<std::string> instructions;
    size_t instr_ptr;

    std::vector<size_t> highIdxs;

    float map_height_val(float v, float vec_min, float vec_max) {
        float min_h = 20;
        float max_h = HEIGHT - 40;
        if (vec_min == vec_max) {
            return min_h;
        }
        float norm = (v - vec_min) / (vec_max - vec_min);
        return min_h + norm * (max_h - min_h);
    }

public:
    BarChart(sf::RenderTarget& target, const VisualVector& vvec)
        : target(target), instructions(vvec.GetInstructions()){
        auto vec = vvec.GetVector();

        float bar_space = float(WIDTH) / (vvec.Size() + 2);
        float x = bar_space;
        float y = 20;
        float bar_width = bar_space * 0.8;
        //float bar_break = bar_space * 0.2;

        float vec_max = float(*std::max_element(vec.begin(), vec.end()));
        float vec_min = float(*std::min_element(vec.begin(), vec.end()));

        for (size_t i = 0; i < vec.size(); i++){
            float bar_height = map_height_val(vec.at(i), vec_min, vec_max); 
            bars.emplace_back(Bar(target, x, y, bar_width, bar_height));
            x += bar_space;
        }
        highIdxs.reserve(5);
    }
    BarChart(sf::RenderTarget& target, std::vector<std::string> instructions)
    : target(target), instructions(instructions) {
        std::string vec_str = instructions.at(0);
        vec = 
    }

    void Draw() const{
        for (const Bar& b : this->bars) {
            b.Draw();
        }
    }


};

void printVec(const std::vector<int>& vec) {
    std::cout<<"Printing vec:"<<std::endl;
    for (size_t i=0; i<vec.size(); i++){
        std::cout<<vec.at(i)<<"\t";
    }
    std::cout<<"\n";
}

void bubbleSort(VisualVector& vec) {
    for (size_t i=0;i<vec.Size();i++){
        for (size_t j=i;j<vec.Size();j++){
            if (vec.At(i) > vec.At(j)) {
                vec.Swap(i, j);
            }
        }
    }
}

int main() {
    std::vector<int> vec = {25,10,2,3,15,65,12};
//    printVec(vec);
//    printVec(vec);

    // SFML 3.1 uses initializer lists for VideoMode dimensions
    auto window = sf::RenderWindow(sf::VideoMode({WIDTH, HEIGHT}), "Algorithm Visualizer by Petr Chyla");
    window.setFramerateLimit(60);


    VisualVector vvec = VisualVector(window, vec);
    bubbleSort(vvec);
    vvec.PrintInstructions();
    vvec.StoreInstructions("instructions.txt");

    BarChart bchart = BarChart(window, vvec);

    while (window.isOpen()) {
        // SFML 3 pollEvent uses optional return values or handle-based loops
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        bchart.Draw();
        window.display();
    }
    return 0;
}

//The actual swapping, splitting into threads how???
//one reading thread that freezes periodically and one rendering (main) thread that handles inputs and rendering at 60 fps, use mutexes to look the shared struct

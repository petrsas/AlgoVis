#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Window.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <chrono>
#include <thread>
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

constexpr std::chrono::duration<double> HOLD_TIME{0.5};

class Utils {
public:
    static std::vector<std::string> split_string (const std::string& str, const std::string& delim) {
        auto res = str
            | std::views::split(delim)
            | std::ranges::to<std::vector<std::string>>();
        return res;
    }

    static size_t convert_to_idx (std::string str) {
        int n{};
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), n);
        if (ec == std::errc()) {
            return n;
        }
        return -1; 
    }

    static std::vector<int> convert_to_ints (const std::vector<std::string>& str_vec) {
        std::vector<int> vec;
        int n{};

        for (const std::string& w : str_vec) {
            //compiler structure unpacking
            auto [ptr, ec] = std::from_chars(w.data(), w.data() + w.size(), n);
            if (ec == std::errc()) {
                vec.emplace_back(n);
            }
        }
        return vec;
    }


    template<typename T>
    static void print_vec(const std::vector<T>& vec) {
        std::cout<<"Printing vec:"<<std::endl;
        for (auto el : vec) {
            std::cout<<el<<"\n";
        }
    }

    static std::vector<std::string> load_file_lines(const std::string& fp) {
        std::ifstream f(fp);
        if (!f) {
            std::cerr<<"File failed to load.\n";
            return std::vector<std::string>();
        }
        std::vector<std::string> lines;
        std::string ln;
        while(std::getline(f, ln)) {
            lines.emplace_back(ln);
        }
        return lines;
    } 
    static void store_files_lines(const std::vector<std::string>& lines, const std::string& fp) {
        std::ofstream f(fp);
        if (!f) {
            std::cerr<<"File failed to open or be created.\n";
            return;
        }

        for (size_t i=0; i<lines.size(); i++) {
            f << lines.at(i);
        }
    }
};

class VisualVector {
    std::vector<int> vec;
    sf::RenderTarget& target;

    //instructions
    std::vector<std::string> instrs;

    //bar chart info
    float start_x, end_x, start_y, end_y;

public:
    VisualVector(sf::RenderTarget& target, std::vector<int>& vec)
        : vec(vec), target(target) {
        std::string vec_str;

        for (size_t i=0; i<this->vec.size(); i++) {
            vec_str += std::format("{};", this->vec.at(i));
        }
        vec_str += "\n";

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
        Utils::print_vec(this->instrs);
    }
    void LoadInstructions(const std::string& fp) {
        this->instrs = Utils::load_file_lines(fp);
    }
    void StoreInstructions(const std::string& fp) {
        Utils::store_files_lines(this->instrs, fp);
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
    Bar& operator=(const Bar& other) {
        if (this != &other) {
            this->x = other.x;
            this->y = other.y;
            this->shape.setPosition({this->x, this->y});
        }
        return *this;
    }
    void Draw() const{
        this->target.draw(this->shape);
    }
    void Highlight(bool highlight) {
        IsHigh = highlight;
        if (IsHigh) {
            shape.setFillColor(HIGHLIGHT_COLOR);
        }
        else {
            shape.setFillColor(MAIN_COLOR);
        }
    }
};

class BarChart {
    std::vector<Bar> bars;
    sf::RenderTarget& target;

    std::vector<std::string> instructions;
    size_t instr_ptr = 0;

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
    void generate_bars(std::vector<int> val_vec) 
    {
        float bar_space = float(WIDTH) / (val_vec.size() + 2);
        float x = bar_space;
        float y = 20;
        float bar_width = bar_space * 0.8;
        //float bar_break = bar_space * 0.2;

        float val_vec_max = float(*std::max_element(val_vec.begin(), val_vec.end()));
        float val_vec_min = float(*std::min_element(val_vec.begin(), val_vec.end()));

        for (size_t i = 0; i < val_vec.size(); i++){
            float bar_height = map_height_val(val_vec.at(i), val_vec_min, val_vec_max); 
            bars.emplace_back(Bar(target, x, y, bar_width, bar_height));
            x += bar_space;
        }
        highIdxs.reserve(5);
    }

    //H idx
    //S idx_a idx_b
    void execute_instruction() {
        std::vector<std::string> cmd = Utils::split_string(this->instructions.at(this->instr_ptr), " ");
        //clean previous highlights
        for (size_t i=0; i<this->highIdxs.size(); i++) {
            bars.at(highIdxs.at(i)).Highlight(false);
        }

        if (cmd.at(0) == "H") {
            size_t idx = Utils::convert_to_idx(cmd.at(1));
            if (idx < 0) { //invalid instruction
                return;
            }
            bars.at(idx).Highlight(true);
            highIdxs.emplace_back(idx);
            std::cout<<this->instructions.at(this->instr_ptr);
        }
        else if (cmd.at(0) == "S") {
            size_t idx_a = Utils::convert_to_idx(cmd.at(1));
            bars.at(idx_a).Highlight(true);
            highIdxs.emplace_back(idx_a);

            size_t idx_b = Utils::convert_to_idx(cmd.at(2));
            bars.at(idx_b).Highlight(true);
            highIdxs.emplace_back(idx_b);

            Bar tmp = bars.at(idx_a);
            bars.at(idx_a) = bars.at(idx_b);
            bars.at(idx_b) = tmp;
            std::cout<<this->instructions.at(this->instr_ptr);
        }

    }

public:
    BarChart(sf::RenderTarget& target, std::vector<std::string> instructions)
    : target(target), instructions(instructions) {
        std::string vec_str = this->instructions.at(0);
        auto val_vec = Utils::convert_to_ints(Utils::split_string(vec_str, ";"));
        generate_bars(val_vec);
    }
    BarChart(sf::RenderTarget& target, const VisualVector& vvec)
        : target(target), instructions(vvec.GetInstructions()){
        std::string vec_str = this->instructions.at(0);
        auto val_vec = Utils::convert_to_ints(Utils::split_string(vec_str, ";"));
        generate_bars(val_vec);
    }
    BarChart(sf::RenderTarget& target, const std::string& file_path)
        : target(target) {
        this->instructions = Utils::load_file_lines(file_path);
        std::string vec_str = this->instructions.at(0);
        auto val_vec = Utils::convert_to_ints(Utils::split_string(vec_str, ";"));
        generate_bars(val_vec);
    }
    void Move(int direction) {
        if ((this->instr_ptr + direction) >= instructions.size() || (this->instr_ptr + direction) < 0) return; //out of bounds

        if (direction > 0) {
            for (size_t i=0; i<=direction; i++) {
                this->instr_ptr++;
                execute_instruction();
                std::this_thread::sleep_for(HOLD_TIME);
            }
            return;
        }
        if (direction < 0) {
            for (size_t i=0; i<=abs(direction); i++) {
                this->instr_ptr--;
                execute_instruction();
                std::this_thread::sleep_for(HOLD_TIME);
            }
            return;
        }

    }
    void Draw() {
        for (const Bar& b : this->bars) {
            b.Draw();
        }
        Move(1);
    }


};

void bubbleSortOptimized(VisualVector& vec) {
    size_t n = vec.Size();
    if (n == 0) return;

    for (size_t i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (size_t j = 0; j < n - i - 1; ++j) {
            if (vec.At(j) > vec.At(j + 1)) {
                vec.Swap(j, j + 1);
                swapped = true;
            }
        }
        if (!swapped) break; // Array is sorted early
    }
}

int main() {
    std::vector<int> vec = {25,10,2,3,15,65,12};

    // SFML 3.1 uses initializer lists for VideoMode dimensions
    auto window = sf::RenderWindow(sf::VideoMode({WIDTH, HEIGHT}), "Algorithm Visualizer by Petr Chyla");
    window.setFramerateLimit(60);

    VisualVector vvec = VisualVector(window, vec);
    bubbleSortOptimized(vvec);
    Utils::print_vec(vvec.GetVector());
    vvec.PrintInstructions();
    vvec.StoreInstructions("instructions.txt");

    std::cout<<"*************************************************"<<std::endl;

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

//The BarChart should only work with instructions, do not touch the vis vec vec itself (cause its already sorted)
//give instr gen a pass over I guess
//Only thing moving the instr_ptr should be the Move

//The actual swapping, splitting into threads how???
//one reading thread that freezes periodically and one rendering (main) thread that handles inputs and rendering at 60 fps, use mutexes to look the shared struct

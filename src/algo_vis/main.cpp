#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderTarget.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <format>
#include <ranges>
#include <mutex>
#include <concepts>

#include "Utils.hpp"
#include "LoggingAllocator.hpp"
#include "ThreadSafeLogger.hpp"


#define WIDTH 1280
#define HEIGHT 720
#define MAIN_COLOR sf::Color::Blue
#define HIGHLIGHT_COLOR sf::Color::Red
//#define OUTLINE_COLOR

constexpr std::chrono::duration<double> HOLD_TIME{0.5};
/*
class ThreadSafeLogger {
//most likely
    static inline std::vector<std::string> logs;
    static inline std::mutex mutex;
public:
    static void log_alloc(const char* name, const size_t count, const size_t size) {
        std::lock_guard<std::mutex> lock(mutex); //auto unlock when out of scope
        logs.emplace_back(std::format("Allocated {} of {} requiring {} bytes\n", count, name, size));
    }
    static void print_log() {
        for (const std::string& l : logs) {
            std::cout<<l;
        }
    }
};
*/

//noexcept gurantees that no exceptions will be thrown by that method
//gurantee can be broken, it which case the program will just end immidiately without handling the exception
// = default means that compiler will generate the implementation of the constructor, ie. sets members with their default initializers or leaves unit primitives
//this simple stateless allocator does not throw exceptions
//for constr because then std containers can just swap pointers instead of creating full copies during moving
//for dealloc because std containers destructors are noexcept as cleanup ops are simply not allowed to fail ...
//std::terminate is called on dual exceptions or on exception thrown in noexcept function, which terminates the program at once to prevent propagation of assumed corrupted internal state (like writing corrupted data on disk)
/*
template <typename T>
class LoggingAllocator {
public:
    using value_type = T;

    LoggingAllocator() noexcept = default;

    template <typename U>
    LoggingAllocator(const LoggingAllocator<U>&) noexcept {}

    T* allocate(size_t n) {
        ThreadSafeLogger::log_alloc(typeid(T).name(), n, n * sizeof(T));
        //creates instace of std::allocator on stack
        //allocates n elems of type T on heap via the allocate(n) method
        //returns a pointer to that mem
        return std::allocator<T>{}.allocate(n); 
    }

    void deallocate(T* p, std::size_t n) noexcept {
        //creates an instance of std::allocator on stack
        //dealloc n elems of type T from heap starting at p
        std::allocator<T>{}.deallocate(p, n);
    }

    //hidden friend, typename T is already known
    //also the compiler does not have to consider this on every == operation
    //lack of named params, since they are not used inside the function
    //params are there only to satisfy the compiler which reqs a specific sig
    template <typename U>
    friend bool operator==(const LoggingAllocator&, const LoggingAllocator<U>&) { return true; }
    template < typename U>
    friend bool operator!=(const LoggingAllocator&, const LoggingAllocator<U>&) { return false; }
};
*/
/*
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

    template<typename T>
    requires std::integral<T>
    static std::vector<T> convert_to_integrals (const std::vector<std::string>& str_vec) {
        std::vector<T> vec;
        T n{};

        for (const std::string& w : str_vec) {
            //compiler structure unpacking
            auto [ptr, ec] = std::from_chars(w.data(), w.data() + w.size(), n);
            if (ec == std::errc()) {
                vec.emplace_back(n);
            }
        }
        return vec;
    }

    template<typename T, typename Alloc>
    static void print_vec(const std::vector<T, Alloc>& vec) {
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
*/
/*
template <typename T, typename Alloc>
requires std::integral<T>
class VisualVector {
public:
    std::vector<T, Alloc> vec;
    sf::RenderTarget& target;
    std::vector<std::string> instrs;

public:
    VisualVector(sf::RenderTarget& target, const std::vector<T, Alloc>& vec)
        : vec(vec), target(target) {
        std::string vec_str;

        for (size_t i=0; i<this->vec.size(); i++) {
            vec_str += std::format("{};", this->vec.at(i));
        }
        vec_str += "\n";

        this->instrs = {vec_str};

    }
    std::vector<T, Alloc> GetDataVec () const {
        return this->vec;
    }
    std::vector<std::string> GetInstructions() const {
        return this->instrs;
    }
    size_t Size() const{
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
    // 0 > if a > b, 0 < if a < b, 0 if a == b
    int Cmp (size_t idx_a, size_t idx_b) {
        this->instrs.emplace_back(std::format("C {} {}\n", idx_a, idx_b));
        return vec.at(idx_a) - vec.at(idx_b);
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
*/
/*
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
            this->h = other.h;
            this->w = other.w;
            this->shape.setSize({this->w, this->h});
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
*/
/*
class IdxBag {
private:
    std::array<size_t, 10> arr{};
    size_t current_idx = 0;
public:
    void AddIdx(size_t idx) {
        arr[current_idx] = idx;
        if (current_idx < 9) {
            current_idx++;
        } 
    }
    void Clear() {
        current_idx = 0;
    }
    size_t Size() {
        return current_idx+1;
    }
    size_t At(size_t idx) {
        if (idx >= arr.size() || idx < 0) {
            return 0;
        }
        return arr[idx];
    }
};
*/
//To Be Split
template <typename T>
requires std::integral<T>
class BarChart {
    std::vector<Bar> bars;
    sf::RenderTarget& target;

    std::vector<std::string> instructions;
    size_t instr_ptr = 0;

    float delta_accum = 0;
    float delta_mark = 1;
    int to_move = 0;
    IdxBag idx_bag;

    float map_height_val(T v, float vec_min, float vec_max) {
        float min_h = 20;
        float max_h = HEIGHT - 40;
        if (vec_min == vec_max) {
            return min_h;
        }
        float norm = (v - vec_min) / (vec_max - vec_min);
        return min_h + norm * (max_h - min_h);
    }

    void generate_bars(std::vector<T> val_vec) 
    {
        float bar_space = float(WIDTH) / (val_vec.size() + 2);
        float x = bar_space;
        float y = 20;
        float bar_width = bar_space * 0.8;

        float val_vec_max = float(*std::max_element(val_vec.begin(), val_vec.end()));
        float val_vec_min = float(*std::min_element(val_vec.begin(), val_vec.end()));

        for (size_t i = 0; i < val_vec.size(); i++){
            float bar_height = map_height_val(val_vec.at(i), val_vec_min, val_vec_max); 
            bars.emplace_back(Bar(target, x, y, bar_width, bar_height));
            x += bar_space;
        }
    }

    //C idx_a idx_b
    //S idx_a idx_b
    void execute_instruction() {
        std::vector<std::string> cmd = Utils::split_string(this->instructions.at(this->instr_ptr), " ");
        //clean previous highlights
        for (size_t i=0; i<this->idx_bag.Size(); i++) {
            bars.at(idx_bag.At(i)).Highlight(false);
        }
        idx_bag.Clear();

        if (cmd.at(0) == "C") {
            size_t idx_a = Utils::convert_to_idx(cmd.at(1));
            bars.at(idx_a).Highlight(true);
            idx_bag.AddIdx(idx_a);

            size_t idx_b = Utils::convert_to_idx(cmd.at(2));
            bars.at(idx_b).Highlight(true);
            idx_bag.AddIdx(idx_b);
            std::cout<<this->instructions.at(this->instr_ptr);
        }
        else if (cmd.at(0) == "S") {
            size_t idx_a = Utils::convert_to_idx(cmd.at(1));
            bars.at(idx_a).Highlight(true);
            idx_bag.AddIdx(idx_a);

            size_t idx_b = Utils::convert_to_idx(cmd.at(2));
            bars.at(idx_b).Highlight(true);
            idx_bag.AddIdx(idx_b);

            std::swap(bars.at(idx_a), bars.at(idx_b));
            std::cout<<this->instructions.at(this->instr_ptr);
        }
    }

public:
    BarChart(sf::RenderTarget& target, std::vector<std::string> instructions)
    : target(target), instructions(instructions) {
        std::string vec_str = this->instructions.at(0);
        auto val_vec = Utils::convert_to_integrals<T>(Utils::split_string(vec_str, ";"));
        generate_bars(val_vec);
    }
    BarChart(sf::RenderTarget& target, const std::string& file_path)
        : target(target) {
        this->instructions = Utils::load_file_lines(file_path);
        std::string vec_str = this->instructions.at(0);
        auto val_vec = Utils::convert_to_integrals<T>(Utils::split_string(vec_str, ";"));
        generate_bars(val_vec);
    }
    void Move(int direction) {
        if ((this->instr_ptr + direction) >= instructions.size() || (this->instr_ptr + direction) < 0) return; //out of bounds

        this->to_move += direction;

        if (this->delta_accum >= this->delta_mark) {
            delta_accum = 0;
            if (to_move < 0) {
                this->instr_ptr--;
                execute_instruction();
                to_move++;
            }
            if (to_move > 0) {
                this->instr_ptr++;
                execute_instruction();
                to_move--;
            }
        }
    }
    void Draw() {
        for (const Bar& b : this->bars) {
            b.Draw();
        }
    }
    void Update(float delta_time) {
        this->delta_accum += delta_time;
        Move(1);
        this->Draw();
    }
};

template <typename T>
requires std::integral<T>

void BubbleSortOptimized(VisualVector<T, LoggingAllocator<T>>& vec) {
    size_t n = vec.Size();
    if (n == 0) return;

    for (size_t i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (size_t j = 0; j < n - i - 1; ++j) {
            if (vec.Cmp(j, j+1) > 0) { 
                vec.Swap(j, j + 1);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

int main() {
    std::vector<int, LoggingAllocator<int>> vec = {42, 15, 88, 7, 23, 91, 34, 62, 5, 50, 19, 76, 12, 99, 31, 54, 8, 67, 45, 20};

    // SFML 3.1 uses initializer lists for VideoMode dimensions
    auto window = sf::RenderWindow(sf::VideoMode({WIDTH, HEIGHT}), "Algorithm Visualizer by Petr Chyla");
    window.setFramerateLimit(60);

    VisualVector vvec = VisualVector(window, vec);
    BubbleSortOptimized(vvec);
    Utils::print_vec<int>(vvec.GetDataVec());
    vvec.StoreInstructions("instructions.txt");

    BarChart bchart = BarChart<int>(window, vvec.GetInstructions());

    //delta time setup
    std::chrono::time_point<std::chrono::steady_clock> last, now;
    std::chrono::duration<float> dt;
    float delta_time;
    using clock = std::chrono::steady_clock;

    while (window.isOpen()) {
        //delta time calcs
        now = clock::now();
        dt = now - last;
        last = now;
        delta_time = dt.count();

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear(sf::Color::Black);
        bchart.Update(delta_time);
        bchart.Draw();
        window.display();
    }
    ThreadSafeLogger::print_log();
    return 0;
}

//New TP: template properly...

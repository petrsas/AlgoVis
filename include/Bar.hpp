#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>

//make it global somehow
inline const sf::Color MAIN_COLOR = sf::Color::White;
inline const sf::Color HIGHLIGHT_COLOR = sf::Color::Red;

class Bar {
private:
    float x{}, y{}, w{}, h{};
    sf::RectangleShape shape;
    sf::RenderTarget* target{nullptr}; 
    bool IsHigh{false};

public:
    Bar(sf::RenderTarget& target, float x, float y, float w, float h);

    // Rule of 5 / Copy assignment??? This is broken
    Bar& operator=(const Bar& other);

    void Draw() const;
    void Highlight(bool highlight);
};

#include "Bar.h"

Bar::Bar(sf::RenderTarget& target, float x, float y, float w, float h)
    : x(x), y(y), w(w), h(h), target(&target), IsHigh(false) {
    shape.setPosition({x, y});
    shape.setSize({w, h});
    shape.setFillColor(MAIN_COLOR);
}

Bar& Bar::operator=(const Bar& other) {
    if (this != &other) {
        x = other.x;
        y = other.y;
        w = other.w;
        h = other.h;
        IsHigh = other.IsHigh;
        target = other.target; // Pointer can be safely reassigned

        shape.setPosition({x, y});
        shape.setSize({w, h});
        shape.setFillColor(IsHigh ? HIGHLIGHT_COLOR : MAIN_COLOR);
    }
    return *this;
}

void Bar::Draw() const {
    if (target) {
        target->draw(shape);
    }
}

void Bar::Highlight(bool highlight) {
    IsHigh = highlight;
    if (IsHigh) {
        shape.setFillColor(HIGHLIGHT_COLOR);
    } else {
        shape.setFillColor(MAIN_COLOR);
    }
}

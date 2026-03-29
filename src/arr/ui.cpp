#include "ui.hpp"

#include "sdlxx.hpp"

#include <cstdint>

UI::UI()
{
    // Test layout

    auto* button = emplace<Button>();
    button->origin(200, 100);
    button->width(600);
    button->height(400);
    button->color(200, 30, 30);
}

void UI::present(sdl::Renderer& rr) const
{
    for (const auto& widget : _widgets) {
        widget->present(rr, ScreenVector::zero());
    }
}

void Button::origin(float x, float y)
{
    _origin.x = x;
    _origin.y = y;
}

void Button::color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    _color = SDL_Color{.r = r, .g = g, .b = b, .a = a};
}

void Button::width(float w)
{
    _size.x = w;
}

void Button::height(float h)
{
    _size.y = h;
}

void Button::present(sdl::Renderer& rr, const ScreenVector& offset) const
{
    rr.drawColor(_color.r, _color.g, _color.b, _color.a);
    rr.fillRect(
        SDL_FRect{
            .x = _origin.x + offset.x,
            .y = _origin.y + offset.y,
            .w = _size.x,
            .h = _size.y,
        });
}

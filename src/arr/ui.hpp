#pragma once

#include "geometry.hpp"

#include "sdlxx.hpp"

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

struct ScreenDomain;

using ScreenPoint = Point<float, ScreenDomain>;
using ScreenVector = Vector<float, ScreenDomain>;

class Widget {
public:
    virtual ~Widget() = default;

    virtual void update(double) { }

    virtual void present(
        sdl::Renderer& rr, const ScreenVector& offset) const = 0;
};

class UI {
public:
    UI();

    void present(sdl::Renderer& rr) const;

    template <std::derived_from<Widget> T, class... Args>
    T* emplace(Args&&... args)
    {
        _widgets.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        return reinterpret_cast<T*>(_widgets.back().get());
    }

private:
    std::vector<std::unique_ptr<Widget>> _widgets;
};

class Button : public Widget {
public:
    void origin(float x, float y);
    void size(float w, float h);
    void color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    void present(sdl::Renderer& rr, const ScreenVector& offset) const override;

private:
    ScreenPoint _origin;
    ScreenVector _size;
    SDL_Color _color;
};

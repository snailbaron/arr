#pragma once

#include "assets.hpp"
#include "geometry.hpp"
#include "timer.hpp"

#include "sdlxx.hpp"

#include <concepts>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <utility>
#include <vector>

struct ScreenDomain;

using ScreenPoint = Point<float, ScreenDomain>;
using ScreenVector = Vector<float, ScreenDomain>;

enum class WidgetState : uint8_t {
    Neutral,
    Focused,
    Pressed,
    Left,
};

class Widget {
public:
    virtual ~Widget() = default;

    virtual void update(double /*delta*/) { }

    virtual void onState(WidgetState /*state*/) { }

    virtual void onClick() const { }

    [[nodiscard]] virtual Widget* locate(float /*x*/, float /*y*/)
    {
        return nullptr;
    }

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

    void processEvent(const SDL_Event& e);

private:
    std::vector<std::unique_ptr<Widget>> _widgets;
    Widget* _widgetUnderCursor = nullptr;
    Widget* _pressedWidget = nullptr;
};

class Button : public Widget {
public:
    explicit Button(const assets::ButtonSprite& sprite);
    Button* origin(float x, float y) &;
    Button* click(std::function<void()> f) &;

    void update(double delta) override;
    void onState(WidgetState state) override;
    void onClick() const override;
    Widget* locate(float x, float y) override;

    void present(sdl::Renderer& rr, const ScreenVector& offset) const override;

private:
    const assets::ButtonSprite* _sprite = nullptr;
    std::span<const SDL_FRect> _frames;
    ScreenPoint _origin;
    Ticker _ticker{60};
    std::function<void()> _onClick;
};

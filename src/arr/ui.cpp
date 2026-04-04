#include "ui.hpp"

#include "assets.hpp"
#include "sdlxx.hpp"

#include <functional>
#include <utility>

UI::UI() = default;

void UI::present(sdl::Renderer& rr) const
{
    for (const auto& widget : _widgets) {
        widget->present(rr, ScreenVector::zero());
    }
}

void UI::processEvent(const SDL_Event& e)
{
    if (e.type == SDL_EVENT_MOUSE_MOTION) {
        if ((_widgetUnderCursor != nullptr) &&
            _widgetUnderCursor->locate(e.motion.x, e.motion.y) !=
                _widgetUnderCursor) {
            if (_pressedWidget == _widgetUnderCursor) {
                _widgetUnderCursor->onState(WidgetState::Left);
            } else if (_pressedWidget == nullptr) {
                _widgetUnderCursor->onState(WidgetState::Neutral);
            }
            _widgetUnderCursor = nullptr;
        }

        if (_widgetUnderCursor == nullptr) {
            for (const auto& widget : _widgets) {
                if (auto* w = widget->locate(e.motion.x, e.motion.y)) {
                    _widgetUnderCursor = w;
                    if (_pressedWidget == nullptr) {
                        _widgetUnderCursor->onState(WidgetState::Focused);
                    }
                    break;
                }
            }
        }
    }

    if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
        e.button.button == SDL_BUTTON_LEFT) {
        if (_widgetUnderCursor != nullptr) {
            _pressedWidget = _widgetUnderCursor;
            _widgetUnderCursor->onState(WidgetState::Pressed);
        }
    }

    if (e.type == SDL_EVENT_MOUSE_BUTTON_UP &&
        e.button.button == SDL_BUTTON_LEFT) {
        if (_pressedWidget != nullptr) {
            if (_pressedWidget == _widgetUnderCursor) {
                _pressedWidget->onClick();
                _pressedWidget->onState(WidgetState::Focused);
            } else {
                _pressedWidget->onState(WidgetState::Neutral);
            }
            _pressedWidget = nullptr;
        }
    }
}

Button::Button(const assets::ButtonSprite& sprite)
    : _sprite(&sprite)
    , _frames(_sprite->animations.neutral)
{ }

Button* Button::origin(float x, float y) &
{
    _origin.x = x;
    _origin.y = y;
    return this;
}

Button* Button::click(std::function<void()> f) &
{
    _onClick = std::move(f);
    return this;
}

void Button::update(double delta)
{
    _ticker.tick(delta);
}

void Button::onState(WidgetState state)
{
    const auto& a = _sprite->animations;
    switch (state) {
    case WidgetState::Neutral: _frames = a.neutral; break;
    case WidgetState::Focused: _frames = a.focused; break;
    case WidgetState::Pressed: _frames = a.pressed; break;
    case WidgetState::Left: _frames = a.left; break;
    }
}

void Button::onClick() const
{
    _onClick();
}

Widget* Button::locate(float x, float y)
{
    auto xInside = x >= _origin.x && x < _origin.x + _sprite->size.w;
    auto yInside = y >= _origin.y && y < _origin.y + _sprite->size.h;
    if (xInside && yInside) {
        return this;
    }
    return nullptr;
}

void Button::present(sdl::Renderer& rr, const ScreenVector& offset) const
{
    rr.render(
        assets::ButtonSprite::texture(),
        _frames.at(_ticker.frame() % _frames.size()),
        SDL_FRect{
            .x = _origin.x + offset.x,
            .y = _origin.y + offset.y,
            .w = static_cast<float>(_sprite->size.w),
            .h = static_cast<float>(_sprite->size.h),
        });
}

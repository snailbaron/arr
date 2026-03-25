#pragma once

#include "holder.hpp"

#include <SDL3/SDL.h>

#include <cstdint>
#include <limits>
#include <span>

namespace sdl {

class Init final {
public:
    explicit Init(SDL_InitFlags flags);
    ~Init();

    Init(const Init&) = delete;
    Init(Init&&) noexcept = delete;
    Init& operator=(const Init&) = delete;
    Init& operator=(Init&&) noexcept = delete;
};

class Window : public internal::Holder<SDL_Window, SDL_DestroyWindow> {
public:
    Window(const char* title, int w, int h, SDL_WindowFlags flags);
};

class Renderer : public internal::Holder<SDL_Renderer, SDL_DestroyRenderer> {
public:
    explicit Renderer(Window& window, const char* name = nullptr);

    void present();

    void drawColor(
        uint8_t r,
        uint8_t g,
        uint8_t b,
        uint8_t a = std::numeric_limits<uint8_t>::max());
    void clear();

    void renderRect(const SDL_FRect& rect);
    void renderRects(std::span<const SDL_FRect> rects);
    void fillRect(const SDL_FRect& rect);
    void fillRects(std::span<const SDL_FRect> rects);
};

} // namespace sdl

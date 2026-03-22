#pragma once

#include "holder.hpp"

#include <SDL3/SDL.h>

#include <cstdint>

namespace sdl {

class Init final {
public:
    Init(SDL_InitFlags flags);
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
    Renderer(Window& window, const char* name = nullptr);

    void present();

    void drawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void clear();
};

} // namespace sdl

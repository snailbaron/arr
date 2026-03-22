#include "sdlxx.hpp"

#include "err.hpp"

#include <mutex>
#include <source_location>

namespace sdl {

namespace {

std::mutex _initMutex;
bool _initialized = false;

void check(
    bool success, std::source_location sl = std::source_location::current())
{
    if (!success) {
        throw err::Error{SDL_GetError(), sl};
    }
}

template <class T>
T* check(T* ptr, std::source_location sl = std::source_location::current())
{
    if (ptr == nullptr) {
        throw err::Error{SDL_GetError(), sl};
    }
    return ptr;
}

} // namespace

Init::Init(SDL_InitFlags flags)
{
    auto lock = std::lock_guard{_initMutex};
    if (_initialized) {
        throw err::Error{"sdl::Init created twice"};
    }

    check(SDL_Init(flags));
    _initialized = true;
}

Init::~Init()
{
    auto lock = std::lock_guard{_initMutex};
    SDL_Quit();
    _initialized = false;
}

Window::Window(const char* title, int w, int h, SDL_WindowFlags flags)
    : Holder{check(SDL_CreateWindow(title, w, h, flags))}
{ }

Renderer::Renderer(Window& window, const char* name)
    : Holder{check(SDL_CreateRenderer(window.ptr(), name))}
{ }

void Renderer::present()
{
    check(SDL_RenderPresent(ptr()));
}

void Renderer::drawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    check(SDL_SetRenderDrawColor(ptr(), r, g, b, a));
}

void Renderer::clear()
{
    check(SDL_RenderClear(ptr()));
}

} // namespace sdl

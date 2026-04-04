#include "sdlxx.hpp"

#include "err.hpp"
#include "holder.hpp"

#include <SDL3_image/SDL_image.h>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <mutex>
#include <source_location>
#include <span>

namespace sdl {

namespace {

std::mutex initMutex;
bool initialized = false;

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
    auto lock = std::scoped_lock{initMutex};
    if (initialized) {
        throw err::Error{"sdl::Init created twice"};
    }

    check(SDL_Init(flags));
    initialized = true;
}

Init::~Init()
{
    auto lock = std::scoped_lock{initMutex};
    SDL_Quit();
    initialized = false;
}

IOStream::IOStream(std::span<const std::byte> mem)
{
    _ptr.reset(check(SDL_IOFromConstMem(mem.data(), mem.size())));
}

constexpr const SDL_IOStream* IOStream::ptr() const noexcept
{
    return _ptr.get();
}

constexpr SDL_IOStream* IOStream::ptr() noexcept
{
    return _ptr.get();
}

void IOStream::close()
{
    check(SDL_CloseIO(_ptr.release()));
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

void Renderer::renderRect(const SDL_FRect& rect)
{
    check(SDL_RenderRect(ptr(), &rect));
}

void Renderer::renderRects(std::span<const SDL_FRect> rects)
{
    check(SDL_RenderRects(ptr(), rects.data(), static_cast<int>(rects.size())));
}

void Renderer::fillRect(const SDL_FRect& rect)
{
    check(SDL_RenderFillRect(ptr(), &rect));
}

void Renderer::fillRects(std::span<const SDL_FRect> rects)
{
    check(SDL_RenderFillRects(
        ptr(), rects.data(), static_cast<int>(rects.size())));
}

Texture Renderer::loadTexture(const std::filesystem::path& file)
{
    return Texture{check(IMG_LoadTexture(ptr(), file.string().c_str()))};
}

Texture Renderer::loadTexture(std::span<const std::byte> mem)
{
    auto io = IOStream{mem};
    auto texture = Texture{check(IMG_LoadTexture_IO(ptr(), io.ptr(), false))};
    io.close();
    return texture;
}

void Renderer::render(
    Texture& texture, const SDL_FRect& srcrect, const SDL_FRect& dstrect)
{
    check(SDL_RenderTexture(ptr(), texture.ptr(), &srcrect, &dstrect));
}

} // namespace sdl

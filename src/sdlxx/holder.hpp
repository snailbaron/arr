#pragma once

#include <memory>

namespace sdl::internal {

template <class T, void (*deleter)(T*)>
class Holder {
public:
    constexpr explicit Holder(T* ptr) noexcept
    {
        _ptr.reset(ptr);
    }

    [[nodiscard]] constexpr const T* ptr() const noexcept
    {
        return _ptr.get();
    }

    [[nodiscard]] constexpr T* ptr() noexcept
    {
        return _ptr.get();
    }

private:
    std::unique_ptr<T, void (*)(T*)> _ptr{nullptr, deleter};
};

} // namespace sdl::internal

#pragma once

#include <concepts>

template <class T>
concept Scalar = std::integral<T> || std::floating_point<T>;

template <Scalar T, class Domain>
struct Vector {
    static constexpr Vector zero{};

    constexpr Vector& operator+=(const Vector& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr Vector& operator-=(const Vector& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr Vector& operator*=(T scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr Vector& operator/=(T scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    T x;
    T y;

private:
    friend constexpr auto operator<=>(const Vector&, const Vector&) noexcept =
        default;
};

template <Scalar T, class Domain>
Vector<T, Domain> constexpr operator-(const Vector<T, Domain>& vector)
{
    return {.x = -vector.x, .y = -vector.y};
}

template <Scalar T, class Domain>
Vector<T, Domain> constexpr operator+(
    Vector<T, Domain> lhs, const Vector<T, Domain>& rhs)
{
    lhs += rhs;
    return lhs;
}

template <Scalar T, class Domain>
Vector<T, Domain> constexpr operator-(
    Vector<T, Domain> lhs, const Vector<T, Domain>& rhs)
{
    lhs -= rhs;
    return lhs;
}

template <Scalar T, class Domain>
Vector<T, Domain> constexpr operator*(Vector<T, Domain> vector, T scalar)
{
    vector *= scalar;
    return vector;
}

template <Scalar T, class Domain>
Vector<T, Domain> constexpr operator*(T scalar, Vector<T, Domain> vector)
{
    vector *= scalar;
    return vector;
}

template <Scalar T, class Domain>
Vector<T, Domain> constexpr operator/(Vector<T, Domain> vector, T scalar)
{
    vector /= scalar;
    return vector;
}

template <Scalar T, class Domain>
struct Point {
    constexpr Point& operator+=(const Vector<T, Domain>& vector)
    {
        x += vector.x;
        y += vector.y;
        return *this;
    }

    constexpr Point& operator-=(const Vector<T, Domain>& vector)
    {
        x -= vector.x;
        y -= vector.y;
        return *this;
    }

    T x;
    T y;

private:
    friend constexpr auto operator<=>(const Point&, const Point&) = default;
};

template <Scalar T, class Domain>
Point<T, Domain> constexpr operator+(
    Point<T, Domain> point, const Vector<T, Domain>& vector)
{
    point += vector;
    return point;
}

template <Scalar T, class Domain>
Point<T, Domain> constexpr operator-(
    Point<T, Domain> point, const Vector<T, Domain>& vector)
{
    point -= vector;
    return point;
}

template <Scalar T, class Domain>
class Matrix {
public:

private:
};

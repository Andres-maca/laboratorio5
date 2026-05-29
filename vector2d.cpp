#include "vector2d.h"

Vector2::Vector2()
    : x(0.0),
      y(0.0)
{
}

Vector2::Vector2(double xValue, double yValue)
    : x(xValue),
      y(yValue)
{
}

double Vector2::length() const
{
    return std::sqrt(lengthSquared());
}

double Vector2::lengthSquared() const
{
    return x * x + y * y;
}

Vector2 Vector2::normalized() const
{
    const double value = length();
    if (value == 0.0) {
        return Vector2(0.0, 0.0);
    }

    return Vector2(x / value, y / value);
}

Vector2 Vector2::operator+(const Vector2 &other) const
{
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2 &other) const
{
    return Vector2(x - other.x, y - other.y);
}

Vector2 Vector2::operator*(double scalar) const
{
    return Vector2(x * scalar, y * scalar);
}

Vector2 Vector2::operator/(double scalar) const
{
    return Vector2(x / scalar, y / scalar);
}

Vector2 &Vector2::operator+=(const Vector2 &other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vector2 &Vector2::operator-=(const Vector2 &other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector2 &Vector2::operator*=(double scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

double Vector2::dot(const Vector2 &a, const Vector2 &b)
{
    return a.x * b.x + a.y * b.y;
}

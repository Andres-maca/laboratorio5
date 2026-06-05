#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

class Vector2
{
public:
    double x;
    double y;

    Vector2();
    Vector2(double xValue, double yValue);

    double length() const;
    double lengthSquared() const;
    Vector2 normalized() const;

    Vector2 operator+(const Vector2 &other) const;
    Vector2 operator-(const Vector2 &other) const;
    Vector2 operator*(double scalar) const;
    Vector2 operator/(double scalar) const;

    Vector2 &operator+=(const Vector2 &other);
    Vector2 &operator-=(const Vector2 &other);
    Vector2 &operator*=(double scalar);

    static double dot(const Vector2 &a, const Vector2 &b);
};

#endif

#include "vector2.h"

inline Vector2::Vector2() 
    : x(0.0f)
    , y(0.0f)
{}

inline Vector2::Vector2(float val) 
    : Vector2(val, val)
{}

inline Vector2::Vector2(float x, float y) 
    : x(x)
    , y(y)
{}

inline Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

inline Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

inline Vector2 Vector2::operator*(const Vector2& other) const {
    return Vector2(x * other.x, y * other.y);
}

inline Vector2 Vector2::operator/(const Vector2& other) const {
    return Vector2(x / other.x, y / other.y);
}

inline Vector2& Vector2::operator+=(const Vector2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

inline Vector2& Vector2::operator*=(const Vector2& other) {
    x *= other.x;
    y *= other.y;
    return *this;
}

inline Vector2& Vector2::operator/=(const Vector2& other) {
    x /= other.x;
    y /= other.y;
    return *this;
}

inline Vector2 Vector2::operator-(float val) const {
    return Vector2(x - val, y - val);
}

inline Vector2 Vector2::operator+(float val) const {
    return Vector2(x + val, y + val);
}

inline Vector2 Vector2::operator*(float val) const {
    return Vector2(x * val, y * val);
}

inline Vector2 Vector2::operator/(float val) const {
    return Vector2(x / val, y / val);
}

inline Vector2& Vector2::operator+=(float val) {
    x += val;
    y += val;
    return *this;
}

inline Vector2& Vector2::operator-=(float val) {
    x -= val;
    y -= val;
    return *this;
}

inline Vector2& Vector2::operator*=(float val) {
    x *= val;
    y *= val;
    return *this;
}

inline Vector2& Vector2::operator/=(float val) {
    x /= val;
    y /= val;
    return *this;
}

inline bool Vector2::operator==(const Vector2& other) const {
    return x == other.x && y == other.y;
}

inline bool Vector2::operator!=(const Vector2& other) const {
    return x != other.x || y != other.y;
}

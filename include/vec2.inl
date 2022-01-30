#include "vec2.h"

inline vec2::vec2() 
    : x(0.0f)
    , y(0.0f)
{}

inline vec2::vec2(float val) 
    : vec2(val, val)
{}

inline vec2::vec2(float x, float y) 
    : x(x)
    , y(y)
{}

inline vec2 vec2::operator+(const vec2& other) const {
    return vec2(x + other.x, y + other.y);
}

inline vec2 vec2::operator-(const vec2& other) const {
    return vec2(x - other.x, y - other.y);
}

inline vec2 vec2::operator*(const vec2& other) const {
    return vec2(x * other.x, y * other.y);
}

inline vec2 vec2::operator/(const vec2& other) const {
    return vec2(x / other.x, y / other.y);
}

inline vec2& vec2::operator+=(const vec2& other) {
    x += other.x;
    y += other.y;
    return *this;
}

inline vec2& vec2::operator-=(const vec2& other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

inline vec2& vec2::operator*=(const vec2& other) {
    x *= other.x;
    y *= other.y;
    return *this;
}

inline vec2& vec2::operator/=(const vec2& other) {
    x /= other.x;
    y /= other.y;
    return *this;
}

inline vec2 vec2::operator-(float val) const {
    return vec2(x - val, y - val);
}

inline vec2 vec2::operator+(float val) const {
    return vec2(x + val, y + val);
}

inline vec2 vec2::operator*(float val) const {
    return vec2(x * val, y * val);
}

inline vec2 vec2::operator/(float val) const {
    return vec2(x / val, y / val);
}

inline vec2& vec2::operator+=(float val) {
    x += val;
    y += val;
    return *this;
}

inline vec2& vec2::operator-=(float val) {
    x -= val;
    y -= val;
    return *this;
}

inline vec2& vec2::operator*=(float val) {
    x *= val;
    y *= val;
    return *this;
}

inline vec2& vec2::operator/=(float val) {
    x /= val;
    y /= val;
    return *this;
}

inline bool vec2::operator==(const vec2& other) const {
    return x == other.x && y == other.y;
}

inline bool vec2::operator!=(const vec2& other) const {
    return x != other.x || y != other.y;
}

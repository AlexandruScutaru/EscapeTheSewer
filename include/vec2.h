#ifndef VECTOR2_H
#define VECTOR2_H


class vec2 {
public:
    vec2();
    vec2(float val);
    vec2(float x, float y);
    vec2(const vec2& other);
    vec2& operator=(const vec2& other);

    vec2 operator+(const vec2& other) const;
    vec2 operator-(const vec2& other) const;
    vec2 operator*(const vec2& other) const;
    vec2 operator/(const vec2& other) const;
    vec2& operator+=(const vec2& other);
    vec2& operator-=(const vec2& other);
    vec2& operator*=(const vec2& other);
    vec2& operator/=(const vec2& other);

    vec2 operator+(float val) const;
    vec2 operator-(float val) const;
    vec2 operator*(float val) const;
    vec2 operator/(float val) const;
    vec2& operator+=(float val);
    vec2& operator-=(float val);
    vec2& operator*=(float val);
    vec2& operator/=(float val);

    bool operator==(const vec2& other) const;
    bool operator!=(const vec2& other) const;

    float x;
    float y;
};

#endif // VECTOR2_H

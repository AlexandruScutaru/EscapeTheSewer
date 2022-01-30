#ifndef VEC2_H
#define VEC2_H


struct vec2 {
    vec2();
    vec2(float val);
    vec2(float x, float y);

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

#endif // VEC2_H

#ifndef VECTOR2_H
#define VECTOR2_H


class Vector2 {
public:
    Vector2();
    Vector2(float val);
    Vector2(float x, float y);

    Vector2 operator+(const Vector2& other) const;
    Vector2	operator-(const Vector2& other) const;
    Vector2 operator*(const Vector2& other) const;
    Vector2	operator/(const Vector2& other) const;
    Vector2& operator+=(const Vector2& other);
    Vector2& operator-=(const Vector2& other);
    Vector2& operator*=(const Vector2& other);
    Vector2& operator/=(const Vector2& other);

    Vector2 operator+(float val) const;
    Vector2 operator-(float val) const;
    Vector2 operator*(float val) const;
    Vector2 operator/(float val) const;
    Vector2& operator+=(float val);
    Vector2& operator-=(float val);
    Vector2& operator*=(float val);
    Vector2& operator/=(float val);

    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;

    float x;
    float y;
};

#endif // VECTOR2_H

#pragma once

#include "String.h"

class Vector3;

namespace Compound {
    class Array;
}

class EXPORT Vector2 : public IData
{
public:
    Vector2();
    explicit Vector2(float f);
    Vector2(float x, float y);
    Vector2(const Vector3& rhs);
    Vector2(const Vector2& rhs);

    float magnitude() const;
    float dot_product(const Vector2& rhs) const;
    Vector2 normalized() const;
    void normalize();
    String to_string() const;
    
    static float angle_global(const Vector2& from, const Vector2& to);
    static float distance(const Vector2& a, const Vector2& b) { return (a - b).magnitude(); }

    Vector2 operator+(const Vector2& rhs) const;
    Vector2 operator-(const Vector2& rhs) const;
    Vector2 operator*(const Vector2& rhs) const;
    Vector2 operator/(const Vector2& rhs) const;
    void operator+=(const Vector2& rhs);
    void operator-=(const Vector2& rhs);
    void operator*=(const Vector2& rhs);
    void operator/=(const Vector2& rhs);
    Vector2 operator+(float rhs) const;
    Vector2 operator-(float rhs) const;
    Vector2 operator*(float rhs) const;
    Vector2 operator/(float rhs) const;
    void operator+=(float rhs);
    void operator-=(float rhs);
    void operator*=(float rhs);
    void operator/=(float rhs);
    bool operator==(const Vector2& rhs) const;
    bool operator!=(const Vector2& rhs) const;

    void convert_to(Compound::Array& to) const;
    void convert_from(const Compound::Array& from);
    
    float x;
    float y;

    static Vector2 zero();
    static Vector2 one();
    static Vector2 right();
    static Vector2 up();
};

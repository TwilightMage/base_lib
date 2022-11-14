#pragma once

#include "Compound.h"
#include "framework.h"
#include "IConvertible.h"
#include "String.h"

class Vector3;

class EXPORT Vector2 : public IData
{
public:
    Vector2();
    explicit Vector2(float f);
    Vector2(float x, float y);
    Vector2(const Vector3& rhs);
    Vector2(const Vector2& rhs);

    FORCEINLINE float magnitude() const;
    FORCEINLINE float dot_product(const Vector2& rhs) const;
    Vector2 normalized() const;
    void normalize();
    FORCEINLINE String to_string() const;
    
    static float angle_global(const Vector2& from, const Vector2& to);
    FORCEINLINE static float distance(const Vector2& a, const Vector2& b) { return (a - b).magnitude(); }

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

    void convert_to(Compound::Object& to) const;
    void convert_from(const Compound::Object& from);
    
    float x;
    float y;

    FORCEINLINE static Vector2 zero();
    FORCEINLINE static Vector2 one();
    FORCEINLINE static Vector2 right();
    FORCEINLINE static Vector2 up();
};

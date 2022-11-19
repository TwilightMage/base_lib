#pragma once

#include "Vector2.h"

class EXPORT Vector3 : public IData
{
public:
    Vector3();
    explicit Vector3(float f);
    Vector3(float x, float y);
    Vector3(float x, float y, float z);
    Vector3(const Vector2& rhs);
    Vector3(const Vector2& vec2, float z);
    Vector3(const Vector3& rhs);

    float magnitude() const;
    float dot_product(const Vector3& rhs) const;
    Vector3 cross_product(const Vector3& rhs) const;
    Vector3 normalized() const;
    void normalize();
    String to_string() const;

    static float angle_deg(const Vector3& a, const Vector3& b);
    static float angle_rad(const Vector3& a, const Vector3& b);
    static float distance(const Vector3& a, const Vector3& b);
    static Vector3 reflect(const Vector3& in_vec, const Vector3& normal);

    float get_min_axis() const;
    float get_max_axis() const;
    
    Vector3 operator+(const Vector3& rhs) const;
    Vector3 operator-(const Vector3& rhs) const;
    Vector3 operator*(const Vector3& rhs) const;
    Vector3 operator/(const Vector3& rhs) const;
    void operator+=(const Vector3& rhs);
    void operator-=(const Vector3& rhs);
    void operator*=(const Vector3& rhs);
    void operator/=(const Vector3& rhs);
    Vector3 operator+(float rhs) const;
    Vector3 operator-(float rhs) const;
    Vector3 operator*(float rhs) const;
    Vector3 operator/(float rhs) const;
    void operator+=(float rhs);
    void operator-=(float rhs);
    void operator*=(float rhs);
    void operator/=(float rhs);
    bool operator==(const Vector3& rhs) const;
    bool operator!=(const Vector3& rhs) const;

    Vector3 operator-() const;

    float sum_all() const;

    void convert_to(Compound::Array& to) const;
    void convert_from(const Compound::Array& from);
    
    float x;
    float y;
    float z;

    static Vector3 zero()    { return {0, 0, 0}; }
    static Vector3 one()     { return {1, 1, 1}; }
    static Vector3 forward() { return {1, 0, 0}; }
    static Vector3 right()   { return {0, 1, 0}; }
    static Vector3 up()      { return {0, 0, 1}; }
};

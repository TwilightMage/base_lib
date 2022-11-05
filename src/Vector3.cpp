#include "../include/Vector3.h"

Vector3::Vector3()
    : x(0.0f)
    , y(0.0f)
    , z(0.0f)
{
}

Vector3::Vector3(float f)
    : x(f)
    , y(f)
    , z(f)
{
}

Vector3::Vector3(float x, float y)
    : x(x)
    , y(y)
    , z(0.0f)
{
}

Vector3::Vector3(float x, float y, float z)
    : x(x)
    , y(y)
    , z(z)
{
}

Vector3::Vector3(const Vector2& rhs)
    : x(rhs.x)
    , y(rhs.y)
    , z(0.0f)
{
}

Vector3::Vector3(const Vector2& vec2, float z)
    : x(vec2.x)
    , y(vec2.y)
    , z(z)
{
}

Vector3::Vector3(const Vector3& rhs)
    : x(rhs.x)
    , y(rhs.y)
    , z(rhs.z)
{
}

float Vector3::magnitude() const
{
    return Math::sqrt(x * x + y * y + z * z);
}

float Vector3::dot_product(const Vector3& rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vector3 Vector3::cross_product(const Vector3& rhs) const
{
    return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);
}

Vector3 Vector3::normalized() const
{
    const auto m = magnitude();
    return Vector3(x / m, y / m, z / m);
}

void Vector3::normalize()
{
    const auto m = magnitude();
    x /= m;
    y /= m;
    z /= m;
}

String Vector3::to_string() const
{
    return String::format("{ x=%f, y=%f, z=%f }", x, y, z);
}

Vector3 Vector3::reflect(const Vector3& in_vec, const Vector3& normal)
{
    return in_vec - normal * in_vec.dot_product(normal) * 2.f;
}

float Vector3::get_min_axis() const
{
    if (x < y)
    {
        return Math::min(x, z);
    }
    else
    {
        return Math::min(y, z);
    }
}

float Vector3::get_max_axis() const
{
    if (x > y)
    {
        return Math::max(x, z);
    }
    else
    {
        return Math::max(y, z);
    }
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
    return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
    return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
}

Vector3 Vector3::operator*(const Vector3& rhs) const
{
    return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
}

Vector3 Vector3::operator/(const Vector3& rhs) const
{
    return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
}

void Vector3::operator+=(const Vector3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
}

void Vector3::operator-=(const Vector3& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
}

void Vector3::operator*=(const Vector3& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
}

void Vector3::operator/=(const Vector3& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
}

Vector3 Vector3::operator+(float rhs) const
{
    return Vector3(x + rhs, y + rhs, z + rhs);
}

Vector3 Vector3::operator-(float rhs) const
{
    return Vector3(x - rhs, y - rhs, z - rhs);
}

Vector3 Vector3::operator*(float rhs) const
{
    return Vector3(x * rhs, y * rhs, z * rhs);
}

Vector3 Vector3::operator/(float rhs) const
{
    return Vector3(x / rhs, y / rhs, z / rhs);
}

void Vector3::operator+=(float rhs)
{
    x += rhs;
    y += rhs;
    z += rhs;
}

void Vector3::operator-=(float rhs)
{
    x -= rhs;
    y -= rhs;
    z -= rhs;
}

void Vector3::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
    z *= rhs;
}

void Vector3::operator/=(float rhs)
{
    x /= rhs;
    y /= rhs;
    z /= rhs;
}

bool Vector3::operator==(const Vector3& rhs) const
{
    return (x == rhs.x && y == rhs.y && z == rhs.z);
}

bool Vector3::operator!=(const Vector3& rhs) const
{
    return (x != rhs.x || y != rhs.y || z != rhs.z);
}

Vector3 Vector3::operator-() const
{
    return Vector3(-x, -y, -z);
}

float Vector3::sum_all() const
{
    return x + y + z;
}

void Vector3::convert_to(Compound::Object& to) const
{
    to = Compound::Object{
        { "x", x},
        { "y", y},
        { "z", z}
    };
}

void Vector3::convert_from(const Compound::Object& from)
{
    x = from.get_float("x", 0);
    y = from.get_float("y", 0);
    z = from.get_float("z", 0);
}

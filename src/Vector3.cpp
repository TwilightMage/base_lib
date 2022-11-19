#include "base_lib/Vector3.h"

#include "base_lib/Math.h"
#include "base_lib/Compound.h"

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

float Vector3::angle_deg(const Vector3& a, const Vector3& b) { return Math::acos_deg(a.normalized().dot_product(b.normalized())); }

float Vector3::angle_rad(const Vector3& a, const Vector3& b) { return Math::acos_rad(a.normalized().dot_product(b.normalized())); }

float Vector3::distance(const Vector3& a, const Vector3& b) { return (a - b).magnitude(); }

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

void Vector3::convert_to(Compound::Array& to) const
{
    to = Compound::Array{x, y, z};
}

void Vector3::convert_from(const Compound::Array& from)
{
    x = from.at_or_default(0, 0.0f).get_float();
    y = from.at_or_default(1, 0.0f).get_float();
    z = from.at_or_default(2, 0.0f).get_float();
}

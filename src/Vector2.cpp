#include "base_lib/Vector2.h"

#include <glm/glm.hpp>

#include "base_lib/Math.h"
#include "base_lib/Vector3.h"
#include "base_lib/Compound.h"

Vector2::Vector2()
    : x(0.0f)
    , y(0.0f)
{
}

Vector2::Vector2(float f)
    : x(f)
    , y(f)
{
}

Vector2::Vector2(float x, float y)
    : x(x)
    , y(y)
{
}

Vector2::Vector2(const Vector3& rhs)
    : x(rhs.x)
    , y(rhs.y)
{
}

Vector2::Vector2(const Vector2& rhs)
    : x(rhs.x)
    , y(rhs.y)
{
}

float Vector2::magnitude() const
{
    return glm::sqrt(x * x + y * y);
}

float Vector2::dot_product(const Vector2& rhs) const
{
    return x * rhs.x + y * rhs.y;
}

Vector2 Vector2::normalized() const
{
    const auto m = magnitude();
    return Vector2(x / m, y / m);
}

void Vector2::normalize()
{
    const auto m = magnitude();
    x /= m;
    y /= m;
}

String Vector2::to_string() const
{
    return String::format("{ x=%f, y=%f }", x, y);
}

float Vector2::angle_global(const Vector2& from, const Vector2& to)
{
    const Vector2 delta_norm = (to - from).normalized();
    const float dot_right = right().dot_product(delta_norm);
    const float dot_up = up().dot_product(delta_norm);

    float angle = Math::acos_deg(dot_right);
    if (dot_up > 0) angle *= -1;
    return angle;
}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
    return Vector2(x + rhs.x, y + rhs.y);
}

Vector2 Vector2::operator-(const Vector2& rhs) const
{
    return Vector2(x - rhs.x, y - rhs.y);
}

Vector2 Vector2::operator*(const Vector2& rhs) const
{
    return Vector2(x * rhs.x, y * rhs.y);
}

Vector2 Vector2::operator/(const Vector2& rhs) const
{
    return Vector2(x / rhs.x, y / rhs.y);
}

void Vector2::operator+=(const Vector2& rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vector2::operator-=(const Vector2& rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

void Vector2::operator*=(const Vector2& rhs)
{
    x *= rhs.x;
    y *= rhs.y;
}

void Vector2::operator/=(const Vector2& rhs)
{
    x /= rhs.x;
    y /= rhs.y;
}

Vector2 Vector2::operator+(float rhs) const
{
    return Vector2(x + rhs, y + rhs);
}

Vector2 Vector2::operator-(float rhs) const
{
    return Vector2(x - rhs, y - rhs);
}

Vector2 Vector2::operator*(float rhs) const
{
    return Vector2(x * rhs, y * rhs);
}

Vector2 Vector2::operator/(float rhs) const
{
    return Vector2(x / rhs, y / rhs);
}

void Vector2::operator+=(float rhs)
{
    x += rhs;
    y += rhs;
}

void Vector2::operator-=(float rhs)
{
    x -= rhs;
    y -= rhs;
}

void Vector2::operator*=(float rhs)
{
    x *= rhs;
    y *= rhs;
}

void Vector2::operator/=(float rhs)
{
    x /= rhs;
    y /= rhs;
}

bool Vector2::operator==(const Vector2& rhs) const
{
    return (x == rhs.x && y == rhs.y);
}

bool Vector2::operator!=(const Vector2& rhs) const
{
    return (x != rhs.x || y != rhs.y);
}

void Vector2::convert_to(Compound::Array& to) const
{
    to = Compound::Array{x, y};
}

void Vector2::convert_from(const Compound::Array& from)
{
    x = from.at_or_default(0, 0.0f).get_float();
    y = from.at_or_default(1, 0.0f).get_float();
}

Vector2 Vector2::zero()
{
    return Vector2(0.0f, 0.0f);
}

Vector2 Vector2::one()
{
    return Vector2(1.0f, 1.0f);
}

Vector2 Vector2::right()
{
    return Vector2(1.0f, 0.0f);
}

Vector2 Vector2::up()
{
    return Vector2(0.0f, 1.0f);
}

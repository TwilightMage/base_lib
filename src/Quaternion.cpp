#include "../include/Quaternion.h"

#include <glm/detail/type_quat.hpp>
#include <glm/gtc/quaternion.hpp>

Quaternion::Quaternion()
    : x(0.0f)
    , y(0.0f)
    , z(0.0f)
    , w(1.0f)
{
}

Quaternion::Quaternion(float x, float y, float z, float w)
    : x(x)
    , y(y)
    , z(z)
    , w(w)
{
}

Quaternion::Quaternion(const Quaternion& rhs)
    : x(rhs.x)
    , y(rhs.y)
    , z(rhs.z)
    , w(rhs.w)
{
}

Quaternion::Quaternion(const Vector3& euler)
{
    const auto c = Vector3(cos(glm::radians(euler.x) * 0.5f), cos(glm::radians(euler.y) * 0.5f), cos(glm::radians(euler.z) * 0.5f));
    const auto s = Vector3(sin(glm::radians(euler.x) * 0.5f), sin(glm::radians(euler.y) * 0.5f), sin(glm::radians(euler.z) * 0.5f));

    w = c.x * c.y * c.z + s.x * s.y * s.z;
    x = s.x * c.y * c.z - c.x * s.y * s.z;
    y = c.x * s.y * c.z + s.x * c.y * s.z;
    z = c.x * c.y * s.z - s.x * s.y * c.z;
}

float Quaternion::magnitude() const
{
    return sqrt(x * x + y * y + z * z + w * w);
}

Vector3 Quaternion::rotate_vector3(const Vector3& rhs) const
{
    Vector3 const quat_vector(x, y, z);
    Vector3 const uv = quat_vector.cross_product(rhs);
    Vector3 const uuv = quat_vector.cross_product(uv);

    return rhs + ((uv * w) + uuv) * 2.0f;
}

Vector3 Quaternion::forward() const
{
    return rotate_vector3(Vector3::forward());
}

Vector3 Quaternion::right() const
{
    return rotate_vector3(Vector3::right());
}

Vector3 Quaternion::up() const
{
    return rotate_vector3(Vector3::up());
}

float Quaternion::pitch() const
{
    return glm::degrees(asin(glm::clamp(-2.0f * (x * z - w * y), -1.0f, 1.0f)));
}

float Quaternion::yaw() const
{
    return glm::degrees(glm::atan(2.0f * (x * y + w * z), w * w + x * x - y * y - z * z));
}

float Quaternion::roll() const
{
    auto const y = static_cast<float>(2) * (this->y * this->z + this->w * this->x);
    auto const x = this->w * this->w - this->x * this->x - this->y * this->y + this->z * this->z;

    if(x == 0.0f && y == 0.0f) return glm::degrees(2.0f * glm::atan(this->x, this->w));

    return glm::degrees(glm::atan(y, x));
}

Vector3 Quaternion::euler() const
{
    return Vector3(roll(), pitch(), yaw());
}

Quaternion Quaternion::normalized() const
{
    const auto m = magnitude();
    return Quaternion(x / m, y / m, z / m, w / m);
}

void Quaternion::normalize()
{
    const auto m = magnitude();
    x /= m;
    y /= m;
    z /= m;
    w /= m;
}

String Quaternion::to_string() const
{
    return String::format("{ x=%f, y=%f, z=%f, q=%f }", x, y, z, w);
}

Quaternion Quaternion::conjugate() const
{
    return Quaternion(-x, -y, -z, w);
}

Vector3 Quaternion::axis() const
{
    return w == 1.0f ? Vector3::up() : Vector3(x / sqrt(1 - w * w), y / sqrt(1 - w * w), z / sqrt(1 - w * w));
}

float Quaternion::axis_angle() const
{
    return 2 * acos(w);
}

float Quaternion::dot(const Quaternion& rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z + w * rhs.w;
}

Quaternion Quaternion::from_axis_angle(const Vector3& axis, float angle)
{   
    const auto factor = sin(glm::radians(angle) / 2.0f);

    return Quaternion(axis.x * factor, axis.y * factor, axis.z * factor, cos(glm::radians(angle) / 2.0f)).normalized();
}

Quaternion Quaternion::look_at(const Vector3& direction)
{
    return look_at(direction, Vector3::up());
    
    if (direction.magnitude() == 0) return Quaternion();

    const Vector3 direction_normal = direction.normalized();
    
    const Vector3 rot_axis = Vector3::forward().cross_product(direction_normal);
    const float dot = Vector3::forward().dot_product(direction_normal);

    Quaternion q;
    q.x = rot_axis.x;
    q.y = rot_axis.y;
    q.z = rot_axis.z;
    q.w = dot + 1;

    return q.normalized();
}

Quaternion Quaternion::look_at(const Vector3& direction, const Vector3& up)
{
    if (direction.magnitude() == 0) return Quaternion();
    
    return cast_object<Quaternion>(quatLookAt(cast_object<glm::vec3>(direction), cast_object<glm::vec3>(up.normalized())));
}

Quaternion Quaternion::slerp(const Quaternion& a, const Quaternion& b, float alpha)
{
    if (1 - Math::abs(a.dot(b)) <= KINDA_SMALL_NUMBER) return b;
    return cast_object<Quaternion>(glm::slerp(cast_object<glm::quat>(a), cast_object<glm::quat>(b), Math::clamp(alpha, 0.0f, 1.0f)));
}

Quaternion Quaternion::lerp(const Quaternion& from, const Quaternion& to, float alpha)
{
    return Quaternion(Math::lerp(from.x, to.x, alpha), Math::lerp(from.y, to.y, alpha), Math::lerp(from.z, to.z, alpha), Math::lerp(from.w, to.w, alpha));
}

Quaternion Quaternion::rotate_around(const Vector3& axis, float angle) const
{
    return from_axis_angle(axis, angle) * *this;
}

Quaternion Quaternion::rotate_around_forward(float angle) const
{
    return from_axis_angle(Vector3::forward(), angle) * *this;
}

Quaternion Quaternion::rotate_around_right(float angle) const
{
    return from_axis_angle(Vector3::right(), angle) * *this;
}

Quaternion Quaternion::rotate_around_up(float angle) const
{
    return from_axis_angle(Vector3::up(), angle) * *this;
}

Quaternion Quaternion::operator*(const Quaternion& rhs) const
{
    return Quaternion(x * rhs.w + y * rhs.z - z * rhs.y + w * rhs.x, -x * rhs.z + y * rhs.w + z * rhs.x + w * rhs.y, x * rhs.y - y * rhs.x + z * rhs.w + w * rhs.z, -x * rhs.x - y * rhs.y - z * rhs.z + w * rhs.w);
}

Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
    const auto x_ = x;
    const auto y_ = y;
    const auto z_ = z;
    const auto w_ = w;
    x =  x_ * rhs.w + y_ * rhs.z - z_ * rhs.y + w_ * rhs.x;
    y = -x_ * rhs.z + y_ * rhs.w + z_ * rhs.x + w_ * rhs.y;
    z =  x_ * rhs.y - y_ * rhs.x + z_ * rhs.w + w_ * rhs.z;
    w = -x_ * rhs.x - y_ * rhs.y - z_ * rhs.z + w_ * rhs.w;

    return *this;
}

bool Quaternion::operator==(const Quaternion& rhs) const
{
    return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
}

bool Quaternion::operator!=(const Quaternion& rhs) const
{
    return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
}

Quaternion Quaternion::operator-() const
{
    return Quaternion(-x, -y, -z, -w);
}

void Quaternion::convert_to(Compound::Object& to) const
{
    to = Compound::Object{
        { "x", x },
        { "y", y },
        { "z", z },
        { "w", w }
    };
}

void Quaternion::convert_from(const Compound::Object& from)
{
    x = from.get_float("x", 0);
    y = from.get_float("y", 0);
    z = from.get_float("z", 0);
    w = from.get_float("w", 1);
}

#pragma once

#include <assert.h>

#include "Compound.h"
#include "IConvertible.h"
#include "Quaternion.h"
#include "Rect.h"
#include "Vector3.h"

class EXPORT Matrix4x4 : public IData
{
public:
    struct Row : public IData
    {
        float x;
        float y;
        float z;
        float w;

        Row()
            : x(0)
            , y(0)
            , z(0)
            , w(0)
        {
        }

        Row(float f)
        : x(f)
        , y(f)
        , z(f)
        , w(f)
        {
        }

        Row(float x, float y, float z, float w)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
        {
        }

        float& operator[](uint index)
        {
            return reinterpret_cast<float*>(this)[index];
        }

        const float& operator[](uint index) const
        {
            return reinterpret_cast<const float*>(this)[index];
        }

        Row operator+(const Row& rhs) const
        {
            return { x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
        }

        Row operator+(float rhs) const
        {
            return { x + rhs, y + rhs, z + rhs, w + rhs };
        }

        Row operator-(const Row& rhs) const
        {
            return { x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
        }

        Row operator-(float rhs) const
        {
            return { x - rhs, y - rhs, z - rhs, w - rhs };
        }

        Row operator*(const Row& rhs) const
        {
            return { x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w };
        }

        Row operator*(float rhs) const
        {
            return { x * rhs, y * rhs, z * rhs, w * rhs };
        }

        Row operator/(const Row& rhs) const
        {
            return { x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w };
        }

        Row operator/(float rhs) const
        {
            return { x / rhs, y / rhs, z / rhs, w / rhs };
        }
    };

    Matrix4x4();
    explicit Matrix4x4(float scale);
    Matrix4x4(const Row& r0, const Row& r1, const Row& r2, const Row& r3);
    Matrix4x4(float x0, float y0, float z0, float w0, float x1, float y1, float z1, float w1, float x2, float y2, float z2, float w2, float x3, float y3, float z3, float w3);

    Matrix4x4 translate(const Vector3& by) const;
    Matrix4x4 rotate(const Quaternion& by) const;
    Matrix4x4 scale(const Vector3& by) const;

    Matrix4x4 inverse() const;

    static Matrix4x4 look_at(const Vector3& from, const Vector3& to);
    static Matrix4x4 perspective(float fov_y, float aspect, float z_near, float z_far);
    static Matrix4x4 ortho(float left, float right, float bottom, float top, float z_near, float z_far);

    static Vector3 un_project(const Vector2& screen_point, const Vector2& screen_size, const Matrix4x4& camera_model, const Matrix4x4& camera_view, const Matrix4x4& camera_projection);

    void convert_to(Compound::Array& to) const;
    void convert_from(const Compound::Array& from);
    
    Matrix4x4 operator*(const Matrix4x4& rhs) const;

    Row operator*(const Row& rhs) const;

    Matrix4x4 operator*(float rhs) const;
    Matrix4x4 operator/(float rhs) const;

    Vector3 operator*(const Vector3& rhs) const;
    Vector2 operator*(const Vector2& rhs) const;

    Row data[4];
};

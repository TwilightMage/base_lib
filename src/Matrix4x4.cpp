#include "../include/Matrix4x4.h"

#include "../include/glm/ext/matrix_transform.hpp"

#include "../include/Math.h"

Matrix4x4::Matrix4x4()
    : Matrix4x4(1.0f)
{
}

Matrix4x4::Matrix4x4(float scale)
    : data {
        {scale, 0, 0, 0},
        {0, scale, 0, 0},
        {0, 0, scale, 0},
        {0, 0, 0, scale}
    }
{
}

Matrix4x4::Matrix4x4(const Row& r0, const Row& r1, const Row& r2, const Row& r3)
	: data{ r0, r1, r2, r3 }
{
}

Matrix4x4::Matrix4x4(float x0, float y0, float z0, float w0, float x1, float y1, float z1, float w1, float x2, float y2,
                     float z2, float w2, float x3, float y3, float z3, float w3)
    : data {
        {x0, y0, z0, w0},
        {x1, y1, z1, w1},
        {x2, y2, z2, w2},
        {x3, y3, z3, w3}
    }
{
}

Matrix4x4 Matrix4x4::translate(const Vector3& by) const
{
    Matrix4x4 result = *this;
    result.data[3] = data[0] * by.x + data[1] * by.y + data[2] * by.z + data[3];
    return result;
}

Matrix4x4 Matrix4x4::rotate(const Quaternion& by) const
{
    const auto a = by.axis_angle();
    const auto c = cos(a);
    const auto s = sin(a);

    const auto axis = by.axis().normalized();
    const auto temp = axis * (1 - c);

    Matrix4x4 Rotate;
    Rotate.data[0][0] = c + temp.x * axis.x;
    Rotate.data[0][1] = temp.x * axis.y + s * axis.z;
    Rotate.data[0][2] = temp.x * axis.z - s * axis.y;

    Rotate.data[1][0] = temp.y * axis.x - s * axis.z;
    Rotate.data[1][1] = c + temp.y * axis.y;
    Rotate.data[1][2] = temp.y * axis.z + s * axis.x;

    Rotate.data[2][0] = temp.z * axis.x + s * axis.y;
    Rotate.data[2][1] = temp.z * axis.y - s * axis.x;
    Rotate.data[2][2] = c + temp.z * axis.z;

    Matrix4x4 Result;
    Result.data[0] = data[0] * Rotate.data[0][0] + data[1] * Rotate.data[0][1] + data[2] * Rotate.data[0][2];
    Result.data[1] = data[0] * Rotate.data[1][0] + data[1] * Rotate.data[1][1] + data[2] * Rotate.data[1][2];
    Result.data[2] = data[0] * Rotate.data[2][0] + data[1] * Rotate.data[2][1] + data[2] * Rotate.data[2][2];
    
    Result.data[3] = data[3];
    return Result;
}

Matrix4x4 Matrix4x4::scale(const Vector3& by) const
{
    Matrix4x4 Result;
    Result.data[0] = data[0] * by.x;
    Result.data[1] = data[1] * by.y;
    Result.data[2] = data[2] * by.z;
    Result.data[3] = data[3];
    return Result;
}

Matrix4x4 Matrix4x4::inverse() const
{
    float Coef00 = data[2][2] * data[3][3] - data[3][2] * data[2][3];
	float Coef02 = data[1][2] * data[3][3] - data[3][2] * data[1][3];
	float Coef03 = data[1][2] * data[2][3] - data[2][2] * data[1][3];

	float Coef04 = data[2][1] * data[3][3] - data[3][1] * data[2][3];
	float Coef06 = data[1][1] * data[3][3] - data[3][1] * data[1][3];
	float Coef07 = data[1][1] * data[2][3] - data[2][1] * data[1][3];

	float Coef08 = data[2][1] * data[3][2] - data[3][1] * data[2][2];
	float Coef10 = data[1][1] * data[3][2] - data[3][1] * data[1][2];
	float Coef11 = data[1][1] * data[2][2] - data[2][1] * data[1][2];

	float Coef12 = data[2][0] * data[3][3] - data[3][0] * data[2][3];
	float Coef14 = data[1][0] * data[3][3] - data[3][0] * data[1][3];
	float Coef15 = data[1][0] * data[2][3] - data[2][0] * data[1][3];

	float Coef16 = data[2][0] * data[3][2] - data[3][0] * data[2][2];
	float Coef18 = data[1][0] * data[3][2] - data[3][0] * data[1][2];
	float Coef19 = data[1][0] * data[2][2] - data[2][0] * data[1][2];

	float Coef20 = data[2][0] * data[3][1] - data[3][0] * data[2][1];
	float Coef22 = data[1][0] * data[3][1] - data[3][0] * data[1][1];
	float Coef23 = data[1][0] * data[2][1] - data[2][0] * data[1][1];

	Row Fac0(Coef00, Coef00, Coef02, Coef03);
	Row Fac1(Coef04, Coef04, Coef06, Coef07);
	Row Fac2(Coef08, Coef08, Coef10, Coef11);
	Row Fac3(Coef12, Coef12, Coef14, Coef15);
	Row Fac4(Coef16, Coef16, Coef18, Coef19);
	Row Fac5(Coef20, Coef20, Coef22, Coef23);

	Row Vec0(data[1][0], data[0][0], data[0][0], data[0][0]);
	Row Vec1(data[1][1], data[0][1], data[0][1], data[0][1]);
	Row Vec2(data[1][2], data[0][2], data[0][2], data[0][2]);
	Row Vec3(data[1][3], data[0][3], data[0][3], data[0][3]);

	Row Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
	Row Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
	Row Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
	Row Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

	Row SignA(+1, -1, +1, -1);
	Row SignB(-1, +1, -1, +1);
	Matrix4x4 Inverse(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

	Row Row0(Inverse.data[0][0], Inverse.data[1][0], Inverse.data[2][0], Inverse.data[3][0]);

	Row Dot0(data[0] * Row0);
	float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

	float OneOverDeterminant = 1.0f / Dot1;

	return Inverse * OneOverDeterminant;

	//return cast_object<Matrix4x4>(glm::inverse(cast_object<glm::mat4>(*this)));
}

Matrix4x4 Matrix4x4::look_at(const Vector3& from, const Vector3& to)
{
    Vector3 zaxis = (from - to).normalized();    
    Vector3 xaxis = zaxis.cross_product(Vector3::up()).normalized();
    Vector3 yaxis = xaxis.cross_product(zaxis);

    xaxis *= -1;

    const auto result = Matrix4x4(
            xaxis.x, yaxis.x, zaxis.x, 0,
            xaxis.y, yaxis.y, zaxis.y, 0,
            xaxis.z, yaxis.z, zaxis.z, 0,
            -xaxis.dot_product(from), -yaxis.dot_product(from), -zaxis.dot_product(from), 1
      );

    return result;
}

Matrix4x4 Matrix4x4::perspective(float fov_y, float aspect, float z_near, float z_far)
{
    assert(abs(aspect - Math::epsilon<float>()) > 0);

    float const tan_half_fov_y = tan(fov_y / 2);

    Matrix4x4 result(0);
    result.data[0][0] = 1.0f / (aspect * tan_half_fov_y);
    result.data[1][1] = 1.0f / (tan_half_fov_y);
    result.data[2][2] = - (z_far + z_near) / (z_far - z_near);
    result.data[2][3] = - 1.0f;
    result.data[3][2] = - (2.0f * z_far * z_near) / (z_far - z_near);
    
    return result;
}

Matrix4x4 Matrix4x4::ortho(float left, float right, float bottom, float top, float z_near, float z_far)
{
    Matrix4x4 result(1);
    result.data[0][0] = 2.0f / (right - left);
    result.data[1][1] = 2.0f / (top - bottom);
    result.data[2][2] = - 2.0f / (z_far - z_near);
    result.data[3][0] = - (right + left) / (right - left);
    result.data[3][1] = - (top + bottom) / (top - bottom);
    result.data[3][2] = - (z_far + z_near) / (z_far - z_near);
    return result;
}

Vector3 Matrix4x4::un_project(const Vector2& screen_point, const Vector2& screen_size, const Matrix4x4& camera_model, const Matrix4x4& camera_view, const Matrix4x4& camera_projection)
{
	const auto inverse = (camera_projection * camera_view * camera_model).inverse();
	const Row point((screen_point.x / screen_size.x) * 2 - 1, (1 - (screen_point.y / screen_size.y)) * 2 - 1, 1.0f, 1.0f);
	auto pos = inverse * point;
	pos.w = 1.0f / pos.w;

	return Vector3(pos.x / pos.w, pos.y / pos.w, pos.z / pos.w).normalized();
}

void Matrix4x4::convert_to(Compound::Array& to) const
{
	to = Compound::Array{
		data[0].x,
		data[0].y,
		data[0].z,
		data[0].w,
		data[1].x,
		data[1].y,
		data[1].z,
		data[1].w,
		data[2].x,
		data[2].y,
		data[2].z,
		data[2].w,
		data[3].x,
		data[3].y,
		data[3].z,
		data[3].w
	};
}

void Matrix4x4::convert_from(const Compound::Array& from)
{
	for (uint i = 0; i < from.length(); i++)
	{
		data[i / 4][i % 4] = from[i].get_float(0);
	}
	
	data[0].x = 1;
	data[1].y = 1;
	data[2].z = 1;
	data[3].w = 1;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rhs) const
{
    const Row SrcA0 = data[0];
    const Row SrcA1 = data[1];
    const Row SrcA2 = data[2];
    const Row SrcA3 = data[3];

    const Row SrcB0 = rhs.data[0];
    const Row SrcB1 = rhs.data[1];
    const Row SrcB2 = rhs.data[2];
    const Row SrcB3 = rhs.data[3];

    Matrix4x4 Result;
    Result.data[0] = SrcA0 * SrcB0.x + SrcA1 * SrcB0.y + SrcA2 * SrcB0.z + SrcA3 * SrcB0.w;
    Result.data[1] = SrcA0 * SrcB1.x + SrcA1 * SrcB1.y + SrcA2 * SrcB1.z + SrcA3 * SrcB1.w;
    Result.data[2] = SrcA0 * SrcB2.x + SrcA1 * SrcB2.y + SrcA2 * SrcB2.z + SrcA3 * SrcB2.w;
    Result.data[3] = SrcA0 * SrcB3.x + SrcA1 * SrcB3.y + SrcA2 * SrcB3.z + SrcA3 * SrcB3.w;
    return Result;
}

Matrix4x4::Row Matrix4x4::operator*(const Row& rhs) const
{
	Row const Mov0(rhs[0]);
	Row const Mov1(rhs[1]);
	Row const Mul0 = data[0] * Mov0;
	Row const Mul1 = data[1] * Mov1;
	Row const Add0 = Mul0 + Mul1;
	Row const Mov2(rhs[2]);
	Row const Mov3(rhs[3]);
	Row const Mul2 = data[2] * Mov2;
	Row const Mul3 = data[3] * Mov3;
	Row const Add1 = Mul2 + Mul3;
	Row const Add2 = Add0 + Add1;
	return Add2;
}

Matrix4x4 Matrix4x4::operator*(float rhs) const
{
	return Matrix4x4(data[0] * rhs, data[1] * rhs, data[2] * rhs, data[3] * rhs);
}

Matrix4x4 Matrix4x4::operator/(float rhs) const
{
	return Matrix4x4(data[0] / rhs, data[1] / rhs, data[2] / rhs, data[3] / rhs);
}

Vector3 Matrix4x4::operator*(const Vector3& rhs) const
{
	return cast_object_unsafe<Vector3>(cast_object<glm::mat4>(*this) * glm::vec4(rhs.x, rhs.y, rhs.z, 0.0f));
}

Vector2 Matrix4x4::operator*(const Vector2& rhs) const
{
	return cast_object_unsafe<Vector2>(cast_object<glm::mat4>(*this) * glm::vec4(rhs.x, rhs.y, 0.0f, 0.0f));
}

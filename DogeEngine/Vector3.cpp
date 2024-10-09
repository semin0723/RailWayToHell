#include <cmath>
#include "Vector2.h"
#include "Vector3.h"
#include "Math.h"

Vector3::Vector3()
{
	x = 0, y = 0, z = 0;
}
Vector3::Vector3(float a, float b, float c)
{
	x = a, y = b, z = c;
}

Vector3::Vector3(Vector2 vec)
{
	x = vec.x, y = vec.y, z = 0;
}

Vector3 Vector3::operator-() const
{
	return Vector3{ -x, -y, -z };
}

Vector3 Vector3::operator+(const Vector3& vec) const
{
	return Vector3{ x + vec.x, y + vec.y, z + vec.z };
}
Vector3 Vector3::operator+(const Vector2& vec) const
{
	return Vector3{ x + vec.x, y + vec.y, z };
}
Vector3 Vector3::operator+(const int val) const
{
	return Vector3{ x + val, y + val, z + val };
}
Vector3 Vector3::operator+(const float val) const
{
	return Vector3{ x + val, y + val, z + val };
}

Vector3 Vector3::operator-(const Vector3& vec) const
{
	return Vector3{ x - vec.x, y - vec.y, z - vec.z };
}
Vector3 Vector3::operator-(const Vector2& vec) const
{
	return Vector3{ x - vec.x, y - vec.y, z };
}
Vector3 Vector3::operator-(const int val) const
{
	return Vector3{ x - val, y - val, z - val };
}
Vector3 Vector3::operator-(const float val) const
{
	return Vector3{ x - val, y - val, z - val };
}

Vector3 Vector3::operator*(const Vector3& vec) const
{
	return Vector3{ x * vec.x, y * vec.y, z * vec.z };
}

Vector3 Vector3::operator*(const Vector2& vec) const
{
	return Vector3{ x * vec.x, y * vec.y, z };
}

Vector3 Vector3::operator*(const int mult) const
{
	return Vector3{ x * mult, y * mult, z * mult };
}
Vector3 Vector3::operator*(const float mult) const
{
	return Vector3{ x * mult, y * mult, z * mult };
}

Vector3 Vector3::operator/(const int divide) const
{
	return Vector3{ x / divide, y / divide, z / divide };
}

Vector3 Vector3::operator/(const float divide) const
{
	return Vector3{ x / divide, y / divide, z / divide };
}

Vector3& Vector3::operator+=(const Vector3& vec)
{
	x += vec.x;
	y += vec.y;
	z += vec.z;
	return *this;
}
Vector3& Vector3::operator-=(const Vector3& vec)
{
	x -= vec.x;
	y -= vec.y;
	z -= vec.z;
	return *this;
}
Vector3& Vector3::operator*=(const Vector3& vec)
{
	x *= vec.x;
	y *= vec.y;
	z *= vec.z;
	return *this;
}
Vector3& Vector3::operator/=(const Vector3& vec)
{
	x /= vec.x;
	y /= vec.y;
	z /= vec.z;
	return *this;
}

bool Vector3::operator==(const Vector3& vec) const
{
	if (x == vec.x && y == vec.y && z == vec.z)
	{
		return true;
	}
	return false;
}

Vector3 Vector3::CrossProduct(const Vector3& vec) const
{
	float n_x = y * vec.z - z * vec.y;
	float n_y = z * vec.x - x * vec.z;
	float n_z = x * vec.y - y * vec.x;

	return Vector3{ n_x, n_y, n_z };
}
float Vector3::DotProduct(const Vector3& vec) const
{
	return (x * vec.x) + (y * vec.y) + (z * vec.z);
}

Vector3 Vector3::Reflect(const Vector3& normal) const
{
	Vector3 project = normal * Vector3{ -x, -y, -z }.DotProduct(normal);
	Vector3 reflect = Vector3{ x, y, z } + (2 * project);
	return reflect;
}

float Vector3::Magnitude() const
{
	return sqrt((x * x) + (y * y) + (z * z));
	//return Math::GetSqrtFast((x * x) + (y * y) + (z * z));
}

float Vector3::MagnitudeSquare() const
{
	return (x * x) + (y * y) + (z * z);
}

Vector2 Vector3::ToVector2() const
{
	return Vector2{ x, y };
}

Vector3 Vector3::Normalize() const
{
	float magnitude = Magnitude();
	// 변수가 분모로 올 때는 항상 0으로 나누지 못 하도록 주의하자.
	if (Math::Approximate(magnitude, 0.0f))
	{
		return Vector3{ 0, 0, 0 };
	}
	return Vector3(x / magnitude, y / magnitude, z / magnitude);
}

Vector3 Vector3::Right()
{
	return Vector3(1, 0, 0);
}

Vector3 Vector3::Up()
{
	return Vector3(0, 1, 0);
}

Vector3 Vector3::Down()
{
	return Vector3(0, -1, 0);
}

Vector3 Vector3::Left()
{
	return Vector3(-1, 0, 0);
}

Vector3 Vector3::One()
{
	return Vector3(1, 1, 1);
}

Vector3 Vector3::Zero()
{
	return Vector3(0, 0, 0);
}

Vector3 Vector3::Rotate90() const
{
	return Vector3(y, -x, z);
}

Vector3 Vector3::Rotate180() const
{
	return Vector3(-x, -y, z);
}

Vector3 Vector3::Rotate270() const
{
	return Vector3(-y, x, z);
}

Vector3 Vector3::Rotate(float radian) const
{
	// D2DMatrix::Rotation이 시계방향으로 회전하므로 여기서도 시계방향으로 회전
	// 반시계는 cosX - sinX, sinX + cosX
	// 시계는 cosX + sinX, -sinX + cosX
	float cosX = cos(radian);
	float sinX = sin(radian);
	return Vector3(x * cosX + y * sinX, x * -sinX + y * cosX, 0);
}
Vector3 Vector3::RotateAt(Vector3 anchor, float radian) const
{
	Vector3 pos = Vector3{ x, y, z } - anchor;
	pos = pos.Rotate(radian);
	return pos + anchor;
}
Vector3 Vector3::Reverse() const
{
	return Vector3(-x, -y, -z);
}

Vector3 Vector3::Lerp(Vector3 start, Vector3 end, float p)
{
	if (p < 0) p = 0;
	else if (p > 1) p = 1;

	Vector3 vec = end - start;
	return start + (vec * p);
}

Vector3 Vector3::SmoothDamp(Vector3 current, Vector3 target, Vector3& currentVelocity, float smoothTime, float maxSpeed)
{
	float x = Math::SmoothDamp(current.x, target.x, currentVelocity.x, smoothTime, maxSpeed);
	float y = Math::SmoothDamp(current.y, target.y, currentVelocity.y, smoothTime, maxSpeed);
	float z = Math::SmoothDamp(current.z, target.z, currentVelocity.z, smoothTime, maxSpeed);

	return Vector3{ x, y, z };
}


Vector3 operator+(const int val, Vector3 v)
{
	return v + val;
}

Vector3 operator+(const float val, Vector3 v)
{
	return v + val;
}

Vector3 operator-(const int val, Vector3 v)
{
	return v - val;
}

Vector3 operator-(const float val, Vector3 v)
{
	return v - val;
}

Vector3 operator*(const int mult, Vector3 v)
{
	return v * mult;
}
Vector3 operator*(const float mult, Vector3 v)
{
	return v * mult;
}
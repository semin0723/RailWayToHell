#include <cmath>
#include "Vector2.h"
#include "Vector3.h"
#include "Math.h"

Vector2::Vector2()
{
	x = 0;
	y = 0;
}
Vector2::Vector2(float a, float b)
{
	x = a;
	y = b;
}

Vector2::Vector2(Vector3 vec)
{
	x = vec.x;
	y = vec.y;
}

Vector2 Vector2::operator-()
{
	return Vector2{ -x, -y };
}

Vector2 Vector2::operator+(const Vector2& vec) const
{
	return Vector2{ x + vec.x, y + vec.y };
}
Vector3 Vector2::operator+(const Vector3& vec) const
{
	return Vector3{ x + vec.x, y + vec.y, vec.z };
}
Vector2 Vector2::operator+(const int val) const
{
	return Vector2{ x + val, y + val };
}
Vector2 Vector2::operator+(const float val) const
{
	return Vector2{ x + val, y + val };
}
Vector2 Vector2::operator-(const Vector2& vec) const
{
	return Vector2{ x - vec.x, y - vec.y };
}
Vector3 Vector2::operator-(const Vector3& vec) const
{
	return Vector3{ x - vec.x, y - vec.y, vec.z };
}
Vector2 Vector2::operator-(const int val) const
{
	return Vector2{ x - val, y - val };
}
Vector2 Vector2::operator-(const float val) const
{
	return Vector2{ x - val, y - val };
}
Vector2 Vector2::operator*(const Vector2& vec) const
{
	return Vector2{ x * vec.x, y * vec.y };
}
Vector3 Vector2::operator*(const Vector3& vec) const
{
	return Vector2{ x * vec.x, y * vec.y };
}
Vector2 Vector2::operator*(const int mult) const
{
	return Vector2{ x * mult, y * mult };
}
Vector2 Vector2::operator*(const float mult) const
{
	return Vector2{ x * mult, y * mult };
}
Vector2 Vector2::operator/(const int divide) const
{
	return Vector2{ x / divide, y / divide };
}
Vector2 Vector2::operator/(const float divide) const
{
	return Vector2{ x / divide, y / divide };
}
Vector2& Vector2::operator+=(const Vector2& vec)
{
	x += vec.x;
	y += vec.y;
	return *this;
}
Vector2& Vector2::operator-=(const Vector2& vec)
{
	x -= vec.x;
	y -= vec.y;
	return *this;
}
Vector2& Vector2::operator*=(const Vector2& vec)
{
	x *= vec.x;
	y *= vec.y;
	return *this;
}
Vector2& Vector2::operator/=(const Vector2& vec)
{
	x /= vec.x;
	y /= vec.y;
	return *this;
}
bool Vector2::operator==(const Vector2& vec) const
{
	if (x == vec.x && y == vec.y)
	{
		return true;
	}
	return false;
}
float Vector2::DotProduct(const Vector2& vec) const
{
	return (x * vec.x) + (y * vec.y);
}
Vector2 Vector2::Reflect(const Vector2& normal) const
{
	Vector2 project = normal * Vector2{ -x, -y }.DotProduct(normal);
	Vector2 reflect = Vector2{ x, y } + (2 * project);
	return reflect;
}
float Vector2::Magnitude() const
{
	return sqrt((x * x) + (y * y));
	//return Math::GetSqrtFast((x * x) + (y * y));
}

float Vector2::MagnitudeSquare() const
{
	return (x * x) + (y * y);
}

Vector3 Vector2::ToVector3() const
{
	return Vector3{ x, y, 0 };
}

Vector2 Vector2::Normalize() const
{
	float magnitude = Magnitude();
	// 변수가 분모로 올 때는 항상 0으로 나누지 못 하도록 주의하자.
	if (Math::Approximate(magnitude, 0.0f))
	{
		return Vector2{ 0, 0 };
	}
	return Vector2(x / magnitude, y / magnitude);
}

Vector2 Vector2::Right()
{
	return Vector2(1, 0);
}

Vector2 Vector2::Up()
{
	return Vector2(0, 1);
}

Vector2 Vector2::Down()
{
	return Vector2(0, -1);
}

Vector2 Vector2::Left()
{
	return Vector2(-1, 0);
}
Vector2 Vector2::One()
{
	return Vector2(1, 1);
}
Vector2 Vector2::Zero()
{
	return Vector2(0, 0);
}
Vector2 Vector2::Rotate90() const
{
	return Vector2(y, -x);
}
Vector2 Vector2::Rotate180() const
{
	return Vector2(-x, -y);
}
Vector2 Vector2::Rotate270() const
{
	return Vector2(-y, x);
}
Vector2 Vector2::Rotate(float radian) const
{
	// D2DMatrix::Rotation이 시계방향으로 회전하므로 여기서도 시계방향으로 회전
	// 반시계는 cosX - sinX, sinX + cosX
	// 시계는 cosX + sinX, -sinX + cosX
	float cosX = cos(radian);
	float sinX = sin(radian);
	return Vector2(x * cosX + y * sinX, x * -sinX + y * cosX);
}

Vector2 Vector2::RotateAt(Vector2 anchor, float radian) const
{
	Vector2 pos = Vector2{ x, y } - anchor;
	pos = pos.Rotate(radian);
	return pos + anchor;
}

Vector2 Vector2::Reverse() const
{
	return Vector2(-x, -y);
}

Vector2 Vector2::Lerp(Vector2 start, Vector2 end, float p)
{
	if (p < 0) p = 0;
	else if (p > 1) p = 1;

	Vector2 vec = end - start;
	return start + (vec * p);
}

Vector2 Vector2::SmoothDamp(Vector2 current, Vector2 target, Vector2& currentVelocity, float smoothTime, float maxSpeed)
{
	float x = Math::SmoothDamp(current.x, target.x, currentVelocity.x, smoothTime, maxSpeed);
	float y = Math::SmoothDamp(current.y, target.y, currentVelocity.y, smoothTime, maxSpeed);

	return Vector2{ x, y };
}

Vector2 operator+(const int val, Vector2 v)
{
	return v + val;
}

Vector2 operator+(const float val, Vector2 v)
{
	return v + val;
}

Vector2 operator-(const int val, Vector2 v)
{
	return v - val;
}

Vector2 operator-(const float val, Vector2 v)
{
	return v - val;
}

Vector2 operator*(const int mult, Vector2 v)
{
	return v * mult;
}
Vector2 operator*(const float mult, Vector2 v)
{
	return v * mult;
}
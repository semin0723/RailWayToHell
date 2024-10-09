#pragma once

struct Vector3;

struct Vector2
{
	float x;
	float y;

	Vector2();
	Vector2(float x, float y);
	Vector2(Vector3 vec); // 상호 호환을 위한 변환 생성자

	Vector2 operator-();

	Vector2 operator+(const Vector2& vec) const;
	Vector3 operator+(const Vector3& vec) const;
	Vector2 operator+(const int val) const;
	Vector2 operator+(const float val) const;

	Vector2 operator-(const Vector2& vec) const;
	Vector3 operator-(const Vector3& vec) const;
	Vector2 operator-(const int val) const;
	Vector2 operator-(const float val) const;

	Vector2 operator*(const Vector2& vec) const;
	Vector3 operator*(const Vector3& vec) const;
	Vector2 operator*(const int mult) const;
	Vector2 operator*(const float mult) const;

	Vector2 operator/(const int divide) const;
	Vector2 operator/(const float divide) const;

	Vector2& operator+=(const Vector2& vec);
	Vector2& operator-=(const Vector2& vec);
	Vector2& operator*=(const Vector2& vec);
	Vector2& operator/=(const Vector2& vec);

	bool operator==(const Vector2& vec) const;

	float DotProduct(const Vector2& vec) const;
	Vector2 Reflect(const Vector2& normal) const;

	float Magnitude() const;
	float MagnitudeSquare() const;

	Vector3 ToVector3() const;
	Vector2 Normalize() const;

	static Vector2 Right();
	static Vector2 Up();
	static Vector2 Down();
	static Vector2 Left();
	static Vector2 One();
	static Vector2 Zero();
	Vector2 Rotate90() const;
	Vector2 Rotate180() const;
	Vector2 Rotate270() const;

	Vector2 Rotate(float radian) const;
	Vector2 RotateAt(Vector2 anchor, float radian) const;
	Vector2 Reverse() const;

	// p : 진행도 (0 ~ 1)
	static Vector2 Lerp(Vector2 start, Vector2 end, float p);
	static Vector2 SmoothDamp(Vector2 current, Vector2 target, Vector2& currentVelocity, float smoothTime, float maxSpeed = 0xFFFFFFFF);
};

Vector2 operator+(const int val, Vector2 v);
Vector2 operator+(const float val, Vector2 v);

Vector2 operator-(const int val, Vector2 v);
Vector2 operator-(const float val, Vector2 v);

Vector2 operator*(const int mult, Vector2 v);
Vector2 operator*(const float mult, Vector2 v);
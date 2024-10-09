#pragma once

struct Vector2;

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3();
	Vector3(float x, float y, float z);
	Vector3(Vector2 vec); // 상호 호환을 위한 변환 생성자

	Vector3 operator-() const;

	Vector3 operator+(const Vector3& vec) const;
	Vector3 operator+(const Vector2& vec) const;
	Vector3 operator+(const int val) const;
	Vector3 operator+(const float val) const;

	Vector3 operator-(const Vector3& vec) const;
	Vector3 operator-(const Vector2& vec) const;
	Vector3 operator-(const int val) const;
	Vector3 operator-(const float val) const;

	Vector3 operator*(const Vector3& vec) const;
	Vector3 operator*(const Vector2& vec) const;
	Vector3 operator*(const int mult) const;
	Vector3 operator*(const float mult) const;

	Vector3 operator/(const int divide) const;
	Vector3 operator/(const float divide) const;

	Vector3& operator+=(const Vector3& vec);
	Vector3& operator-=(const Vector3& vec);
	Vector3& operator*=(const Vector3& vec);
	Vector3& operator/=(const Vector3& vec);

	bool operator==(const Vector3& vec) const;

	Vector3 CrossProduct(const Vector3& vec) const;
	float DotProduct(const Vector3& vec) const;
	Vector3 Reflect(const Vector3& normal) const;

	float Magnitude() const;
	float MagnitudeSquare() const;

	Vector2 ToVector2() const;
	Vector3 Normalize() const;

	static Vector3 Right();
	static Vector3 Up();
	static Vector3 Down();
	static Vector3 Left();
	static Vector3 One();
	static Vector3 Zero();
	Vector3 Rotate90() const;
	Vector3 Rotate180() const;
	Vector3 Rotate270() const;

	Vector3 Rotate(float radian) const;
	Vector3 RotateAt(Vector3 anchor, float radian) const;
	Vector3 Reverse() const;

	// p : 진행도 (0 ~ 1)
	static Vector3 Lerp(Vector3 start, Vector3 end, float p);
	static Vector3 SmoothDamp(Vector3 current, Vector3 target, Vector3& currentVelocity, float smoothTime, float maxSpeed = 0xFFFFFFFF);
};

Vector3 operator+(const int val, Vector3 v);
Vector3 operator+(const float val, Vector3 v);

Vector3 operator-(const int val, Vector3 v);
Vector3 operator-(const float val, Vector3 v);

Vector3 operator*(const int mult, Vector3 v);
Vector3 operator*(const float mult, Vector3 v);
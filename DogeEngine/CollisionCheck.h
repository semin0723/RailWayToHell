#pragma once
#include <vector>
#include "defines.h"
#include "StandardFramework.h"
#include "Vector2.h"
#include "Vector3.h"
#include "ColliderType.h"

#undef rad1
#undef rad2

class Rigidbody2D;
class BoxCollider;
class CircleCollider;
class Collider;

NAMESPACE_DOGEENGINE_BEGIN

struct SATResult
{
	bool isCollided = false;
	float minOverlap = 0.0f;
	Vector2 bestSeparateAxis{ 0, 0 };
};

class CollisionCheck
{
public:
	// 밀어낼 때 CollisionStay 유지되어야 하니까 이만큼 덜 밀어냄
	static constexpr float OVERLAP_EPSILON = 0.00001f;


public:
	static bool CheckBetweenColliders(Collider* rbCollider, Collider* otherCollider);
	static bool SimpleDistanceCheck(Collider* rbCollider, ColliderType rbType, Collider* otherCollider, ColliderType otherType);
private:
	static bool CheckCollider_Box_Box(BoxCollider* box1, BoxCollider* box2);
	static bool CheckCollider_Box_Circle(BoxCollider* box, CircleCollider* circle);
	static bool CheckCollider_Circle_Circle(CircleCollider* circle1, CircleCollider* circle2);



public:
	static bool CheckBetweenColliderAndPoint(Collider* collider, Vector2 point);
private:
	static bool CheckPoint_Box(BoxCollider* box, Vector2 point);
	static bool CheckPoint_Circle(CircleCollider* circle, Vector2 point);



public:
	static bool CheckBetweenColliderAndLine(Collider* collider, Vector2 lineStart, Vector2 lineEnd);
private:
	static bool CheckLine_Box(BoxCollider* box, Vector2 lineStart, Vector2 lineEnd);
	static bool CheckLine_Circle(CircleCollider* circle, Vector2 lineStart, Vector2 lineEnd);


public:
	static bool CheckBetweenColliderAndCircle(Collider* collider, Vector2 center, float radius);
private:



public:
	// Line - Line
	static bool BetweenLinesegAndLineseg(Vector2 line1Start, Vector2 line1End, Vector2 line2Start, Vector2 line2End);
	// 두 반직선이 교차함이 보장된 경우에 교차점을 구하기 위해 사용
	static Vector2 GetIntersectPointOfTwoLine(Vector2 line1Start, Vector2 line1End, Vector2 line2Start, Vector2 line2End);



	// AABox - AABox
	static bool BetweenAxisAlignedBox(Vector2 first_min, Vector2 first_max, Vector2 second_min, Vector2 second_max);
	// AABox - Point
	static bool BetweenAxisAlignedBoxAndPoint(Vector2 min, Vector2 max, Vector2 point);
	// AABox - Line
	static bool BetweenAxisAlignedBoxAndLine(Vector2 min, Vector2 max, Vector2 lineStart, Vector2 lineEnd);


	// OBB - OBB
	// 박스의 포인트들은 시계방향 또는 반시계방향으로 차례대로 전달되어야 합니다. 예컨대 p1이 좌측 상단 포인트라면 p2는 우측 상단 또는 좌측 하단 포인트여야 합니다.
	static SATResult SAT_Box_Box(Vector2 rbPos, std::vector<Vector2>&& rbPoints, Vector2 otherPos, std::vector<Vector2>&& otherPoints);
	// OBB - Circle
	static SATResult SAT_Box_Circle(Vector2 rbPos, std::vector<Vector2>&& rbPoints, Vector3 otherPos, float otherRad);
	static SATResult SAT_Circle_Box(Vector3 rbPos, float rbRad, Vector2 otherPos, std::vector<Vector2>&& otherPoints);


	// OBB - Point
	static bool BetweenOrientedBoxAndPoint(std::vector<Vector2> boxPoints, Vector2 point);
	// OBB - Line
	static bool BetweenOrientedBoxAndLine(std::vector<Vector2> boxPoints, Vector2 lineStart, Vector2 lineEnd);



	// Circle - Circle
	static SATResult SAT_Circle_Circle(Vector3 rbPos, float rbRad, Vector3 otherPos, float otherRad);
	static bool BetweenTwoCircle(Vector3 pos1, float rad1, Vector3 pos2, float rad2);
	// Circle - Point
	static bool BetweenCircleAndPoint(Vector3 circle_pos, float radius, Vector2 point);
	// Circle - Line
	static bool BetweenCircleAndLine(Vector3 circle_pos, float radius, Vector2 lineStart, Vector2 lineEnd);



	// AABox - Circle
	static bool BetweenAxisAlignedBoxAndCircle(Vector2 boxMin, Vector2 boxMax, Vector2 circleCenter, float circleRadius);



	// Triangle
	// 외적을 활용한 삼각형 내부 판별 ~ 외적하는 순서에 따라 외적한 법선 벡터의 방향이 달라지는 점을 이용
	static bool CheckTriangleByOuterProduct(Vector3 v1, Vector3 v2, Vector3 v3, Vector2 point);
	static bool CheckTriangleByOuterProduct(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 point);







	// ---------------- 겹친 영역 구하기 ---------------------

	// movement가 있을 경우
public:
	static float GetOverlapAmountBySAT(Collider* rbCollider, Collider* otherCollider, Vector2 moveDir);
private:
	static float GetOvelapAmountBySAT_Box_Box(Vector2 separateAxis, BoxCollider* rbBox, BoxCollider* other);
	static float GetOvelapAmountBySAT_Box_Circle(Vector2 separateAxis, BoxCollider* rbBox, CircleCollider* other);
	static float GetOvelapAmountBySAT_Circle_Box(Vector2 separateAxis, CircleCollider* rbCircle, BoxCollider* other);
	static float GetOvelapAmountBySAT_Circle_Circle(Vector2 separateAxis, CircleCollider* rbCircle, CircleCollider* other);

	// obsolete
	/*static Vector2 GetOverlapAreaBetweenBoxAndBox(BoxCollider* box1, BoxCollider* box2);
	static Vector2 GetOverlapAreaBetweenBoxAndCircle(BoxCollider* box, CircleCollider* circle);
	static Vector2 GetOverlapAreaBetweenCircleAndCircle(CircleCollider* circle1, CircleCollider* circle2);
	static Vector2 GetOverlapAreaBetweenOBBAndOBB(BoxCollider* box1, BoxCollider* box2);*/

};

NAMESPACE_DOGEENGINE_END
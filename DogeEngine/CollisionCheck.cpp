#include <limits>
#include "CollisionCheck.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include "Object.h"
#include "Math.h"

#undef max

NAMESPACE_DOGEENGINE_BEGIN
bool CollisionCheck::CheckBetweenColliders(Collider* rbCollider, Collider* otherCollider)
{
	ColliderType rbType = rbCollider->GetColliderType();
	ColliderType otherType = otherCollider->GetColliderType();

	if (!SimpleDistanceCheck(rbCollider, rbType, otherCollider, otherType)) return false;

	// 어차피 물리 시뮬에서 겹친 만큼 밀어낼 때 SAT를 사용하기 때문에 충돌 체크를 하면서 동시에 겹친 영역 알아내기 위해 SAT를 쓰는 걸로 통일
	if (rbType == ColliderType::Box)
	{
		if (otherType == ColliderType::Box)
		{
			BoxCollider* rbBox = static_cast<BoxCollider*>(rbCollider);
			BoxCollider* otherBox = static_cast<BoxCollider*>(otherCollider);

			SATResult&& result = SAT_Box_Box(rbBox->GetCenterPosition(), rbBox->GetPoints(), otherBox->GetCenterPosition(), otherBox->GetPoints());
			if (result.isCollided == true)
			{
				rbCollider->bestSeparateAxis = result.bestSeparateAxis;
				rbCollider->minOverlap = result.minOverlap;
				otherCollider->bestSeparateAxis = -result.bestSeparateAxis;
				otherCollider->minOverlap = result.minOverlap;
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (otherType == ColliderType::Circle)
		{
			BoxCollider* rbBox = static_cast<BoxCollider*>(rbCollider);
			CircleCollider* otherCircle = static_cast<CircleCollider*>(otherCollider);

			SATResult&& result = SAT_Box_Circle(rbBox->GetCenterPosition(), rbBox->GetPoints(), otherCircle->GetCenterPosition(), otherCircle->GetRadius());
			if (result.isCollided == true)
			{
				rbCollider->bestSeparateAxis = result.bestSeparateAxis;
				rbCollider->minOverlap = result.minOverlap;
				otherCollider->bestSeparateAxis = -result.bestSeparateAxis;
				otherCollider->minOverlap = result.minOverlap;
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else if (rbType == ColliderType::Circle)
	{
		if (otherType == ColliderType::Box)
		{
			CircleCollider* rbCircle = static_cast<CircleCollider*>(rbCollider);
			BoxCollider* otherBox = static_cast<BoxCollider*>(otherCollider);

			SATResult&& result = SAT_Circle_Box(rbCircle->GetCenterPosition(), rbCircle->GetRadius(), otherBox->GetCenterPosition(), otherBox->GetPoints());
			if (result.isCollided == true)
			{
				rbCollider->bestSeparateAxis = result.bestSeparateAxis;
				rbCollider->minOverlap = result.minOverlap;
				otherCollider->bestSeparateAxis = -result.bestSeparateAxis;
				otherCollider->minOverlap = result.minOverlap;
				return true;
			}
			else
			{
				return false;
			}
		}
		else if (otherType == ColliderType::Circle)
		{
			CircleCollider* rbCircle = static_cast<CircleCollider*>(rbCollider);
			CircleCollider* otherCircle = static_cast<CircleCollider*>(otherCollider);

			SATResult&& result = SAT_Circle_Circle(rbCircle->GetCenterPosition(), rbCircle->GetRadius(), otherCircle->GetCenterPosition(), otherCircle->GetRadius());
			if (result.isCollided == true)
			{
				rbCollider->bestSeparateAxis = result.bestSeparateAxis;
				rbCollider->minOverlap = result.minOverlap;
				otherCollider->bestSeparateAxis = -result.bestSeparateAxis;
				otherCollider->minOverlap = result.minOverlap;
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	return false;
}

bool CollisionCheck::SimpleDistanceCheck(Collider* rbCollider, ColliderType rbType, Collider* otherCollider, ColliderType otherType)
{
	Vector2 rbPos = rbCollider->GetCenterPosition();
	Vector2 otherPos = rbCollider->GetCenterPosition();

	// 매우 간단한 수준의 가지치기, 접촉 판정이 날 수 있는 최장거리보다 더 멀면 체크 생략
	if (rbType == ColliderType::Box)
	{
		if (otherType == ColliderType::Box)
		{
			float d1 = static_cast<BoxCollider*>(rbCollider)->GetDiagonalSquare() / 2.f;
			float d2 = static_cast<BoxCollider*>(otherCollider)->GetDiagonalSquare() / 2.f;
			float dist = (rbPos - otherPos).MagnitudeSquare();
			if (d1 + d2 < dist) return false;
		}
		else if (otherType == ColliderType::Circle)
		{
			float d1 = static_cast<BoxCollider*>(rbCollider)->GetDiagonalSquare() / 2.f;
			float d2 = pow(static_cast<CircleCollider*>(otherCollider)->GetRadius(), 2);
			float dist = (rbPos - otherPos).MagnitudeSquare();
			if (d1 + d2 < dist) return false;
		}
	}
	else if (rbType == ColliderType::Circle)
	{
		if (otherType == ColliderType::Box)
		{
			float d1 = static_cast<BoxCollider*>(otherCollider)->GetDiagonalSquare() / 2.f;
			float d2 = pow(static_cast<CircleCollider*>(rbCollider)->GetRadius(), 2);
			float dist = (rbPos - otherPos).MagnitudeSquare();
			if (d1 + d2 < dist) return false;
		}
		else if (otherType == ColliderType::Circle)
		{
			float d1 = pow(static_cast<CircleCollider*>(rbCollider)->GetRadius(), 2);
			float d2 = pow(static_cast<CircleCollider*>(otherCollider)->GetRadius(), 2);
			float dist = (rbPos - otherPos).MagnitudeSquare();
			if (d1 + d2 < dist) return false;
		}
	}
	return true;
}

bool CollisionCheck::CheckCollider_Box_Box(BoxCollider* box1, BoxCollider* box2)
{
	// obsolete
	//if (Math::Approximate(box1->GetOwner()->transform->GetRotation().z, 0.0f) && Math::Approximate(box2->GetOwner()->transform->GetRotation().z, 0.0f))
	//{
	//	// AABB
	//	return BetweenAxisAlignedBox(box1->GetMinPosition(), box1->GetMaxPosition(), box2->GetMinPosition(), box2->GetMaxPosition());
	//}
	//else
	//{
	//	// OBB
	//	return BetweenOrientedBox(box1->GetPoints(), box2->GetPoints());
	//}
	return false;
}

bool CollisionCheck::CheckCollider_Box_Circle(BoxCollider* box, CircleCollider* circle)
{
	// obsolete
	//float rotation = box->GetOwner()->transform->GetRotation().z;

	//if (Math::Approximate(rotation, 0.0f))
	//{
	//	// AABB - Circle
	//	Vector2 min = box->GetMinPosition();
	//	Vector2 max = box->GetMaxPosition();

	//	Vector3 cirPos = circle->GetOwner()->transform->GetPosition() + circle->GetAnchor();
	//	float rad = circle->GetRadius();

	//	return BetweenAxisAlignedBoxAndCircle(min, max, cirPos.ToVector2(), rad);
	//}
	//else
	//{
	//	// OBB - Circle
	//	// 다양한 방법이 있을 수 있으나, 가장 쉬운 방법은 원은 회전시켜도 변하지 않으므로 사각형이 축에 정렬되도록 회전했을 때의 네 꼭지점을 구한 다음 AABB-Circle Collision detection 알고리즘을 사용하는 것이다
	//	// 사각형이 AABB가 되도록 사각형의 중점을 기준으로 회전했을 때의 circle의 포지션을 구하자.
	//	Vector3 circleCenter = circle->GetOwner()->transform->GetPosition() + circle->GetAnchor();
	//	Vector3 boxCenter = box->GetOwner()->transform->GetPosition();
	//	float rotation = box->GetOwner()->transform->GetRotation().z;
	//	circleCenter = circleCenter.RotateAt(boxCenter, Math::Deg2Rad(-rotation));

	//	Vector2 boxMin = box->GetMinPosition(true);
	//	Vector2 boxMax = box->GetMaxPosition(true);

	//	return BetweenAxisAlignedBoxAndCircle(boxMin, boxMax, circleCenter, circle->GetRadius());
	//}
	return false;
}

bool CollisionCheck::CheckCollider_Circle_Circle(CircleCollider* circle1, CircleCollider* circle2)
{
	// obsolete
	/*Vector3 pos1 = circle1->GetOwner()->transform->GetPosition() + circle1->GetAnchor();
	Vector3 pos2 = circle2->GetOwner()->transform->GetPosition() + circle2->GetAnchor();

	float radius1 = circle1->GetRadius();
	float radius2 = circle2->GetRadius();

	return BetweenTwoCircle(pos1, radius1, pos2, radius2);*/
	return false;
}



bool CollisionCheck::CheckBetweenColliderAndPoint(Collider* collider, Vector2 point)
{
	if (collider->GetColliderType() == ColliderType::Box)
	{
		return CheckPoint_Box(static_cast<BoxCollider*>(collider), point);
	}
	else if (collider->GetColliderType() == ColliderType::Circle)
	{
		return CheckPoint_Circle(static_cast<CircleCollider*>(collider), point);
	}
	return false;
}

bool CollisionCheck::CheckPoint_Box(BoxCollider* box, Vector2 point)
{
	if (Math::Approximate(box->GetOwner()->transform->GetRotation().z, 0.0f))
	{
		Vector2 min = box->GetMinPosition();
		Vector2 max = box->GetMaxPosition();
		return BetweenOrientedBoxAndPoint(box->GetPoints(), point);
		//나중에 곷피기
		//return BetweenAxisAlignedBoxAndPoint(min, max, point);
	}
	else
	{
		return BetweenOrientedBoxAndPoint(box->GetPoints(), point);
	}
}

bool CollisionCheck::CheckPoint_Circle(CircleCollider* circle, Vector2 point)
{
	Vector3 pos = circle->GetOwner()->transform->GetPosition() + circle->GetAnchor();
	float radius = circle->GetRadius();

	return BetweenCircleAndPoint(pos, radius, point);
}



bool CollisionCheck::CheckBetweenColliderAndLine(Collider* collider, Vector2 lineStart, Vector2 lineEnd)
{
	if (collider->GetColliderType() == ColliderType::Box)
	{
		return CheckLine_Box(static_cast<BoxCollider*>(collider), lineStart, lineEnd);
	}
	else if (collider->GetColliderType() == ColliderType::Circle)
	{
		return CheckLine_Circle(static_cast<CircleCollider*>(collider), lineStart, lineEnd);
	}
	return false;
}

bool CollisionCheck::CheckLine_Box(BoxCollider* box, Vector2 lineStart, Vector2 lineEnd)
{
	if (Math::Approximate(box->GetOwner()->transform->GetRotation().z, 0.0f))
	{
		Vector2 min = box->GetMinPosition();
		Vector2 max = box->GetMaxPosition();

		return BetweenAxisAlignedBoxAndLine(min, max, lineStart, lineEnd);
	}
	else
	{
		return BetweenOrientedBoxAndLine(box->GetPoints(), lineStart, lineEnd);
	}
}

bool CollisionCheck::CheckLine_Circle(CircleCollider* circle, Vector2 lineStart, Vector2 lineEnd)
{
	Vector3 pos = circle->GetOwner()->transform->GetPosition() + circle->GetAnchor();
	float radius = circle->GetRadius();

	return BetweenCircleAndLine(pos, radius, lineStart, lineEnd);
}

bool CollisionCheck::CheckBetweenColliderAndCircle(Collider* collider, Vector2 center, float radius)
{
	ColliderType type = collider->GetColliderType();

	if (type == ColliderType::Box)
	{
		BoxCollider* box = static_cast<BoxCollider*>(collider);
		if (Math::Approximate(box->GetOwner()->transform->GetRotation().z, 0.0f))
		{
			Vector2 min = box->GetMinPosition();
			Vector2 max = box->GetMaxPosition();

			return BetweenAxisAlignedBoxAndCircle(min, max, center, radius);
		}
		else
		{
			// OBB - Circle
			// 다양한 방법이 있을 수 있으나, 가장 쉬운 방법은 원은 회전시켜도 변하지 않으므로 사각형이 축에 정렬되도록 회전했을 때의 네 꼭지점을 구한 다음 AABB-Circle Collision detection 알고리즘을 사용하는 것이다
			// 사각형이 AABB가 되도록 사각형의 중점을 기준으로 회전했을 때의 circle의 포지션을 구하자.
			Vector3 boxCenter = box->GetOwner()->transform->GetPosition();
			float rotation = box->GetOwner()->transform->GetRotation().z;
			Vector2 circleCenter = center.RotateAt(boxCenter, Math::Deg2Rad(-rotation));

			Vector2 boxMin = box->GetMinPosition();
			Vector2 boxMax = box->GetMaxPosition();

			return BetweenAxisAlignedBoxAndCircle(boxMin, boxMax, circleCenter, radius);
		}
	}
	else if (type == ColliderType::Circle)
	{
		CircleCollider* circle = static_cast<CircleCollider*>(collider);
		Vector3 colliderCenter = circle->GetOwner()->transform->GetPosition() + circle->GetAnchor();
		float colliderRadius = circle->GetRadius();

		return BetweenTwoCircle(colliderCenter, colliderRadius, center, radius);
	}
}




bool CollisionCheck::BetweenLinesegAndLineseg(Vector2 line1Start, Vector2 line1End, Vector2 line2Start, Vector2 line2End)
{
	// dn은 pn에서 다른 선분의 시작점, 끝점으로 향하는 벡터를 외적했을 때의 z값
	float d1 = (line2Start - line1Start).ToVector3().CrossProduct((line2End - line1Start).ToVector3()).z;
	float d2 = (line2Start - line1End).ToVector3().CrossProduct((line2End - line1End).ToVector3()).z;
	float d3 = (line1Start - line2Start).ToVector3().CrossProduct((line1End - line2Start).ToVector3()).z;
	float d4 = (line1Start - line2End).ToVector3().CrossProduct((line1End - line2End).ToVector3()).z;

	// 같은 선분으로부터 출발한 벡터의 외적인 dn(d1-d2, d3-d4)에 대해, 두 선분이 교차할 경우 두 dn의 부호가 반대가 된다.
	// 반면 두 선분이 교차하지 않는다면, d1-d2 또는 d3-d4 중 적어도 하나는 부호가 같다.
	// 단 두 선분 또는 한 선분과 다른 선분의 한 점이 일직선상에 놓인다면, dn은 0이 된다.
	if (d1 * d2 < 0 && d3 * d4 < 0) return true;
	else return false;
}
Vector2 CollisionCheck::GetIntersectPointOfTwoLine(Vector2 line1Start, Vector2 line1End, Vector2 line2Start, Vector2 line2End)
{
	float line1_a = line1Start.y - line1End.y;
	float line1_b = line1End.x - line1Start.x;
	float line1_c = (line1Start.x * line1End.y) - (line1End.x * line1Start.y);

	float line2_a = line2Start.y - line2End.y;
	float line2_b = line2End.x - line2Start.x;
	float line2_c = (line2Start.x * line2End.y) - (line2End.x * line2Start.y);

	float denominator = line1_a * line2_b - line2_a * line1_b;
	if (std::abs(denominator) > 0.00001f)
	{
		Vector2 result;
		result.x = (line1_b * line2_c - line1_c * line2_b) / denominator;
		result.y = (line1_c * line2_a - line1_a * line2_c) / denominator;

		return result;
	}
}
bool CollisionCheck::BetweenAxisAlignedBox(Vector2 first_min, Vector2 first_max, Vector2 second_min, Vector2 second_max)
{
	if (first_min.x > second_max.x) return false;
	if (first_min.y > second_max.y) return false;
	if (first_max.x < second_min.x) return false;
	if (first_max.y < second_min.y) return false;
	return true;
}
bool CollisionCheck::BetweenAxisAlignedBoxAndPoint(Vector2 min, Vector2 max, Vector2 point)
{
	if (point.x >= min.x && point.x <= max.x && point.y >= min.y && point.y <= max.y)
	{
		return true;
	}
	else {
		return false;
	}
}
bool CollisionCheck::BetweenAxisAlignedBoxAndLine(Vector2 min, Vector2 max, Vector2 lineStart, Vector2 lineEnd)
{
	// 시작점 또는 끝점이 박스의 안에 있을 경우
	if (BetweenAxisAlignedBoxAndPoint(min, max, lineStart)) return true;
	if (BetweenAxisAlignedBoxAndPoint(min, max, lineEnd)) return true;

	// 판단 불가면 네 변의 선분과 교차판정
	// 우측변
	Vector2 start = { max.x, min.y };
	Vector2 end = { max.x, max.y };
	if (BetweenLinesegAndLineseg(start, end, lineStart, lineEnd)) return true;

	// 좌측변
	start = { min.x, min.y };
	end = { min.x, max.y };
	if (BetweenLinesegAndLineseg(start, end, lineStart, lineEnd)) return true;

	// 윗변
	start = { min.x, min.y };
	end = { max.x, min.y };
	if (BetweenLinesegAndLineseg(start, end, lineStart, lineEnd)) return true;

	// 아랫변
	start = { min.x, max.y };
	end = { max.x, max.y };
	if (BetweenLinesegAndLineseg(start, end, lineStart, lineEnd)) return true;

	return false;
}

SATResult CollisionCheck::SAT_Box_Box(Vector2 box1Pos, std::vector<Vector2>&& box1, Vector2 box2Pos, std::vector<Vector2>&& box2)
{
	// <07-17>
	// 원을 제외한 Convex 다각형의 분리축은 면의 방향에서 90도 회전한(-y, x)벡터여야 한다.
	// 근데 직사각형만 90도 회전한 변이 곧 인접한 변이다보니 차이가 없다. 즉 직사각형에 대해서만 90도 회전하는 동작을 생략할 수 있다.

	// 사각형인 경우 마주보는 변이 평행하기 때문에 사각형마다 두 개의 축만 확인하면 된다.
	std::vector<Vector2> axes;
	axes.push_back((box1[1] - box1[0]).Normalize());
	axes.push_back((box1[2] - box1[1]).Normalize());
	axes.push_back((box2[1] - box2[0]).Normalize());
	axes.push_back((box2[2] - box2[1]).Normalize());

	// 충돌이 있을 경우 밀어내는 작업에 쓸 수 있도록 최소 overlap을 가지는 분리축을 콜라이더에 기록해야 한다.
	float minOverlap = std::numeric_limits<float>::max();
	Vector2 bestSeparateAxis;
	for (auto axis : axes)
	{
		std::vector<float> box1results;
		for (auto point : box1)
		{
			box1results.push_back(axis.DotProduct(point));
		}
		std::vector<float> box2results;
		for (auto point : box2)
		{
			box2results.push_back(axis.DotProduct(point));
		}

		float box1min = box1results[0];
		float box1max = box1results[0];
		for (int i = 1; i < 4; ++i)
		{
			box1min = box1min > box1results[i] ? box1results[i] : box1min;
			box1max = box1max < box1results[i] ? box1results[i] : box1max;
		}

		float box2min = box2results[0];
		float box2max = box2results[0];
		for (int i = 1; i < 4; ++i)
		{
			box2min = box2min > box2results[i] ? box2results[i] : box2min;
			box2max = box2max < box2results[i] ? box2results[i] : box2max;
		}

		// 어떤 축으로든 충돌하지 않음이 확인된다면 최소 overlap을 만드는 SA를 기록할 필요도 없이 리턴
		if (box1max < box2min) return { false, 0.0f, {0, 0} };
		if (box2max < box1min) return { false, 0.0f, {0, 0} };


		// 해당 축을 기준으로 충돌한 경우 ~ 최소 overlap을 만드는 SA를 찾아서 기록
		float overlap;
		// 완전히 겹치는 경우 ~ 양 쪽으로 체크해봐야 함
		if (box1min <= box2min && box2max <= box1max)
		{
			float overlap_left = box2max - box1min;
			float overlap_right = box1max - box2min;
			overlap = overlap_left <= overlap_right ? overlap_left : overlap_right;
		}
		else if (box2min <= box1min && box1max <= box2max)
		{
			float overlap_left = box1max - box2min;
			float overlap_right = box2max - box1min;
			overlap = overlap_left <= overlap_right ? overlap_left : overlap_right;
		}
		// 일부가 겹치는 경우
		// 완전히 겹치는 경우를 먼저 확인하므로 아래와 같이 간단하게 조건식 작성이 가능하다
		else if (box1min <= box2min && box2min <= box1max)
		{
			overlap = box1max - box2min;
		}
		else if (box2min <= box1min && box1min <= box2max)
		{
			overlap = box2max - box1min;
		}

		if (minOverlap > overlap)
		{
			minOverlap = overlap;
			bestSeparateAxis = axis;
		}
	}

	// 마지막으로 밀어낼 방향을 결정해야 하므로 최소 overlap을 보장하는 분리축을 기준으로 두 박스의 중점의 상대 위치를 구한다.
	float box1Dot = bestSeparateAxis.DotProduct(box1Pos);
	float box2Dot = bestSeparateAxis.DotProduct(box2Pos);
	// 기준(rbCollider)이 분리축 수직선상에서 뒤에 있다면 분리축 방향으로 밀고, 앞에 있다면 반대 방향으로 밀어야 한다.
	if (box1Dot < box2Dot) bestSeparateAxis = -bestSeparateAxis;

	return SATResult{ true, minOverlap, bestSeparateAxis };
}

SATResult CollisionCheck::SAT_Box_Circle(Vector2 boxPos, std::vector<Vector2>&& boxPoints, Vector3 circlePos, float radius)
{
	// 사각형인 경우 마주보는 변이 평행하기 때문에 사각형마다 두 개의 축만 확인하면 된다.
	std::vector<Vector2> axes;
	axes.push_back((boxPoints[1] - boxPoints[0]).Normalize());
	axes.push_back((boxPoints[2] - boxPoints[1]).Normalize());
	axes.push_back((boxPos - circlePos).Normalize());

	// 충돌이 있을 경우 밀어내는 작업에 쓸 수 있도록 최소 overlap을 가지는 분리축을 콜라이더에 기록해야 한다.
	float minOverlap = std::numeric_limits<float>::max();
	Vector2 bestSeparateAxis;
	for (auto axis : axes)
	{
		std::vector<float> boxResults;
		for (auto point : boxPoints)
		{
			boxResults.push_back(axis.DotProduct(point));
		}
		float boxMin = boxResults[0];
		float boxMax = boxResults[0];
		for (int i = 1; i < 4; ++i)
		{
			boxMin = boxMin > boxResults[i] ? boxResults[i] : boxMin;
			boxMax = boxMax < boxResults[i] ? boxResults[i] : boxMax;
		}

		float circleDot = axis.DotProduct(circlePos);
		float circleMin = circleDot - radius;
		float circleMax = circleDot + radius;

		// 어떤 축으로든 충돌하지 않음이 확인된다면 최소 overlap을 만드는 SA를 기록할 필요도 없이 리턴
		if (boxMax < circleMin) return { false, 0.0f, {0, 0} };
		if (circleMax < boxMin) return { false, 0.0f, {0, 0} };


		// 해당 축을 기준으로 충돌한 경우 ~ 최소 overlap을 만드는 SA를 찾아서 기록
		float overlap;
		// 완전히 겹치는 경우 ~ 양 쪽으로 체크해봐야 함
		if (boxMin <= circleMin && circleMax <= boxMax)
		{
			float overlap_left = circleMax - boxMin;
			float overlap_right = boxMax - circleMin;
			overlap = overlap_left <= overlap_right ? overlap_left : overlap_right;
		}
		else if (circleMin <= boxMin && boxMax <= circleMax)
		{
			float overlap_left = boxMax - circleMin;
			float overlap_right = circleMax - boxMin;
			overlap = overlap_left <= overlap_right ? overlap_left : overlap_right;
		}
		// 일부가 겹치는 경우
		// 완전히 겹치는 경우를 먼저 확인하므로 아래와 같이 간단하게 조건식 작성이 가능하다
		else if (boxMin <= circleMin && circleMin <= boxMax)
		{
			overlap = boxMax - circleMin;
		}
		else if (circleMin <= boxMin && boxMin <= circleMax)
		{
			overlap = circleMax - boxMin;
		}

		if (minOverlap > overlap)
		{
			minOverlap = overlap;
			bestSeparateAxis = axis;
		}
	}

	// 마지막으로 밀어낼 방향을 결정해야 하므로 최소 overlap을 보장하는 분리축을 기준으로 두 박스의 중점의 상대 위치를 구한다.
	float boxDot = bestSeparateAxis.DotProduct(boxPos);
	float circleDot = bestSeparateAxis.DotProduct(circlePos);
	// 기준(rbCollider)이 분리축 수직선상에서 뒤에 있다면 분리축 방향으로 밀고, 앞에 있다면 반대 방향으로 밀어야 한다.
	if (boxDot < circleDot) bestSeparateAxis = -bestSeparateAxis;

	return SATResult{ true, minOverlap, bestSeparateAxis };
}

SATResult CollisionCheck::SAT_Circle_Box(Vector3 circlePos, float radius, Vector2 boxPos, std::vector<Vector2>&& boxPoints)
{
	// 사각형인 경우 마주보는 변이 평행하기 때문에 사각형마다 두 개의 축만 확인하면 된다.
	std::vector<Vector2> axes;
	axes.push_back((boxPoints[1] - boxPoints[0]).Normalize());
	axes.push_back((boxPoints[2] - boxPoints[1]).Normalize());
	axes.push_back((circlePos - boxPos).Normalize());

	// 충돌이 있을 경우 밀어내는 작업에 쓸 수 있도록 최소 overlap을 가지는 분리축을 콜라이더에 기록해야 한다.
	float minOverlap = std::numeric_limits<float>::max();
	Vector2 bestSeparateAxis;
	for (auto axis : axes)
	{
		std::vector<float> boxResults;
		for (auto point : boxPoints)
		{
			boxResults.push_back(axis.DotProduct(point));
		}
		float boxMin = boxResults[0];
		float boxMax = boxResults[0];
		for (int i = 1; i < 4; ++i)
		{
			boxMin = boxMin > boxResults[i] ? boxResults[i] : boxMin;
			boxMax = boxMax < boxResults[i] ? boxResults[i] : boxMax;
		}

		float circleDot = axis.DotProduct(circlePos);
		float circleMin = circleDot - radius;
		float circleMax = circleDot + radius;

		// 어떤 축으로든 충돌하지 않음이 확인된다면 최소 overlap을 만드는 SA를 기록할 필요도 없이 리턴
		if (boxMax < circleMin) return { false, 0.0f, {0, 0} };
		if (circleMax < boxMin) return { false, 0.0f, {0, 0} };


		// 해당 축을 기준으로 충돌한 경우 ~ 최소 overlap을 만드는 SA를 찾아서 기록
		float overlap = 0.0f;
		// 완전히 겹치는 경우 ~ 양 쪽으로 체크해봐야 함
		if (boxMin <= circleMin && circleMax <= boxMax)
		{
			float overlap_left = circleMax - boxMin;
			float overlap_right = boxMax - circleMin;
			overlap = overlap_left <= overlap_right ? overlap_left : overlap_right;
		}
		else if (circleMin <= boxMin && boxMax <= circleMax)
		{
			float overlap_left = boxMax - circleMin;
			float overlap_right = circleMax - boxMin;
			overlap = overlap_left <= overlap_right ? overlap_left : overlap_right;
		}
		// 일부가 겹치는 경우
		// 완전히 겹치는 경우를 먼저 확인하므로 아래와 같이 간단하게 조건식 작성이 가능하다
		else if (boxMin <= circleMin && circleMin <= boxMax)
		{
			overlap = boxMax - circleMin;
		}
		else if (circleMin <= boxMin && boxMin <= circleMax)
		{
			overlap = circleMax - boxMin;
		}

		if (minOverlap > overlap)
		{
			minOverlap = overlap;
			bestSeparateAxis = axis;
		}
	}

	// 마지막으로 밀어낼 방향을 결정해야 하므로 최소 overlap을 보장하는 분리축을 기준으로 두 박스의 중점의 상대 위치를 구한다.
	float circleDot = bestSeparateAxis.DotProduct(circlePos);
	float boxDot = bestSeparateAxis.DotProduct(boxPos);
	// 기준(rbCollider)이 분리축 수직선상에서 뒤에 있다면 분리축 방향으로 밀고, 앞에 있다면 반대 방향으로 밀어야 한다.
	if (circleDot < boxDot) bestSeparateAxis = -bestSeparateAxis;

	return SATResult{ true, minOverlap, bestSeparateAxis };
}

bool CollisionCheck::BetweenOrientedBoxAndPoint(std::vector<Vector2> boxPoints, Vector2 point)
{
	// 분리축 이론의 응용. 점이 두 분리축에서 모두 min ~ max 사이에 있다면 사각형의 내부에 있는 것.
	std::vector<Vector2> axes;
	axes.push_back((boxPoints[1] - boxPoints[0]).Normalize());
	axes.push_back((boxPoints[2] - boxPoints[1]).Normalize());

	for (auto axis : axes)
	{
		std::vector<float> result;
		for (auto boxPoint : boxPoints)
		{
			result.push_back(axis.DotProduct(boxPoint));
		}
		float boxMin = result[0];
		float boxMax = result[0];
		for (int i = 1; i < 4; ++i)
		{
			boxMin = boxMin > result[i] ? result[i] : boxMin;
			boxMax = boxMax < result[i] ? result[i] : boxMax;
		}

		float dot = axis.DotProduct(point);

		if (boxMin > dot) return false;
		if (boxMax < dot) return false;
	}

	return true;
}

bool CollisionCheck::BetweenOrientedBoxAndLine(std::vector<Vector2> boxPoints, Vector2 lineStart, Vector2 lineEnd)
{
	// AABB - Line과 똑같다.
	// 시작점 또는 끝점이 박스의 안에 있을 경우
	if (BetweenOrientedBoxAndPoint(boxPoints, lineStart)) return true;
	if (BetweenOrientedBoxAndPoint(boxPoints, lineEnd)) return true;

	// 판단 불가면 네 변의 선분과 교차판정
	for (int i = 0; i < 3; ++i)
	{
		if (BetweenLinesegAndLineseg(boxPoints[i], boxPoints[i + 1], lineStart, lineEnd)) return true;
	}
	if (BetweenLinesegAndLineseg(boxPoints[4], boxPoints[0], lineStart, lineEnd)) return true;

	return false;
}

/*bool CollisionCheck::BetweenOrientedBoxAndCircle(std::vector<Vector2> boxPoints, Vector3 circlePos, float radius)
{

}*/




SATResult CollisionCheck::SAT_Circle_Circle(Vector3 pos1, float rad1, Vector3 pos2, float rad2)
{
	Vector2 axis = (pos1 - pos2).Normalize();

	float c1Dot = axis.DotProduct(pos1);
	float c1Min = c1Dot - rad1;
	float c1Max = c1Dot + rad1;

	float c2Dot = axis.DotProduct(pos2);
	float c2Min = c2Dot - rad2;
	float c2Max = c2Dot + rad2;

	// 어떤 축으로든 충돌하지 않음이 확인된다면 최소 overlap을 만드는 SA를 기록할 필요도 없이 리턴
	if (c1Max < c2Min) return { false, 0.0f, {0, 0} };
	if (c2Max < c1Min) return { false, 0.0f, {0, 0} };

	float overlap = 0.0f;
	if (c1Min <= c2Min && c2Max <= c1Max)
	{
		float overlap_left = c2Max - c1Min;
		float overlap_right = c1Max - c2Min;
		overlap = overlap_left <= overlap_right ? overlap_left : overlap_right;
	}
	else if (c2Min <= c1Min && c1Max <= c2Max)
	{
		float overlap_left = c1Max - c2Min;
		float overlap_right = c2Max - c1Min;
		overlap = overlap_left <= overlap_right ? overlap_left : overlap_right;
	}
	// 일부가 겹치는 경우
	// 완전히 겹치는 경우를 먼저 확인하므로 아래와 같이 간단하게 조건식 작성이 가능하다
	else if (c1Min <= c2Min && c2Min <= c1Max)
	{
		overlap = c1Max - c2Min;
	}
	else if (c2Min <= c1Min && c1Min <= c2Max)
	{
		overlap = c2Max - c1Min;
	}

	// 마지막으로 밀어낼 방향을 결정해야 하므로 최소 overlap을 보장하는 분리축을 기준으로 두 박스의 중점의 상대 위치를 구한다.
	// 기준(rbCollider)이 분리축 수직선상에서 뒤에 있다면 분리축 방향으로 밀고, 앞에 있다면 반대 방향으로 밀어야 한다.
	// 근데 이건 박스의 변을 분리축으로 삼을 때의 얘기지 Circle과 Circle의 SAT에선 반드시 rbCollider(pos1)으로 향하는 벡터가 axis가 되기 때문에 없어도 된다. 그래도 혹시 모르니까 안전장치로...
	if (c1Dot < c2Dot)
	{
		axis = -axis;
	}

	return SATResult{ true, overlap, axis };
}

bool CollisionCheck::BetweenTwoCircle(Vector3 pos1, float rad1, Vector3 pos2, float rad2)
{
	float dist = (pos2 - pos1).MagnitudeSquare();
	float radSum = rad1 + rad2;
	if (dist <= radSum * radSum) return true;
	else return false;
}
bool CollisionCheck::BetweenCircleAndPoint(Vector3 circle_pos, float radius, Vector2 point)
{
	float dist = (point - circle_pos.ToVector2()).MagnitudeSquare();
	if (radius * radius >= dist) return true;
	return false;
}
bool CollisionCheck::BetweenCircleAndLine(Vector3 circle_pos, float radius, Vector2 lineStart, Vector2 lineEnd)
{
	// <24-05-08>
	// 이 부분 노트쉘프 낙서장에 기록해둔 메모 참고해서 다시 정리할 것

	// 선분의 시작점 또는 끝점이 원 내부에 있을 때
	if ((lineStart - circle_pos.ToVector2()).Magnitude() <= radius) return true;
	if ((lineEnd - circle_pos.ToVector2()).Magnitude() <= radius) return true;

	// 판별식을 이용해 원과 선분의 교차판정
	Vector2 line = lineEnd - lineStart;
	Vector2 centerToStart = lineStart - circle_pos.ToVector2();
	// 이차 방정식의 계수 구하기 : a*t^2 + b*t + c = 0
	float a = line.x * line.x + line.y * line.y;
	float b = 2 * (centerToStart.x * line.x + centerToStart.y * line.y);
	float c = (centerToStart.x * centerToStart.x + centerToStart.y * centerToStart.y) - radius * radius;
	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) return false;

	discriminant = std::sqrt(discriminant);
	//discriminant = Math::GetSqrtFast(discriminant);
	float t1 = (-b - discriminant) / (2 * a);
	float t2 = (-b + discriminant) / (2 * a);

	return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}
bool CollisionCheck::BetweenAxisAlignedBoxAndCircle(Vector2 boxMin, Vector2 boxMax, Vector2 circleCenter, float circleRadius)
{
	// circle이 박스의 십자 연장선에 포함될 경우
	if ((boxMin.x <= circleCenter.x && boxMax.x >= circleCenter.x) || (boxMin.y <= circleCenter.y && boxMax.y >= circleCenter.y))
	{
		Vector2 modifiedBoxMin = { boxMin.x - circleRadius, boxMin.y - circleRadius };
		Vector2 modifiedBoxMax = { boxMax.x + circleRadius, boxMax.y + circleRadius };

		if (CollisionCheck::BetweenAxisAlignedBoxAndPoint(modifiedBoxMin, modifiedBoxMax, circleCenter))
		{
			return true;
		}
		else return false;
	}
	// circle이 박스의 십자 연장선에 포함되지 않을 경우
	else
	{
		Vector2 leftTop = { boxMin.x, boxMin.y };
		Vector2 rightTop = { boxMax.x, boxMin.y };
		Vector2 leftBottom = { boxMin.x, boxMax.y };
		Vector2 rightBottom = { boxMax.x, boxMax.y };
		if (CollisionCheck::BetweenCircleAndPoint(circleCenter.ToVector3(), circleRadius, leftTop)) return true;
		if (CollisionCheck::BetweenCircleAndPoint(circleCenter.ToVector3(), circleRadius, rightTop)) return true;
		if (CollisionCheck::BetweenCircleAndPoint(circleCenter.ToVector3(), circleRadius, leftBottom)) return true;
		if (CollisionCheck::BetweenCircleAndPoint(circleCenter.ToVector3(), circleRadius, rightBottom)) return true;
		return false;
	}
}
bool CollisionCheck::CheckTriangleByOuterProduct(Vector3 v1, Vector3 v2, Vector3 v3, Vector2 point)
{
	bool isInside = true;

	Vector3 vt = { point.x, point.y, 0 };
	if ((v2 - v1).CrossProduct(vt - v1).z > 0) isInside = false;
	if ((v3 - v2).CrossProduct(vt - v2).z > 0) isInside = false;
	if ((v1 - v3).CrossProduct(vt - v3).z > 0) isInside = false;

	return isInside;
}
bool CollisionCheck::CheckTriangleByOuterProduct(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 point)
{
	Vector3 v1 = { p1.x, p1.y, 0 };
	Vector3 v2 = { p2.x, p2.y, 0 };
	Vector3 v3 = { p3.x, p3.y, 0 };

	return CheckTriangleByOuterProduct(v1, v2, v3, point);
}










float CollisionCheck::GetOverlapAmountBySAT(Collider* rbCollider, Collider* otherCollider, Vector2 moveDir)
{
	// Rigidbody의 movement 방향을 분리축으로 삼아 SAT를 진행하면 해당 방향으로의 overlap을 구하는 게 가능하다. (AABB간의 overlap을 구하는 경우에도 현재는 중심 앵커와 x, y 축을 기준으로 한 overlap으로 분기하는 엄밀하지 않은 방식이므로, 이 방식을 사용하는 것이 좋다) 이 때 movement은 '현재 fixedUpdate에서의 포지션 - 이전 fixedUpdate에서의 포지션'으로 구할 경우 Rigidbody2D를 통한 이동과 Transform.Translate를 통한 이동을 모두 처리할 수 있다.
	// movement를 그렇게 구했다면, 이 때 서로 충돌하는 두 콜라이더는 반드시 movement 방향을 분리축으로 삼았을 때 overlap을 구할 수 있게 된다. 
	// Rigidbody2D는 매 FixedUpdate마다 오브젝트를 움직이게 하는 정도인 velocity 벡터와 충돌 체크에서 사용할 movement를 만들 때 사용할 prevPosition 벡터를 가지게 된다.
	// curPosition - prevPosition으로 충돌 체크에서 사용하는 movement 벡터를 구할 수 있고, 이 때 curPosition은 Rigidbody2D.MoveObject()를 호출한 뒤여야 한다. 그럼 movement 벡터는 두 FixedUpdate 프레임 간의 포지션의 차, 즉 현재 프레임에서의 이동거리가 되므로, 충돌 후 겹친 overlap 양만큼 밀어내는 것뿐만 아니라 튕겨져 나갈 때 원래 있어야 할 위치를 구하는 데에도 사용할 수 있다. 예컨대 100px만큼 우측으로 이동하고, 겹친 양이 60px이라면, 원래 있어야 할 포지션은 현재 위치에서 60 - 100 = -40px 이동한 위치임을 쉽게 구할 수 있다.

	ColliderType aType = rbCollider->GetColliderType();
	ColliderType bType = otherCollider->GetColliderType();

	if (aType == ColliderType::Box)
	{
		if (bType == ColliderType::Box)
		{
			return GetOvelapAmountBySAT_Box_Box(moveDir, static_cast<BoxCollider*>(rbCollider), static_cast<BoxCollider*>(otherCollider));
		}
		else if (bType == ColliderType::Circle)
		{
			return GetOvelapAmountBySAT_Box_Circle(moveDir, static_cast<BoxCollider*>(rbCollider), static_cast<CircleCollider*>(otherCollider));
		}
	}
	else if (aType == ColliderType::Circle)
	{
		if (bType == ColliderType::Box)
		{
			return GetOvelapAmountBySAT_Circle_Box(moveDir, static_cast<CircleCollider*>(rbCollider), static_cast<BoxCollider*>(otherCollider));
		}
		else if (bType == ColliderType::Circle)
		{
			return GetOvelapAmountBySAT_Circle_Circle(moveDir, static_cast<CircleCollider*>(rbCollider), static_cast<CircleCollider*>(otherCollider));
		}
	}

	return 0.0f;
}
float CollisionCheck::GetOvelapAmountBySAT_Box_Box(Vector2 separateAxis, BoxCollider* rbBox, BoxCollider* other)
{
	std::vector<Vector2>&& b1p = rbBox->GetPoints();
	std::vector<Vector2>&& b2p = other->GetPoints();

	std::vector<float> b1r;
	for (auto point : b1p)
	{
		b1r.push_back(separateAxis.DotProduct(point));
	}
	std::vector<float> b2r;
	for (auto point : b2p)
	{
		b2r.push_back(separateAxis.DotProduct(point));
	}

	float b1min = b1r[0];
	float b1max = b1r[0];
	for (int i = 1; i < 4; ++i)
	{
		b1min = b1min > b1r[i] ? b1r[i] : b1min;
		b1max = b1max < b1r[i] ? b1r[i] : b1max;
	}
	float b2min = b2r[0];
	float b2max = b2r[0];
	for (int i = 1; i < 4; ++i)
	{
		b2min = b2min > b2r[i] ? b2r[i] : b2min;
		b2max = b2max < b2r[i] ? b2r[i] : b2max;
	}

	// rbBox(b1) 기준으로 판별
	// 겹치지 않는 경우
	if (b1max < b2min) return 0.0f;
	if (b2max < b1min) return 0.0f;

	// 겹치는 경우
	// 완전히 겹치는 경우
	if (b1min <= b2min && b2max <= b1max)
	{
		return b2max - b1min;
	}
	else if (b2min <= b1min && b1max <= b2max)
	{
		return b1max - b2min;
	}
	// 일부만 겹치는 경우
	// 두 콜라이더 중 한 쪽이 앞에 오는 경우가 있을 수 있는데,
	// rbCollider가 앞에 있을 경우 rbCollider.max - otherCollider.min을 반환하면 되지만
	// rbCollider가 뒤에 있을 경우 rbCollider를 기준으로 한 overlap을 구하는 것이므로 rbCollider.max - otherCollider.min을 반환해야 한다.
	// 결과적으로 두 경우의 반환값이 같으므로 분기할 필요가 없다.
	else
	{
		return b1max - b2min;
	}

	return 0.0f;
}
float CollisionCheck::GetOvelapAmountBySAT_Box_Circle(Vector2 separateAxis, BoxCollider* rbBox, CircleCollider* other)
{
	std::vector<Vector2>&& bPoints = rbBox->GetPoints();
	std::vector<float> bResult;
	for (auto point : bPoints)
	{
		bResult.push_back(separateAxis.DotProduct(point));
	}
	float bMin = bResult[0];
	float bMax = bResult[0];
	for (int i = 1; i < 4; ++i)
	{
		bMin = bMin > bResult[i] ? bResult[i] : bMin;
		bMax = bMax < bResult[i] ? bResult[i] : bMax;
	}

	float cDot = separateAxis.DotProduct(other->GetCenterPosition());
	float cMin = cDot - other->GetRadius();
	float cMax = cDot + other->GetRadius();

	// 겹치지 않는 경우
	if (bMax < cMin) return 0.0f;
	if (cMax < bMin) return 0.0f;

	// 겹치는 경우
	// 완전히 겹치는 경우
	if (bMin <= cMin && cMax <= bMax)
	{
		return cMax - bMin;
	}
	else if (cMin <= bMin && bMax <= cMax)
	{
		return bMax - cMin;
	}
	// 일부만 겹치는 경우
	// 두 콜라이더 중 한 쪽이 앞에 오는 경우가 있을 수 있는데,
	// rbCollider가 앞에 있을 경우 rbCollider.max - otherCollider.min을 반환하면 되지만
	// rbCollider가 뒤에 있을 경우 rbCollider를 기준으로 한 overlap을 구하는 것이므로 rbCollider.max - otherCollider.min을 반환해야 한다.
	// 결과적으로 두 경우의 반환값이 같으므로 분기할 필요가 없다.
	else
	{
		return bMax - cMin;
	}

	return 0.0f;
}
float CollisionCheck::GetOvelapAmountBySAT_Circle_Box(Vector2 separateAxis, CircleCollider* rbCircle, BoxCollider* other)
{
	float cDot = separateAxis.DotProduct(rbCircle->GetCenterPosition());
	float cMin = cDot - rbCircle->GetRadius();
	float cMax = cDot + rbCircle->GetRadius();

	std::vector<Vector2>&& bPoints = other->GetPoints();
	std::vector<float> bResult;
	for (auto point : bPoints)
	{
		bResult.push_back(separateAxis.DotProduct(point));
	}
	float bMin = bResult[0];
	float bMax = bResult[0];
	for (int i = 1; i < 4; ++i)
	{
		bMin = bMin > bResult[i] ? bResult[i] : bMin;
		bMax = bMax < bResult[i] ? bResult[i] : bMax;
	}


	// 겹치지 않는 경우
	if (bMax < cMin) return 0.0f;
	if (cMax < bMin) return 0.0f;

	// 겹치는 경우
	// 완전히 겹치는 경우
	if (bMin <= cMin && cMax <= bMax)
	{
		return cMax - bMin;
	}
	else if (cMin <= bMin && bMax <= cMax)
	{
		return bMax - cMin;
	}
	// 일부만 겹치는 경우
	// 두 콜라이더 중 한 쪽이 앞에 오는 경우가 있을 수 있는데,
	// rbCollider가 앞에 있을 경우 rbCollider.max - otherCollider.min을 반환하면 되지만
	// rbCollider가 뒤에 있을 경우 rbCollider를 기준으로 한 overlap을 구하는 것이므로 rbCollider.max - otherCollider.min을 반환해야 한다.
	// 결과적으로 두 경우의 반환값이 같으므로 분기할 필요가 없다.
	else
	{
		return cMax - bMin;
	}

	return 0.0f;
}
float CollisionCheck::GetOvelapAmountBySAT_Circle_Circle(Vector2 separateAxis, CircleCollider* rbCircle, CircleCollider* otherCircle)
{
	float c1Dot = separateAxis.DotProduct(rbCircle->GetCenterPosition());
	float c1Min = c1Dot - rbCircle->GetRadius();
	float c1Max = c1Dot + rbCircle->GetRadius();

	float c2Dot = separateAxis.DotProduct(otherCircle->GetCenterPosition());
	float c2Min = c2Dot - otherCircle->GetRadius();
	float c2Max = c2Dot + otherCircle->GetRadius();


	// 겹치지 않는 경우
	if (c1Max < c2Min) return 0.0f;
	if (c2Max < c1Min) return 0.0f;

	// 겹치는 경우
	// 완전히 겹치는 경우
	if (c1Min <= c2Min && c2Max <= c1Max)
	{
		return c2Max - c1Min;
	}
	else if (c2Min <= c1Min && c1Max <= c2Max)
	{
		return c1Max - c2Min;
	}
	// 일부만 겹치는 경우
	// 두 콜라이더 중 한 쪽이 앞에 오는 경우가 있을 수 있는데,
	// rbCollider가 앞에 있을 경우 rbCollider.max - otherCollider.min을 반환하면 되지만
	// rbCollider가 뒤에 있을 경우 rbCollider를 기준으로 한 overlap을 구하는 것이므로 rbCollider.max - otherCollider.min을 반환해야 한다.
	// 결과적으로 두 경우의 반환값이 같으므로 분기할 필요가 없다.
	else
	{
		return c1Max - c2Min;
	}

	return 0.0f;
}
















//Vector2 CollisionCheck::GetOverlapAreaBetweenBoxAndBox(BoxCollider* box1, BoxCollider* box2)
//{
//	Vector2 overlap;
//	RectF box1Rect = box1->GetBoundingBoxRect();
//	RectF box2Rect = box2->GetBoundingBoxRect();
//	Vector2 box1Size = { box1Rect.right - box1Rect.left, box1Rect.top - box1Rect.bottom };
//	Vector2 box2Size = { box2Rect.right - box2Rect.left, box2Rect.top - box2Rect.bottom };


//	float distance = std::abs(box1->GetCenterPosition().x - box2->GetCenterPosition().x);
//	overlap.x = (box1Size.x / 2 + box2Size.x / 2) - distance;
//	if (overlap.x < 0) overlap.x = 0;

//	distance = std::abs(box1->GetCenterPosition().y - box2->GetCenterPosition().y);
//	overlap.y = (box1Size.y / 2 + box2Size.y / 2) - distance;
//	if (overlap.y < 0) overlap.y = 0;

//	return overlap;
//}


//// Circle도 바운딩박스를 씌워서 하는 법밖에 없나?
//Vector2 CollisionCheck::GetOverlapAreaBetweenBoxAndCircle(DogeEngine::Component::BoxCollider* box, DogeEngine::Component::CircleCollider* circle)
//{
//	Vector2 overlap;
//	RectF boxRect = box->GetBoundingBoxRect();
//	Vector2 boxSize = { boxRect.right - boxRect.left, boxRect.top - boxRect.bottom };

//	float distance = std::abs(box->GetCenterPosition().x - circle->GetCenterPosition().x);
//	overlap.x = (boxSize.x / 2 + circle->GetRadius()) - distance;
//	if (overlap.x < 0) overlap.x = 0;

//	distance = std::abs(box->GetCenterPosition().y - circle->GetCenterPosition().y);
//	overlap.y = (boxSize.y / 2 + circle->GetRadius()) - distance;
//	if (overlap.y < 0) overlap.y = 0;

//	return overlap;
//}
//Vector2 CollisionCheck::GetOverlapAreaBetweenCircleAndCircle(DogeEngine::Component::CircleCollider* circle1, DogeEngine::Component::CircleCollider* circle2)
//{
//	Vector2 overlap;

//	float distance = std::abs(circle1->GetCenterPosition().x - circle2->GetCenterPosition().x);
//	overlap.x = (circle1->GetRadius() + circle2->GetRadius()) - distance;
//	if (overlap.x < 0) overlap.x = 0;

//	distance = std::abs(circle1->GetCenterPosition().y - circle2->GetCenterPosition().y);
//	overlap.y = (circle1->GetRadius() + circle2->GetRadius()) - distance;
//	if (overlap.y < 0) overlap.y = 0;

//	return overlap;
//}
//Vector2 CollisionCheck::GetOverlapAreaBetweenOBBAndOBB(DogeEngine::Component::BoxCollider* box1, DogeEngine::Component::BoxCollider* box2)
//{
//	
//	return Vector2{ 0, 0 };
//}
NAMESPACE_DOGEENGINE_END
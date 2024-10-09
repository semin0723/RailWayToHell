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

	// ������ ���� �ùĿ��� ��ģ ��ŭ �о �� SAT�� ����ϱ� ������ �浹 üũ�� �ϸ鼭 ���ÿ� ��ģ ���� �˾Ƴ��� ���� SAT�� ���� �ɷ� ����
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

	// �ſ� ������ ������ ����ġ��, ���� ������ �� �� �ִ� ����Ÿ����� �� �ָ� üũ ����
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
	//	// �پ��� ����� ���� �� ������, ���� ���� ����� ���� ȸ�����ѵ� ������ �����Ƿ� �簢���� �࿡ ���ĵǵ��� ȸ������ ���� �� �������� ���� ���� AABB-Circle Collision detection �˰����� ����ϴ� ���̴�
	//	// �簢���� AABB�� �ǵ��� �簢���� ������ �������� ȸ������ ���� circle�� �������� ������.
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
		//���߿� ���Ǳ�
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
			// �پ��� ����� ���� �� ������, ���� ���� ����� ���� ȸ�����ѵ� ������ �����Ƿ� �簢���� �࿡ ���ĵǵ��� ȸ������ ���� �� �������� ���� ���� AABB-Circle Collision detection �˰����� ����ϴ� ���̴�
			// �簢���� AABB�� �ǵ��� �簢���� ������ �������� ȸ������ ���� circle�� �������� ������.
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
	// dn�� pn���� �ٸ� ������ ������, �������� ���ϴ� ���͸� �������� ���� z��
	float d1 = (line2Start - line1Start).ToVector3().CrossProduct((line2End - line1Start).ToVector3()).z;
	float d2 = (line2Start - line1End).ToVector3().CrossProduct((line2End - line1End).ToVector3()).z;
	float d3 = (line1Start - line2Start).ToVector3().CrossProduct((line1End - line2Start).ToVector3()).z;
	float d4 = (line1Start - line2End).ToVector3().CrossProduct((line1End - line2End).ToVector3()).z;

	// ���� �������κ��� ����� ������ ������ dn(d1-d2, d3-d4)�� ����, �� ������ ������ ��� �� dn�� ��ȣ�� �ݴ밡 �ȴ�.
	// �ݸ� �� ������ �������� �ʴ´ٸ�, d1-d2 �Ǵ� d3-d4 �� ��� �ϳ��� ��ȣ�� ����.
	// �� �� ���� �Ǵ� �� ���а� �ٸ� ������ �� ���� �������� ���δٸ�, dn�� 0�� �ȴ�.
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
	// ������ �Ǵ� ������ �ڽ��� �ȿ� ���� ���
	if (BetweenAxisAlignedBoxAndPoint(min, max, lineStart)) return true;
	if (BetweenAxisAlignedBoxAndPoint(min, max, lineEnd)) return true;

	// �Ǵ� �Ұ��� �� ���� ���а� ��������
	// ������
	Vector2 start = { max.x, min.y };
	Vector2 end = { max.x, max.y };
	if (BetweenLinesegAndLineseg(start, end, lineStart, lineEnd)) return true;

	// ������
	start = { min.x, min.y };
	end = { min.x, max.y };
	if (BetweenLinesegAndLineseg(start, end, lineStart, lineEnd)) return true;

	// ����
	start = { min.x, min.y };
	end = { max.x, min.y };
	if (BetweenLinesegAndLineseg(start, end, lineStart, lineEnd)) return true;

	// �Ʒ���
	start = { min.x, max.y };
	end = { max.x, max.y };
	if (BetweenLinesegAndLineseg(start, end, lineStart, lineEnd)) return true;

	return false;
}

SATResult CollisionCheck::SAT_Box_Box(Vector2 box1Pos, std::vector<Vector2>&& box1, Vector2 box2Pos, std::vector<Vector2>&& box2)
{
	// <07-17>
	// ���� ������ Convex �ٰ����� �и����� ���� ���⿡�� 90�� ȸ����(-y, x)���Ϳ��� �Ѵ�.
	// �ٵ� ���簢���� 90�� ȸ���� ���� �� ������ ���̴ٺ��� ���̰� ����. �� ���簢���� ���ؼ��� 90�� ȸ���ϴ� ������ ������ �� �ִ�.

	// �簢���� ��� ���ֺ��� ���� �����ϱ� ������ �簢������ �� ���� �ุ Ȯ���ϸ� �ȴ�.
	std::vector<Vector2> axes;
	axes.push_back((box1[1] - box1[0]).Normalize());
	axes.push_back((box1[2] - box1[1]).Normalize());
	axes.push_back((box2[1] - box2[0]).Normalize());
	axes.push_back((box2[2] - box2[1]).Normalize());

	// �浹�� ���� ��� �о�� �۾��� �� �� �ֵ��� �ּ� overlap�� ������ �и����� �ݶ��̴��� ����ؾ� �Ѵ�.
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

		// � �����ε� �浹���� ������ Ȯ�εȴٸ� �ּ� overlap�� ����� SA�� ����� �ʿ䵵 ���� ����
		if (box1max < box2min) return { false, 0.0f, {0, 0} };
		if (box2max < box1min) return { false, 0.0f, {0, 0} };


		// �ش� ���� �������� �浹�� ��� ~ �ּ� overlap�� ����� SA�� ã�Ƽ� ���
		float overlap;
		// ������ ��ġ�� ��� ~ �� ������ üũ�غ��� ��
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
		// �Ϻΰ� ��ġ�� ���
		// ������ ��ġ�� ��츦 ���� Ȯ���ϹǷ� �Ʒ��� ���� �����ϰ� ���ǽ� �ۼ��� �����ϴ�
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

	// ���������� �о ������ �����ؾ� �ϹǷ� �ּ� overlap�� �����ϴ� �и����� �������� �� �ڽ��� ������ ��� ��ġ�� ���Ѵ�.
	float box1Dot = bestSeparateAxis.DotProduct(box1Pos);
	float box2Dot = bestSeparateAxis.DotProduct(box2Pos);
	// ����(rbCollider)�� �и��� �������󿡼� �ڿ� �ִٸ� �и��� �������� �а�, �տ� �ִٸ� �ݴ� �������� �о�� �Ѵ�.
	if (box1Dot < box2Dot) bestSeparateAxis = -bestSeparateAxis;

	return SATResult{ true, minOverlap, bestSeparateAxis };
}

SATResult CollisionCheck::SAT_Box_Circle(Vector2 boxPos, std::vector<Vector2>&& boxPoints, Vector3 circlePos, float radius)
{
	// �簢���� ��� ���ֺ��� ���� �����ϱ� ������ �簢������ �� ���� �ุ Ȯ���ϸ� �ȴ�.
	std::vector<Vector2> axes;
	axes.push_back((boxPoints[1] - boxPoints[0]).Normalize());
	axes.push_back((boxPoints[2] - boxPoints[1]).Normalize());
	axes.push_back((boxPos - circlePos).Normalize());

	// �浹�� ���� ��� �о�� �۾��� �� �� �ֵ��� �ּ� overlap�� ������ �и����� �ݶ��̴��� ����ؾ� �Ѵ�.
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

		// � �����ε� �浹���� ������ Ȯ�εȴٸ� �ּ� overlap�� ����� SA�� ����� �ʿ䵵 ���� ����
		if (boxMax < circleMin) return { false, 0.0f, {0, 0} };
		if (circleMax < boxMin) return { false, 0.0f, {0, 0} };


		// �ش� ���� �������� �浹�� ��� ~ �ּ� overlap�� ����� SA�� ã�Ƽ� ���
		float overlap;
		// ������ ��ġ�� ��� ~ �� ������ üũ�غ��� ��
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
		// �Ϻΰ� ��ġ�� ���
		// ������ ��ġ�� ��츦 ���� Ȯ���ϹǷ� �Ʒ��� ���� �����ϰ� ���ǽ� �ۼ��� �����ϴ�
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

	// ���������� �о ������ �����ؾ� �ϹǷ� �ּ� overlap�� �����ϴ� �и����� �������� �� �ڽ��� ������ ��� ��ġ�� ���Ѵ�.
	float boxDot = bestSeparateAxis.DotProduct(boxPos);
	float circleDot = bestSeparateAxis.DotProduct(circlePos);
	// ����(rbCollider)�� �и��� �������󿡼� �ڿ� �ִٸ� �и��� �������� �а�, �տ� �ִٸ� �ݴ� �������� �о�� �Ѵ�.
	if (boxDot < circleDot) bestSeparateAxis = -bestSeparateAxis;

	return SATResult{ true, minOverlap, bestSeparateAxis };
}

SATResult CollisionCheck::SAT_Circle_Box(Vector3 circlePos, float radius, Vector2 boxPos, std::vector<Vector2>&& boxPoints)
{
	// �簢���� ��� ���ֺ��� ���� �����ϱ� ������ �簢������ �� ���� �ุ Ȯ���ϸ� �ȴ�.
	std::vector<Vector2> axes;
	axes.push_back((boxPoints[1] - boxPoints[0]).Normalize());
	axes.push_back((boxPoints[2] - boxPoints[1]).Normalize());
	axes.push_back((circlePos - boxPos).Normalize());

	// �浹�� ���� ��� �о�� �۾��� �� �� �ֵ��� �ּ� overlap�� ������ �и����� �ݶ��̴��� ����ؾ� �Ѵ�.
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

		// � �����ε� �浹���� ������ Ȯ�εȴٸ� �ּ� overlap�� ����� SA�� ����� �ʿ䵵 ���� ����
		if (boxMax < circleMin) return { false, 0.0f, {0, 0} };
		if (circleMax < boxMin) return { false, 0.0f, {0, 0} };


		// �ش� ���� �������� �浹�� ��� ~ �ּ� overlap�� ����� SA�� ã�Ƽ� ���
		float overlap = 0.0f;
		// ������ ��ġ�� ��� ~ �� ������ üũ�غ��� ��
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
		// �Ϻΰ� ��ġ�� ���
		// ������ ��ġ�� ��츦 ���� Ȯ���ϹǷ� �Ʒ��� ���� �����ϰ� ���ǽ� �ۼ��� �����ϴ�
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

	// ���������� �о ������ �����ؾ� �ϹǷ� �ּ� overlap�� �����ϴ� �и����� �������� �� �ڽ��� ������ ��� ��ġ�� ���Ѵ�.
	float circleDot = bestSeparateAxis.DotProduct(circlePos);
	float boxDot = bestSeparateAxis.DotProduct(boxPos);
	// ����(rbCollider)�� �и��� �������󿡼� �ڿ� �ִٸ� �и��� �������� �а�, �տ� �ִٸ� �ݴ� �������� �о�� �Ѵ�.
	if (circleDot < boxDot) bestSeparateAxis = -bestSeparateAxis;

	return SATResult{ true, minOverlap, bestSeparateAxis };
}

bool CollisionCheck::BetweenOrientedBoxAndPoint(std::vector<Vector2> boxPoints, Vector2 point)
{
	// �и��� �̷��� ����. ���� �� �и��࿡�� ��� min ~ max ���̿� �ִٸ� �簢���� ���ο� �ִ� ��.
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
	// AABB - Line�� �Ȱ���.
	// ������ �Ǵ� ������ �ڽ��� �ȿ� ���� ���
	if (BetweenOrientedBoxAndPoint(boxPoints, lineStart)) return true;
	if (BetweenOrientedBoxAndPoint(boxPoints, lineEnd)) return true;

	// �Ǵ� �Ұ��� �� ���� ���а� ��������
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

	// � �����ε� �浹���� ������ Ȯ�εȴٸ� �ּ� overlap�� ����� SA�� ����� �ʿ䵵 ���� ����
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
	// �Ϻΰ� ��ġ�� ���
	// ������ ��ġ�� ��츦 ���� Ȯ���ϹǷ� �Ʒ��� ���� �����ϰ� ���ǽ� �ۼ��� �����ϴ�
	else if (c1Min <= c2Min && c2Min <= c1Max)
	{
		overlap = c1Max - c2Min;
	}
	else if (c2Min <= c1Min && c1Min <= c2Max)
	{
		overlap = c2Max - c1Min;
	}

	// ���������� �о ������ �����ؾ� �ϹǷ� �ּ� overlap�� �����ϴ� �и����� �������� �� �ڽ��� ������ ��� ��ġ�� ���Ѵ�.
	// ����(rbCollider)�� �и��� �������󿡼� �ڿ� �ִٸ� �и��� �������� �а�, �տ� �ִٸ� �ݴ� �������� �о�� �Ѵ�.
	// �ٵ� �̰� �ڽ��� ���� �и������� ���� ���� ����� Circle�� Circle�� SAT���� �ݵ�� rbCollider(pos1)���� ���ϴ� ���Ͱ� axis�� �Ǳ� ������ ��� �ȴ�. �׷��� Ȥ�� �𸣴ϱ� ������ġ��...
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
	// �� �κ� ��Ʈ���� �����忡 ����ص� �޸� �����ؼ� �ٽ� ������ ��

	// ������ ������ �Ǵ� ������ �� ���ο� ���� ��
	if ((lineStart - circle_pos.ToVector2()).Magnitude() <= radius) return true;
	if ((lineEnd - circle_pos.ToVector2()).Magnitude() <= radius) return true;

	// �Ǻ����� �̿��� ���� ������ ��������
	Vector2 line = lineEnd - lineStart;
	Vector2 centerToStart = lineStart - circle_pos.ToVector2();
	// ���� �������� ��� ���ϱ� : a*t^2 + b*t + c = 0
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
	// circle�� �ڽ��� ���� ���弱�� ���Ե� ���
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
	// circle�� �ڽ��� ���� ���弱�� ���Ե��� ���� ���
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
	// Rigidbody�� movement ������ �и������� ��� SAT�� �����ϸ� �ش� ���������� overlap�� ���ϴ� �� �����ϴ�. (AABB���� overlap�� ���ϴ� ��쿡�� ����� �߽� ��Ŀ�� x, y ���� �������� �� overlap���� �б��ϴ� �������� ���� ����̹Ƿ�, �� ����� ����ϴ� ���� ����) �� �� movement�� '���� fixedUpdate������ ������ - ���� fixedUpdate������ ������'���� ���� ��� Rigidbody2D�� ���� �̵��� Transform.Translate�� ���� �̵��� ��� ó���� �� �ִ�.
	// movement�� �׷��� ���ߴٸ�, �� �� ���� �浹�ϴ� �� �ݶ��̴��� �ݵ�� movement ������ �и������� ����� �� overlap�� ���� �� �ְ� �ȴ�. 
	// Rigidbody2D�� �� FixedUpdate���� ������Ʈ�� �����̰� �ϴ� ������ velocity ���Ϳ� �浹 üũ���� ����� movement�� ���� �� ����� prevPosition ���͸� ������ �ȴ�.
	// curPosition - prevPosition���� �浹 üũ���� ����ϴ� movement ���͸� ���� �� �ְ�, �� �� curPosition�� Rigidbody2D.MoveObject()�� ȣ���� �ڿ��� �Ѵ�. �׷� movement ���ʹ� �� FixedUpdate ������ ���� �������� ��, �� ���� �����ӿ����� �̵��Ÿ��� �ǹǷ�, �浹 �� ��ģ overlap �縸ŭ �о�� �ͻӸ� �ƴ϶� ƨ���� ���� �� ���� �־�� �� ��ġ�� ���ϴ� ������ ����� �� �ִ�. ������ 100px��ŭ �������� �̵��ϰ�, ��ģ ���� 60px�̶��, ���� �־�� �� �������� ���� ��ġ���� 60 - 100 = -40px �̵��� ��ġ���� ���� ���� �� �ִ�.

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

	// rbBox(b1) �������� �Ǻ�
	// ��ġ�� �ʴ� ���
	if (b1max < b2min) return 0.0f;
	if (b2max < b1min) return 0.0f;

	// ��ġ�� ���
	// ������ ��ġ�� ���
	if (b1min <= b2min && b2max <= b1max)
	{
		return b2max - b1min;
	}
	else if (b2min <= b1min && b1max <= b2max)
	{
		return b1max - b2min;
	}
	// �Ϻθ� ��ġ�� ���
	// �� �ݶ��̴� �� �� ���� �տ� ���� ��찡 ���� �� �ִµ�,
	// rbCollider�� �տ� ���� ��� rbCollider.max - otherCollider.min�� ��ȯ�ϸ� ������
	// rbCollider�� �ڿ� ���� ��� rbCollider�� �������� �� overlap�� ���ϴ� ���̹Ƿ� rbCollider.max - otherCollider.min�� ��ȯ�ؾ� �Ѵ�.
	// ��������� �� ����� ��ȯ���� �����Ƿ� �б��� �ʿ䰡 ����.
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

	// ��ġ�� �ʴ� ���
	if (bMax < cMin) return 0.0f;
	if (cMax < bMin) return 0.0f;

	// ��ġ�� ���
	// ������ ��ġ�� ���
	if (bMin <= cMin && cMax <= bMax)
	{
		return cMax - bMin;
	}
	else if (cMin <= bMin && bMax <= cMax)
	{
		return bMax - cMin;
	}
	// �Ϻθ� ��ġ�� ���
	// �� �ݶ��̴� �� �� ���� �տ� ���� ��찡 ���� �� �ִµ�,
	// rbCollider�� �տ� ���� ��� rbCollider.max - otherCollider.min�� ��ȯ�ϸ� ������
	// rbCollider�� �ڿ� ���� ��� rbCollider�� �������� �� overlap�� ���ϴ� ���̹Ƿ� rbCollider.max - otherCollider.min�� ��ȯ�ؾ� �Ѵ�.
	// ��������� �� ����� ��ȯ���� �����Ƿ� �б��� �ʿ䰡 ����.
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


	// ��ġ�� �ʴ� ���
	if (bMax < cMin) return 0.0f;
	if (cMax < bMin) return 0.0f;

	// ��ġ�� ���
	// ������ ��ġ�� ���
	if (bMin <= cMin && cMax <= bMax)
	{
		return cMax - bMin;
	}
	else if (cMin <= bMin && bMax <= cMax)
	{
		return bMax - cMin;
	}
	// �Ϻθ� ��ġ�� ���
	// �� �ݶ��̴� �� �� ���� �տ� ���� ��찡 ���� �� �ִµ�,
	// rbCollider�� �տ� ���� ��� rbCollider.max - otherCollider.min�� ��ȯ�ϸ� ������
	// rbCollider�� �ڿ� ���� ��� rbCollider�� �������� �� overlap�� ���ϴ� ���̹Ƿ� rbCollider.max - otherCollider.min�� ��ȯ�ؾ� �Ѵ�.
	// ��������� �� ����� ��ȯ���� �����Ƿ� �б��� �ʿ䰡 ����.
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


	// ��ġ�� �ʴ� ���
	if (c1Max < c2Min) return 0.0f;
	if (c2Max < c1Min) return 0.0f;

	// ��ġ�� ���
	// ������ ��ġ�� ���
	if (c1Min <= c2Min && c2Max <= c1Max)
	{
		return c2Max - c1Min;
	}
	else if (c2Min <= c1Min && c1Max <= c2Max)
	{
		return c1Max - c2Min;
	}
	// �Ϻθ� ��ġ�� ���
	// �� �ݶ��̴� �� �� ���� �տ� ���� ��찡 ���� �� �ִµ�,
	// rbCollider�� �տ� ���� ��� rbCollider.max - otherCollider.min�� ��ȯ�ϸ� ������
	// rbCollider�� �ڿ� ���� ��� rbCollider�� �������� �� overlap�� ���ϴ� ���̹Ƿ� rbCollider.max - otherCollider.min�� ��ȯ�ؾ� �Ѵ�.
	// ��������� �� ����� ��ȯ���� �����Ƿ� �б��� �ʿ䰡 ����.
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


//// Circle�� �ٿ���ڽ��� ������ �ϴ� ���ۿ� ����?
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
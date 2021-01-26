#pragma once

struct UserCmd;
struct Vector;

namespace Grief
{
	Vector calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept;
	void runNormalBlock(UserCmd*);
	void runJumpBlock(UserCmd*);
}
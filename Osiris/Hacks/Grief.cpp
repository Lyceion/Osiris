#include "Grief.h"
#include "../Config.h"
#include "../Interfaces.h"
#include "../Memory.h"
#include "../SDK/Entity.h"
#include "../SDK/LocalPlayer.h"
#include "../SDK/UserCmd.h"

Vector Grief::calculateRelativeAngle(const Vector& source, const Vector& destination, const Vector& viewAngles) noexcept
{
	return ((destination - source).toAngle() - viewAngles).normalize();
}

void Grief::runNormalBlock(UserCmd* cmd)
{
	if (!config->grief.blockbot.enabled)
		return;

	if (!config->grief.blockbot.key.isDown())
		return;

	float bestdist = 250.f;
	int index = -1;

	for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
		auto entity = interfaces->entityList->getEntity(i);

		if (!entity)
			continue;

		if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive() || entity->gunGameImmunity())
			continue;

		float dist = localPlayer->origin().distTo(entity->origin());

		if (dist < bestdist)
		{
			bestdist = dist;
			index = i;
		}
	}

	if (index == -1)
		return;

	auto target = interfaces->entityList->getEntity(index);

	if (!target)
		return;

	const auto angles = calculateRelativeAngle(localPlayer->origin(), target->origin(), cmd->viewangles);

	if (angles.y < 0.0f)
		cmd->sidemove = 450.f;
	else if (angles.y > 0.0f)
		cmd->sidemove = -450.f;
}

void Grief::runJumpBlock(UserCmd* cmd) {
    if (!config->grief.jumpblock.enabled)
        return;

    if (!config->grief.jumpblock.key.isDown())
        return;

    if (!localPlayer)
        return;

    float bestdist = 250.f;
    int index = -1;

    for (int i = 1; i <= interfaces->engine->getMaxClients(); i++) {
        auto entity = interfaces->entityList->getEntity(i);

        if (!entity)
            continue;

        if (!entity || entity == localPlayer.get() || entity->isDormant() || !entity->isAlive() || entity->gunGameImmunity())
            continue;

        float dist = localPlayer->origin().distTo(entity->origin());

        if (dist < bestdist)
        {
            bestdist = dist;
            index = i;
        }
    }
    Entity* pGroundEntity = interfaces->entityList->getEntity(index);

    if (pGroundEntity && pGroundEntity->getClientClass()->classId == ClassId::CSPlayer)
    {
        Vector vecVelocity = pGroundEntity->velocity();
        const float flSpeed = vecVelocity.length2D();
        QAngle angles = vecVelocity.toQAngle();
        Vector viewAngles = cmd->viewangles;
        vecVelocity.rotate(-viewAngles.y);
        Vector TargetPos = pGroundEntity->getBonePosition(8);
        Vector LocalPlayerPos = localPlayer->origin();
        Vector deltaVector = Vector{
            (TargetPos.x - LocalPlayerPos.x),
            (TargetPos.y - LocalPlayerPos.y)
        };
        deltaVector.rotate(-viewAngles.y);
        float size = deltaVector.length2D();

        cmd->forwardmove = vecVelocity.x + 50 * deltaVector.x / size;// +10 * deltaVector.x / dist;
        cmd->sidemove -= vecVelocity.y + 50 * deltaVector.y / size;// +10 * deltaVector.y / dist;
    }
}
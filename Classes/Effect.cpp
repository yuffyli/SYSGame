#include "Effect.h"
#include "EntityManager.h"

Effect::Effect()
{
	setType(ET_Effect);
}

void Effect::onEnter()
{
	MovingEntity::onEnter();

	m_bShouldMove = false;
	m_bIsKilled = false;
}

void Effect::onExit()
{
	MovingEntity::onExit();
}

void Effect::setTarget(CCPoint target)
{
	m_vTargetPos = target;
}

bool Effect::isKilled() const 
{
	return m_bIsKilled;
}

void Effect::kill()
{
	m_bIsKilled = true;
	EM.removeAnEntity(this, ET_Effect);
}
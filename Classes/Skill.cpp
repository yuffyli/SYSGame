#include "Skill.h"
#include "Hero.h"
#include "Monster.h"
#include "Effect.h"
#include "GameInfo.h"
#include "EntityManager.h"

Skill::Skill(int id) : m_iSkillID(id) 
{
}

void Skill::onEnter()
{
	CCNode::onEnter();	

	// 设定谁拥有这个Character
	m_pOwner = (Character*)(getParent());
	m_bIsAttacking = false;
	m_fElapseTime = 100000.f;
	m_bIsEnable = true;
	m_pEffect = NULL;

	// 设置技能各种属性
	int SkillIdx = m_iSkillID - 1;
	m_iDamage = GI.getSkillConfig()[SkillIdx].iDamage;
	m_fCoolTime = GI.getSkillConfig()[SkillIdx].iSkillCD;
	m_fAttackSpeed = GI.getSkillConfig()[SkillIdx].fSpeed;
	m_fAttackRange = GI.getSkillConfig()[SkillIdx].fAttackRange;
	m_fHitRange = GI.getSystemConfig().fHitRange;

	schedule(schedule_selector(Skill::onUpdate));
}

void Skill::onExit()
{
	unschedule(schedule_selector(Skill::onUpdate));

	CCNode::onExit();
}

void Skill::attack()
{
	m_bIsAttacking = true;
	m_fElapseTime = 0;
	EM.addAnAttackEffect(m_pOwner->getPosition(), EEffectType(m_iSkillID), m_pOwner->getEnemyType(), 
		m_iDamage, m_pTarget->getPosition());
}

bool Skill::shouldAttack()
{
	if (!m_bIsEnable)
	{
		return false;
	}

	if (m_fElapseTime < m_fCoolTime)
	{
		return false;
	}

	Character* pCha = (Character*)EM.findNearestEntityInRange(m_pOwner, m_fAttackRange, m_pOwner->getEnemyType());
	if (pCha)
	{
		if (pCha->getType() == ET_Hero && !((Hero*)pCha)->isPickedUp())
		{
			// 还没被捡起来就不要攻击他了啊。。。
		}
		else 
		{
			m_pTarget = pCha;
			return true;
		}
	}

	return false;
}

void Skill::onUpdate(float dt)
{
	// 这个我也不知道怎么解释，坑爹的dt竟然有大于100的时刻！！！
	// >_<所以，将就着吧~~~
	if (dt < 0.5f)
	{
		m_fElapseTime += dt;
	}

	//if (m_bIsAttacking)
	//{
	//	// 特效已经消亡
	//	if (!m_pEffect || m_pEffect->isKilled())
	//	{
	//		m_bIsAttacking = false;
	//	}
	//	else 
	//	{

	//	}
	//}

	if (shouldAttack())
	{
		attack();
	}
}

bool Skill::isEnable() const 
{
	return m_bIsEnable;
}

void Skill::setEnable(bool flag)
{
	m_bIsEnable = flag;
}

Skill* Skill::create(int SkillID)
{
	Skill *pRet = new Skill(SkillID); 
	if (pRet && pRet->init()) 
	{ 
		pRet->autorelease(); 
		return pRet; 
	} 
	else 
	{ 
		delete pRet; 
		pRet = NULL; 
		return NULL; 
	} 
}
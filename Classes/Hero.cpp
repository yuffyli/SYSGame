#include "Hero.h"
#include "Queue.h"
#include "Goods.h"
#include "EntityManager.h"
#include "Skill.h"
#include "GameHelper.h"
#include "GameInfo.h"

#define HERO_UNBEATABLE 0

void Hero::onEnter()
{
	Character::onEnter();

	m_bIsPickedUp = false;

	// 倒计时，一定时间后消失
	int flashTimes = int(/*GI.getSystemConfig().fFlashingTime*/ 5 * 2.5);
	CCAction* fadeAway = CCSequence::create(
		CCDelayTime::create(/*float(GI.getItemConfig()[2].iExistTime)*/ 5.f),
		CCBlink::create(float(GI.getSystemConfig().fFlashingTime), flashTimes),
		CCDelayTime::create(0.2f),
		CCCallFunc::create(this, callfunc_selector(Character::kill)),
		NULL
		);
	fadeAway->setTag(ECAT_HeroFade);
	runAction(fadeAway);

	int HeroIdx = m_iHeroID - 1;

	// 设置速度
	setCurSpeed(GI.getHeroConfig()[HeroIdx].fMoveSpeed); 
	setMaxSpeed(GI.getSystemConfig().fMaxMoveSpeed);
	m_fSpeedIncrement = GI.getSystemConfig().fSpeedIncrement;

	// 设置生命
	setCurHealth(GI.getHeroConfig()[HeroIdx].iHP);
	setMaxHealth(GI.getHeroConfig()[HeroIdx].iHP);

	// 显示血条开关打开
	m_bEnableShowHP = true;

	// 设置技能
	m_pSkill = Skill::create(1);
	addChild(m_pSkill);

	// 设置动画
	// 0.右 1.下 2.左 3.上 

	m_pWalkAnim[0] = CCAnimation::create();
	m_pWalkAnim[0]->retain();
	m_pWalkAnim[0]->addSpriteFrameWithFileName("spirit/hero/Hero1_R_1.png");
	m_pWalkAnim[0]->addSpriteFrameWithFileName("spirit/hero/Hero1_R_2.png");
	m_pWalkAnim[0]->addSpriteFrameWithFileName("spirit/hero/Hero1_R_3.png");
	m_pWalkAnim[0]->setDelayPerUnit(1.f / getCurSpeed() / 3);

	m_pWalkAnim[1] = CCAnimation::create();
	m_pWalkAnim[1]->retain();
	m_pWalkAnim[1]->addSpriteFrameWithFileName("spirit/hero/Hero1_D_1.png");
	m_pWalkAnim[1]->addSpriteFrameWithFileName("spirit/hero/Hero1_D_2.png");
	m_pWalkAnim[1]->setDelayPerUnit(0.5 / getCurSpeed());
	
	m_pWalkAnim[2] = CCAnimation::create();
	m_pWalkAnim[2]->retain();
	m_pWalkAnim[2]->addSpriteFrameWithFileName("spirit/hero/Hero1_L_1.png");
	m_pWalkAnim[2]->addSpriteFrameWithFileName("spirit/hero/Hero1_L_2.png");
	m_pWalkAnim[2]->addSpriteFrameWithFileName("spirit/hero/Hero1_L_3.png");
	m_pWalkAnim[2]->setDelayPerUnit(1.f / getCurSpeed() / 3);

	m_pWalkAnim[3] = CCAnimation::create();
	m_pWalkAnim[3]->retain();
	m_pWalkAnim[3]->addSpriteFrameWithFileName("spirit/hero/Hero1_U_1.png");
	m_pWalkAnim[3]->addSpriteFrameWithFileName("spirit/hero/Hero1_U_2.png");
	m_pWalkAnim[3]->setDelayPerUnit(0.5 / getCurSpeed());	
}

void Hero::setAnimFreq()
{
	m_pWalkAnim[0]->setDelayPerUnit(1.f / getCurSpeed() / 3);
	m_pWalkAnim[1]->setDelayPerUnit(0.5 / getCurSpeed());
	m_pWalkAnim[2]->setDelayPerUnit(1.f / getCurSpeed() / 3);
	m_pWalkAnim[3]->setDelayPerUnit(0.5 / getCurSpeed());
}

void Hero::onExit()
{
	//unschedule(schedule_selector(Hero::onUpdate));
	removeChild(m_pSkill, true);

	// 释放动画资源
	m_pWalkAnim[0]->release();
	m_pWalkAnim[1]->release();
	m_pWalkAnim[2]->release();
	m_pWalkAnim[3]->release();

	Character::onExit();
}

void Hero::onUpdate(float dt)
{
	// 如果这个英雄还没被拾取
	if (!m_bIsPickedUp)
	{
		return;
	}

	// 检查有没有英雄要“吃”
	if (m_pQueue && m_pQueue->getHead() == this)
	{
		BaseEntity* entity = EM.findHeroNotInQueue(this, GI.RangeToPickupHero);
		if (entity != NULL)
		{
			m_pQueue->addAMember((Character*)(entity));

			// 吃了之后，增加速度
			float speed = min(m_fMaxSpeed, m_fCurSpeed + m_fSpeedIncrement);
			m_pQueue->setCurSpeed(speed);
			CCLog("After Adding, Speed: %f", getCurSpeed());
		}
	}

	if (!onMove())
	{
		return;
	}	

	// 检查有没有物品可以使用
	Goods* pGood = (Goods*)(EM.findEntityInRange(this, 60.f, ET_Goods));
	if (pGood)
	{
		pGood->use();
	}

	// 检测是否撞到小怪了
	BaseEntity* entity = EM.findEntityInRange(this, 35.f, ET_Monster);
	if (entity)
	{
		// 这个怪死了
		((Character*)(entity))->kill();

		// 我也死了，呜呜呜>_<
#if !HERO_UNBEATABLE
		//kill();
		getHarmed(1000000);
		GI.IsGameOver = true;
#endif
	}

	// 撞到队友
	if (m_pQueue && m_pQueue->getHead() == this)
	{
		entity = EM.findEntityInRange(this, 25.f, ET_Hero);
		if (entity)
		{
			m_pQueue->allGotoDie();
			GI.IsGameOver = true;
		}
	}

	// 如果撞到墙壁
	// 地图碰撞放在这里！！！ @俊强
	if (!GI.Helper->isWithinMap(getPosition(), 20.f))
	{
		// 全部给我去死吧！！！！
		m_pQueue->allGotoDie();
		GI.IsGameOver = true;
	}
}

Hero* Hero::create(const char *pszFileName)
{
	Hero *pobSprite = new Hero();
	if (pobSprite && pobSprite->initWithFile(pszFileName))
	{
		pobSprite->autorelease();
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);
	return NULL;
}

Hero::Hero() : m_iHeroID(1)
{
	setType(ET_Hero);
}

Hero::Hero(int ID) : m_iHeroID(ID)
{
	setType(ET_Hero);
}

void Hero::setIsPickedUp(bool is_picked_up)
{
	m_bIsPickedUp = is_picked_up;
	
	// 英雄被获取了，就把闪烁动画给停了
	if (m_bIsPickedUp)
	{
		stopActionByTag(ECAT_HeroFade);
		setVisible(true);
	}
}
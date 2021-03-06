#include "Hero.h"
#include "Queue.h"
#include "Goods.h"
#include "EntityManager.h"
#include "Skill.h"
#include "GameHelper.h"
#include "GameInfo.h"
#include "AllEffects.h"
#include "Util.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;


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
	setMaxHealth(GI.getHeroConfig()[HeroIdx].iHP);
	setCurHealth(GI.getHeroConfig()[HeroIdx].iHP);

	// 显示血条开关打开
	m_bEnableShowHP = true;

	// 设置技能
	m_pSkill = Skill::create( GI.getHeroConfig()[HeroIdx].iSkillID);
	addChild(m_pSkill);
	m_pSkill->setEnable(false);

	// 设置动画
	// 0.右 1.下 2.左 3.上 
	char SHero[25];
	sprintf(SHero, "spirit/hero/Hero%d", m_iHeroID);
	std::string StrSHero(SHero);

	m_pWalkAnim[0] = CCAnimation::create();
	m_pWalkAnim[0]->retain();
	m_pWalkAnim[0]->addSpriteFrameWithFileName( (StrSHero + std::string("_R_1.png")).c_str() );
	m_pWalkAnim[0]->addSpriteFrameWithFileName( (StrSHero + std::string("_R_2.png")).c_str() );
	m_pWalkAnim[0]->addSpriteFrameWithFileName( (StrSHero + std::string("_R_3.png")).c_str() );
	m_pWalkAnim[0]->setDelayPerUnit(1.f / getCurSpeed() / 3);

	m_pWalkAnim[1] = CCAnimation::create();
	m_pWalkAnim[1]->retain();
	m_pWalkAnim[1]->addSpriteFrameWithFileName( (StrSHero + std::string("_D_1.png")).c_str() );
	m_pWalkAnim[1]->addSpriteFrameWithFileName( (StrSHero + std::string("_D_2.png")).c_str() );
	m_pWalkAnim[1]->setDelayPerUnit(0.5 / getCurSpeed());
	
	m_pWalkAnim[2] = CCAnimation::create();
	m_pWalkAnim[2]->retain();
	m_pWalkAnim[2]->addSpriteFrameWithFileName( (StrSHero + std::string("_L_1.png")).c_str() );
	m_pWalkAnim[2]->addSpriteFrameWithFileName( (StrSHero + std::string("_L_2.png")).c_str() );
	m_pWalkAnim[2]->addSpriteFrameWithFileName( (StrSHero + std::string("_L_3.png")).c_str() );
	m_pWalkAnim[2]->setDelayPerUnit(1.f / getCurSpeed() / 3);

	m_pWalkAnim[3] = CCAnimation::create();
	m_pWalkAnim[3]->retain();
	m_pWalkAnim[3]->addSpriteFrameWithFileName( (StrSHero + std::string("_U_1.png")).c_str() );
	m_pWalkAnim[3]->addSpriteFrameWithFileName( (StrSHero + std::string("_U_2.png")).c_str() );
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
		BaseEntity* entity = EM.findHeroNotInQueue(this, /*GI.RangeToPickupHero*/ GI.getSystemConfig().fPickupRange);
		if (entity != NULL)
		{
			// 竟然还要。。。加数字，我无语了
			GI.ScoreTimeCoef += 1;
			//showQueueNum(m_pQueue->getQueueNum() + 1);
			showQueueNum(GI.ScoreTimeCoef);

			m_pQueue->addAMember((Character*)(entity));

			// 吃了之后，增加速度
			float speed = min(m_fMaxSpeed, m_fCurSpeed + m_fSpeedIncrement);
			m_pQueue->setCurSpeed(speed);
			CCLog("After Adding, Speed: %f", getCurSpeed());
			
			SimpleAudioEngine::sharedEngine()->playEffect("music/eat3.mp3");
			// 播放烟雾特效
			EM.addAnEffect(getPosition(), EET_Smog, ccp(0, 0));
		}
	}
	
	// 检查有没有物品可以使用
	Goods* pGood = (Goods*)(EM.findEntityInRange(this,  GI.getSystemConfig().fPickupRange, ET_Goods));
	if (pGood)
	{
		pGood->use(this);
	}

	// 检测是否撞到小怪了
	BaseEntity* entity = EM.findEntityInRange(this, 35.f, ET_Monster);
	if (entity)
	{
		// 我也死了，呜呜呜>_<
		//kill();
		getHarmed(1000000);

		// 这个怪死了
		((Character*)(entity))->getHarmed(1000000, true);

	}

	if (!onMove())
	{
		return;
	}

	// 撞到队友
	if (m_pQueue && m_pQueue->getHead() == this)
	{
		entity = EM.findEntityInRange(this, 25.f, ET_Hero);
		if (entity && ((Hero*)(entity))->isPickedUp())
		{
			m_pQueue->allGotoDie();
			GI.IsGameOver = true;
			CCLog("Crash a teammate to die!!!!");
			SimpleAudioEngine::sharedEngine()->playEffect("music/eat3.mp3");
		}
	}

	// 如果撞到墙壁
	// 地图碰撞放在这里！！！ @俊强
	if (!GI.Helper->isWithinMap(getPosition(), 20.f))
	{
		// 全部给我去死吧！！！！
		m_pQueue->allGotoDie();
		GI.IsGameOver = true;
		SimpleAudioEngine::sharedEngine()->playEffect("music/eat3.mp3");
	}
}

Hero* Hero::create(const char *pszFileName, int HeroID)
{
	Hero *pobSprite = new Hero(HeroID);
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

	if (m_pSkill)
	{
		m_pSkill->setEnable(true);
	}
	
	// 英雄被获取了，就把闪烁动画给停了
	if (m_bIsPickedUp)
	{
		stopActionByTag(ECAT_HeroFade);
		setVisible(true);
	}
}

bool Hero::isPickedUp() const 
{
	return m_bIsPickedUp;
}

void Hero::showQueueNum(int QueueNum)
{
	std::string sNum = Util::NumberToString<int>(QueueNum);

	// 先清除掉以前的Tag
	for (int Tag = EHQNT_X; Tag <= EHQNT_3; ++Tag)
	{
		CCNode* Child = getChildByTag(Tag);
		if (Child != NULL)
		{
			((ShowNum*)(Child))->kill();
		}
	}

	float fH = 45.f;
	
	ShowNum* NumX = (ShowNum*)EM.addANumberOverCharacter(this, -1);
	NumX->setPosition(ccp(30.f, fH));
	NumX->setTag(EHQNT_X);

	CCAssert(sNum.size() > 0, "Size must > 0!!!!");

	ShowNum* Num1 = (ShowNum*)EM.addANumberOverCharacter(this, int(sNum[0] - '0'));
	Num1->setPosition(ccp(50.f, fH));
	Num1->setTag(EHQNT_1);

	if (sNum.length() > 1)
	{
		ShowNum* Num2 = (ShowNum*)EM.addANumberOverCharacter(this, int(sNum[1] - '0'));
		Num2->setPosition(ccp(70.f, fH));
		Num2->setTag(EHQNT_2);
	}

	if (sNum.length() > 2)
	{
		ShowNum* Num3 = (ShowNum*)EM.addANumberOverCharacter(this, int(sNum[2] - '0'));
		Num3->setPosition(ccp(90.f, fH));
		Num3->setTag(EHQNT_3);
	}
}
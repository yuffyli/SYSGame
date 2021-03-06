#ifndef SYS_ALL_GOODS
#define SYS_ALL_GOODS

#include "Goods.h"

/** 
 * 金币
 */
class Coin : public Goods
{
public:

	void onEnter();
	void onExit();
	void use(Hero* pHero);

	static Coin* create(const char *pszFileName);

protected:

	int		m_iValue;		// 金币价值
};

/** 
 * 血包
 */
class BloodSupply : public Goods
{
public:

	virtual void onEnter();
	virtual void onExit();
	void use(Hero* pHero = NULL);

	//为什么要在这里重载CCSprite???!!!
	//void  draw();

	static BloodSupply* create(const char *pszFileName);

protected:

	int		m_iSupplyAmount;		// 补给量
};

/** 
 * 冰块(产生冰冻特效)
 */
class IceCube : public Goods
{
public:

	void onEnter();
	void onExit();
	void use(Hero* pHero);

	static IceCube* create(const char *pszFileName);
};

#endif
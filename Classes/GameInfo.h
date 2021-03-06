#ifndef SYS_GAMEINFO
#define SYS_GAMEINFO

#include "cocos2d.h"
#include "Comm.h"
#include "Util.h"
#include "Database.h"

#include <string>

USING_NS_CC;

#define GI (GameInfo::instance())

class Character;
class MainGame;
class Queue;
class GameHelper;

const int M_N = 20;
const int M_M = 20;

/** 英雄最大种类 */
const int MAX_HERO_KINDS = 4;
const int MAX_MONSTER_KINDS = 3;

/** 
 * @brief 所有全局信息的单例
 */
class GameInfo
{
public:	
	static GameInfo& instance();

	GameInfo();


public:

	MainGame*			Game;						// 指向当前游戏Layer的指针
	Queue*				Me;							// 指向玩家的指针

	// 英雄
	std::string			PathOfHero;					// 英雄的图片路径	
	float				RangeToPickupHero;			// “吃”英雄的有效范围
	float				HeroInitSpeed;				// 英雄初始速度
	float				HeroMaxSpeed;				// 英雄最大速度
	int					CurrHeroId;					// 当前选择的英雄ID

	// 怪
	std::string			PathOfMonster;				// 怪的图片路径
	float				MonsterInitSpeed;			// 怪的初始速度
	float				MonsterMaxSpeed;			// 怪的最大速度

	// 场景
	CCLayer*			currentLayer;				// 每一关的切换都要对这个场景进行赋值，以跟随英雄移动
													// 跟随移动的代码在FirstStage的update函数中
	bool				IsGameOver;					// 是否游戏结束

	// 地图相关
	CCTMXTiledMap*		Map;						// 地图的引用
    CCTMXLayer*			Meta;						// 检测碰撞的地图层 
    CCTMXLayer*			Barrier;					// 障碍物层

	int					MapN;						//
	int					MapM;						// N行M列的地图
	float				MapWidth;					// 地图宽度
	float				MapHeight;					// 地图高度
	int					GridSize;					// 地图每一小格的大小

	// 其他
	float				ValidDraggedLength;			// 有效的滑动距离，在此基础上才进行滑动判定
	GameHelper*			Helper;						// 游戏辅助类
	float				ValidRefreshMonsDist;		// 在与英雄队首有此距离外刷怪

	// 音量
	int					musicVolume;				// 背景音乐音量
	int					soundVolume;				// 特效音乐音量

	// 金币，得分，杀怪数
	int					Coin;						// 金币
	float				Score;						// 得分
	int					MonsterKillNum;				// 杀怪数
	float				ChallengeTime;				// 总挑战时间
	int					ScoreTimeCoef;				// 时间系数

	float				HeroFlushElapseTime;		// 出现计时

public:
	void initData();
	TConfig getSystemConfig();
	vector<TMap> getMapConfig();
	vector<THero> getHeroConfig();
	vector<TMonster> getMonsterConfig();
	vector<TItem> getItemConfig();
	vector<TSkill> getSkillConfig();
	vector<TReward> getRewardConfig();
	vector<TFrequency> getFreqConfig();

private:
	TConfig stSystemConfig; // 系统配置
	vector<TMap> vMapConfig; // 地图配置列表
	vector<THero> vHeroConfig; // 英雄配置列表
	vector<TMonster> vMonsterConfig; // 怪物配置列表
	vector<TItem> vItemConfig; // 物品配置列表
	vector<TSkill> vSkillConfig; // 技能配置列表
	vector<TReward> vRewardConfig;	// 奖励配置列表
	vector<TFrequency> vFreqConfig; // 刷怪频率列表
};

#endif
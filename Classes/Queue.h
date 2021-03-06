#ifndef SYS_QUEUE
#define SYS_QUEUE

#include "cocos2d.h"

USING_NS_CC;

class Character;

class Queue : public cocos2d::CCNode
{
public:
	virtual void onEnter();
	virtual void onExit();

	CREATE_FUNC(Queue);
	
	/**
	 * @brief 获取队首
	 */
	Character* getHead() const;

	/** 
	 * @brief 是否在队列当中
	 */
	bool isInQueue(Character* pCha) const;

	/** 
	 * @brief 获取前面的队友
	 * @param pCha 就是这货，获取它前面的队友
	 */
	Character* getPrivousCharacter(Character* pCha) const;

	CCArray* getCharacters() const;

	/** 
	 * @brief 设置这个队伍的MoveVector, 将会逐个调用队伍里每个Character的setMoveVector
	 * @param move_vector 要设置的移动向量
	 */
	void setMoveVector(CCPoint move_vector);

	/** 
	 * @brief 将一个Character加在后面
	 */
	void appendCharacter(Character* character);

	void refreshMembers();
	void addAMember(Character* pCha);
	void removeAMember(Character* pCha);

	/** 
	 * @brief 将一个Character从队列中移除
	 */
	bool removeFromQueue(Character* character);

	/** 
	 * @brief 返回一个队尾后面的位置
	 */
	CCPoint getPositionBehindTail() const;

	CCArray* getAllMembers() const;

	bool isLastMember(Character* pCha) const;

	bool isPendingKill(Character* pCha) const;

	int getQueueNum() const;

	void setCurSpeed(float speed);

	void checkChaningSpeed();

	/** 
	 * 给用户手势用， 检查是否需要改变方向
	 */
	void confirmMoveVector();
	void setNextMoveVector(CCPoint moveVec);

	/** 
	 * 都去死吧~~
	 */
	void allGotoDie();

	/// Refresh Begin
public:
	void onUpdate(float dt);
	void clearUpdateFlag();
	void setUpdateSuccess();
	bool isQueueUpdateFinished() const;
	/// Refresh End

	void testUpdate(float dt);


protected:
	CCArray*		m_pCharacters;			// 指向队伍的指针
	CCArray*		m_pPendingAddPool;		// 即将加入队伍中的指针
	CCArray*		m_pPendingKillPool;		// 即将要从队伍中删除的指针
	bool			m_bPendingChangeSpeed;	// 即将改变速度
	float			m_fNextSpeed;			// 即将要改变的速度
	int				m_iUpdateFlag;			// 队员刷新标记，onMove成功就自增一次
	CCPoint			m_vNextMoveVector;		// 给手势用的，更新的时候使用

};

#endif
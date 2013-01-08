/*
 * GameScene.h
 *
 *  Created on: 2012-9-17
 *      Author: arch
 */

#ifndef GAMESCENE_H_
#define GAMESCENE_H_

#include <Box2D/Box2D.h>
#include <cocos2d.h>

#include "Contact/MyContact.h"
#include "Config/GameConstants.h"
#include "Entity/GameSprite.h"

#include <list>
using namespace cocos2d;

//-----------------------------------------------------------------------//
class GameScene: public cocos2d::CCLayer {

public:

	GameScene();
	~GameScene();

	virtual bool init();

	static cocos2d::CCScene* scene();

	void menuCloseCallback(CCObject* pSender);
	void menuBackCallback(CCObject* pSender);

	//触屏事件
	virtual void registerWithTouchDispatcher();

	virtual bool ccTouchBegan(cocos2d::CCTouch *touch,
			cocos2d::CCEvent *pEvent);
	virtual void ccTouchMoved(cocos2d::CCTouch *pTouche,
			cocos2d::CCEvent *pEvent);
	virtual void ccTouchEnded(cocos2d::CCTouch *pTouche,
			cocos2d::CCEvent *pEvent);



	CREATE_FUNC(GameScene);


private:
//	static int count;
	CCSprite* backSprite1;
	CCSprite* backSprite2;
	CCSprite* backSprite3;

	GameSprite* hero;          //TODO 用Advanced Sprite 替代...灰机

	CCPoint m_tBeginPos;

	CCArray *_targets; //敌机
	CCArray *_projectiles; //子弹
	int _projectilesDestroyed;

	//散播子弹

//	void update(CCTime dt);
//	void spriteDone(CCNode* pSender);
	void spriteMoveFinished(CCNode* sender);

	//发射子弹
	void shootBullet();
	void shootPlane(); //发射敌机
	void addHeart();
	void addBackground();
//	void initEnemy();  //随机初始化 敌机
	void updateCollision(); //每周期碰撞检测
	void updateGame();
	void updateHp();  //检查每个对象的生命值

	void updateBackGround();

//	int stepIndex;
	float xdelta;
	float ydelta;

};

#endif /* GAMESCENE_H_ */

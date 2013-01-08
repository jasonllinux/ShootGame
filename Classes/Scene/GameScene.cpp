/*
 * GameScene.cpp
 *
 *  Created on: 2012-9-17
 *      Author: arch
 */

#include "MenuScene.h"
#include "GameScene.h"

#include "SimpleAudioEngine.h"
#include "Utilty/UILayer.h"
#include "Config/GameConstants.h"
#include "Entity/GameSprite.h"
#include "Entity/AdvanceSprite.h"
#include <list>
#include <vector>

using namespace cocos2d;
using namespace CocosDenshion;

#define TILE_SIZE 32
#define PT_RATIO 32

GameScene::GameScene() :_targets(NULL), _projectiles(NULL) ,_projectilesDestroyed(0)
{

}

GameScene::~GameScene() {
	delete hero;

	CC_SAFE_RELEASE_NULL(_targets);
	CC_SAFE_RELEASE_NULL(_projectiles);
//TODO 选择一下
	if (_targets) {
		_targets->release();
		_targets = NULL;
	}

	if (_projectiles) {
		_projectiles->release();
		_projectiles = NULL;
	}

}

CCScene* GameScene::scene() {
	CCScene *scene = CCScene::create();
	GameScene *layer = GameScene::create();
	scene->addChild(layer);
	return scene;
}

bool GameScene::init() {
	if (!CCLayer::init()) {
		return false;
	}
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	addBackground();  //添加设置背景
	addHeart(); //添加血条
//	initEnemy();

	//添加精灵
	hero = new GameSprite("plane.png");
	hero->getSprite()->setPosition(ccp(60,60));
	this->addChild(hero->getSprite(), PLANE_ORDER, TAG_HERO);

	//添加返回按钮
	CCMenuItemFont *backMenu = CCMenuItemFont::create("Back", this, menu_selector(GameScene::menuBackCallback));
	//exit->setAnchorPoint(CCPointZero);
	backMenu->setPosition(ccp(size.width/2, size.height/2));

	CCMenu* pMenu = CCMenu::create(backMenu, NULL);
//	CC_BREAK_IF(!pMenu);
	pMenu->setPosition(CCPointZero);
	this->addChild(pMenu, 1);

	//添加敌方部队
	//schedule_selector
	//每帧调用

	//添加粒子系统
//	this->addParticle();

	schedule(schedule_selector(GameScene::updateBackGround));
	schedule(schedule_selector(GameScene::shootBullet), 0.5);
//	schedule(schedule_selector(GameScene::update), 1.f);
	//更新子弹
//	schedule(schedule_selector(GameScene::shootPlane));
//	schedule(schedule_selector(GameScene::updateCollision)); //碰撞检测
	schedule(schedule_selector(GameScene::updateGame));		//检测碰撞

	this->setTouchEnabled(true);
	return true;
}

//退出所有程序
void GameScene::menuCloseCallback(CCObject* pSender) {
	CCDirector::sharedDirector()->end();

//#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//	exit(0);
//#endif
}

void GameScene::menuBackCallback(CCObject* pSender) {
	CCDirector::sharedDirector()->replaceScene(MenuScene::scene());
}

//注册
void GameScene::registerWithTouchDispatcher() {
	CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
}

bool GameScene::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
	//获取触点坐标
	CCPoint touchLocation = touch->getLocationInView();
	touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);

	m_tBeginPos = touchLocation; //new m_tBeginPos;
	//获取Hero TODO 用tag获取
	CCPoint heroLocation = hero->getSprite()->getPosition();
	CCSize heroSize = hero->getSprite()->getContentSize();
	CCLog("touch...ing....");
//	GameScene::count++;
	return true;  //TODO 是否返回True
}

void GameScene::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent * event) {
	//手指的位置
	CCPoint touchLocation = touch->getLocationInView();
//	CCPoint touchLocation = touch->getPreviousLocationInView();
	touchLocation = CCDirector::sharedDirector()->convertToGL(touchLocation);

	CCNode* heroNode = getChildByTag(TAG_HERO);
	CCPoint heroPoint = heroNode->getPosition();
	CCPoint movePos = ccpSub(touchLocation, m_tBeginPos); //sub
	CCPoint nextPos = ccpAdd(heroPoint, movePos);         //add
	if(nextPos.x<MAX_X&&nextPos.x>MIN_X && nextPos.y<MAX_Y&& nextPos.y>MIN_Y) {
		heroNode->setPosition(nextPos);
	}
	m_tBeginPos = touchLocation;

}

//触屏结束的时候发射子弹
void GameScene::ccTouchEnded(cocos2d::CCTouch * touch, cocos2d::CCEvent * event) {

//	CCLog("touch endddddd");
}


//move finished,删除该节点
void GameScene::spriteMoveFinished(CCNode* sender) {
	CCSprite* sprite = (CCSprite *) sender;
	this->removeChild(sprite, true);

	// 从 tag 判断类型从对应的数组中移除
	if (sprite->getTag() == TAG_TARGET) {
		_targets->removeObject(sprite);
	} else if (sprite->getTag() == TAG_Bullet) {
		_projectiles->removeObject(sprite);
	}
}


//为Hero发射子弹
void GameScene::shootBullet() {

	float moveDuration = 0.2;

	CCPoint heroPos = this->hero->getSprite()->getPosition();   //CAT 就是子弹
	CCSprite *cat = CCSprite::create("bullet.png");
	float speed = (cat->getContentSize().height/2);
	CCSize catSize = cat->getContentSize();
	CCSize heroSize = this->hero->getSprite()->getContentSize();

	float startX = heroPos.x;
	float startY = heroPos.y + heroSize.height/2 + catSize.height/2;

	CCPoint startPos = ccp(startX, startY);
	float deltaY = 0 + catSize.height/2;
	float endY = CCDirector::sharedDirector()->getWinSize().height+ heroSize.height/2;

	cat->setPosition(startPos);
	CCFiniteTimeAction* actionMove = CCMoveTo::create(moveDuration, ccp(startX, endY));    //Attention
	CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this, callfuncN_selector(GameScene::spriteMoveFinished));
	cat->runAction(CCSequence::create(actionMove, actionMoveDone, NULL));
	//
	addChild(cat, PLANE_ORDER, TAG_Bullet);  //TODO Bullet Order
	 _projectiles->addObject(cat); //加入到投射列表（子弹）
}

//发射敌机
void GameScene::shootPlane() {
	CCSprite* target = CCSprite::create("plane2.png");  //敌机
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	int minX = target->getContentSize().width / 2;
	int maxX = winSize.width - target->getContentSize().width / 2;

	int rangeX = maxX - minX;

	int actualX = (rand() % rangeX) + minX;

	target->setPosition(ccp(actualX, winSize.height + (target->getContentSize().height/2)));

	//determine speed
	int minDuration = (int) 2.0;
	int maxDuratoin = (int) 4.0;
	int rangeDuration = maxDuratoin - minDuration;
	int actualDuration = (rand() % rangeDuration) + minDuration;

	//create the actions
	CCFiniteTimeAction* actionMove = CCMoveTo::create((float) actualDuration,
			ccp(actualX, 0-target->getContentSize().height/2));
	CCFiniteTimeAction* actionMoveDone = CCCallFuncN::create(this,
			callfuncN_selector(GameScene::spriteMoveFinished));
	target->runAction(CCSequence::create(actionMove, actionMoveDone, NULL));
	//
	this->addChild(target, PLANE_ORDER, TAG_TARGET);
    //
	 _targets->addObject(target);     //加入到敌机列表（target）

}



//每周期更新滑动背景
void GameScene::updateBackGround() {
	int scroll_speed = 3;
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	CCPoint backPoint1 = backSprite1->getPosition();
	CCPoint backPoint2 = backSprite2->getPosition();
	backSprite1->setPosition(ccp(backPoint1.x, backPoint1.y-scroll_speed));
	backSprite2->setPosition(ccp(backPoint2.x, backPoint2.y-scroll_speed));

	if(backPoint2.y < 0) {
		float temp = backPoint2.y + size.height;
		backSprite1->setPosition(ccp(backPoint1.x, temp));
	}
	if(backPoint1.y < 0) {
		float temp = backPoint1.y + size.height;
		backSprite2->setPosition(ccp(backPoint2.x, temp));
	}

}

void GameScene::addHeart() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
//	CCSprite* heart = CCSprite::create("heart1.png", CCRectMake(0, 0, 40 ,40));
	CCSprite* heart = CCSprite::create("heart1.png");
	int dis = 50; //间隔距离
	heart->setAnchorPoint(ccp(0, 0));
	CCSize heartSize = heart->getContentSize();
	heart->setPosition(ccp(0, winSize.height-heartSize.height));
	this->addChild(heart, 1);

	CCSprite* heart2 = CCSprite::create("heart1.png");
	heart2->setAnchorPoint(ccp(0, 0));
	heart2->setPosition(ccp(dis, winSize.height-heartSize.height));
	this->addChild(heart2, 1);

	CCSprite* heart3 = CCSprite::create("heart1.png");
	heart3->setAnchorPoint(ccp(0, 0));
	heart3->setPosition(ccp(2*dis, winSize.height-heartSize.height));
	this->addChild(heart3, 1);

	CCSprite* heart4 = CCSprite::create("heart1.png");
	heart4->setAnchorPoint(ccp(0, 0));
	heart4->setPosition(ccp(3*dis, winSize.height-heartSize.height));
	this->addChild(heart4, 1);

	CCSprite* heart5 = CCSprite::create("heart1.png");
	heart5->setAnchorPoint(ccp(0, 0));
	heart5->setPosition(ccp(4*dis, winSize.height-heartSize.height));
	this->addChild(heart5, 1);
	CCArray* heartArray = CCArray::create();
	for(int i=0; i<=5; i++) {
		CCSprite* heart = CCSprite::create("heart1.png");

	}
	//TODO 添加血条
//	UILayer* ui = new UILayer();
//	this->addChild(ui, 0);
}

void GameScene::addBackground() {
	CCSize size = CCDirector::sharedDirector()->getWinSize();
		//720 1184
	backSprite1 = CCSprite::create("back1.jpg");
	backSprite2 = CCSprite::create("back1.jpg");
	backSprite1->setAnchorPoint(ccp(0, 0));
	backSprite2->setAnchorPoint(ccp(0, 0));
	backSprite1->setPosition(ccp(0, 0));
	backSprite2->setPosition(ccp(0, size.height));
	this->addChild(backSprite1, 0, BACK_LAYER1);
	this->addChild(backSprite2, 0, BACK_LAYER2);

}


void GameScene::updateCollision() {
	CCArray* projectilesToDelete = new CCArray();
	CCObject* pobject;

	// cocos2d定义的宏，提供方便的只读遍历CCARRAY写法
	CCARRAY_FOREACH(_projectiles, pobject) {
		CCSprite* projectile = (CCSprite*) pobject;
		CCRect pRect = CCRect(
				projectile->getPosition().x
						- projectile->getContentSize().width / 2,
				projectile->getPosition().y
						- projectile->getContentSize().height / 2,
				projectile->getContentSize().width,
				projectile->getContentSize().height);

		CCArray* targetsToDelete = new CCArray();
		CCObject* tobject;
		CCARRAY_FOREACH(_targets, tobject) {
			CCSprite* target = (CCSprite*) tobject;
			CCRect tRect = CCRect(
					target->getPosition().x
							- target->getContentSize().width / 2,
					target->getPosition().y
							- target->getContentSize().height / 2,
					target->getContentSize().width,
					target->getContentSize().height);

			// 碰撞测试
			if (pRect.intersectsRect(tRect)) {
//				CCLog("jfsljflsjflsjfls.......");
				targetsToDelete->addObject(target);
			}
		}

		// 移除被击中的目标
		CCARRAY_FOREACH(targetsToDelete, tobject) {
			CCSprite* target = (CCSprite*) tobject;
			_targets->removeObject(target);
			this->removeChild(target, true);
		}

		// 记录击中目标的子弹
		if (targetsToDelete->count() > 0) {
			targetsToDelete->addObject(projectile);
		}

		/* 由于我们是用的 new CCArray() 而非 CCArray::create()
		 获得的数组对象，所以需要手动调用release */
		targetsToDelete->release();
	}

	// 移除击中目标的子弹
	CCARRAY_FOREACH(projectilesToDelete, pobject) {
		CCSprite* projectile = (CCSprite*) pobject;
		_projectiles->removeObject(projectile, true);
		this->removeChild(projectile, true);
	}

	projectilesToDelete->release();

}

void GameScene::updateGame() {
	CCArray *projectilesToDelete = new CCArray;
	CCObject* it = NULL;
	CCObject* jt = NULL;

	CCARRAY_FOREACH(_projectiles, it) {
		CCSprite *projectile = dynamic_cast<CCSprite*>(it);
		CCRect projectileRect =
				CCRectMake(
						projectile->getPosition().x - (projectile->getContentSize().width/2),
						projectile->getPosition().y - (projectile->getContentSize().height/2),
						projectile->getContentSize().width,
						projectile->getContentSize().height);

		CCArray* targetsToDelete = new CCArray;

		CCARRAY_FOREACH(_targets, jt) {
			CCSprite *target = dynamic_cast<CCSprite*>(jt);
			CCRect targetRect =
					CCRectMake(
							target->getPosition().x - (target->getContentSize().width/2),
							target->getPosition().y - (target->getContentSize().height/2),
							target->getContentSize().width,
							target->getContentSize().height);

			if (projectileRect.intersectsRect(targetRect)) {
				targetsToDelete->addObject(target);
			}
		}

		CCARRAY_FOREACH(targetsToDelete, jt) {
			CCSprite *target = dynamic_cast<CCSprite*>(jt);
			_targets->removeObject(target);
			this->removeChild(target, true);

			_projectilesDestroyed++;
			if (_projectilesDestroyed >= 5) {
				//TODO GameOver Scene 判断什么时候游戏结束
//				GameOverScene *gameOverScene = GameOverScene::create();
//				gameOverScene->getLayer()->getLabel()->setString("You Win!");
//				CCDirector::sharedDirector()->replaceScene(gameOverScene);
			}
		}

		if (targetsToDelete->count() > 0) {
			projectilesToDelete->addObject(projectile);
		}
		targetsToDelete->release();
	}

	CCARRAY_FOREACH(projectilesToDelete, it) {
		CCSprite* projectile = dynamic_cast<CCSprite*>(it);
		_projectiles->removeObject(projectile);
		this->removeChild(projectile, true);
	}
	projectilesToDelete->release();

}


void GameScene::updateHp() {

}




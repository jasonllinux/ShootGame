/*
 * GameConstants.h
 *
 *  Created on: 2012-10-7
 *      Author: arch
 */

#ifndef GAMECONSTANTS_H_
#define GAMECONSTANTS_H_


//记录游戏状态
enum GameState {
	NOT_START,
	STARTED,
	GAME_OVER,
};

//Node的Tag值
enum NodeTag {
	//背景图片
	BACK_LAYER1 = 10,
	BACK_LAYER2 = 11,
	BACK_LAYER3 = 12,

	//精灵类
	TAG_HERO = 1000,
	PLANE_OTHER = 2000,
	TAG_Bullet = 49,
	TAG_TARGET = 50,

};

enum speed {
	SPEED_HERO_X = 5,
	SPEED_HERO_Y = 4,

};

enum move_bound {
	MIN_X = 0,
	MAX_X = 480,
	MIN_Y = 0, //125
	MAX_Y = 800,
};

enum NodeOrder {
	PLANE_ORDER = 2,

	SIMPLE_MENU_ORDER = 5,
	TOP_MENU_ORDER = 1000,
};




#endif /* GAMECONSTANTS_H_ */

/*
 * AdvanceSprite.h
 *
 *  Created on: 2012-11-4
 *      Author: arch
 */

#ifndef ADVANCESPRITE_H_
#define ADVANCESPRITE_H_

#include <cocos2d.h>
using namespace cocos2d;


class AdvanceSprite : public CCSprite
{
public:
	AdvanceSprite();

	virtual ~AdvanceSprite();

	int getHp();
	void setHp(int hp_);

private:
	int hp;
};

#endif /* ADVANCESPRITE_H_ */

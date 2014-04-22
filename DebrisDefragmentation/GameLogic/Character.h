﻿/*
	Character를 상속받은 player
	움직임과 각종 스킬들을 구현한다.
	세부 클래스는 하위 클래스에서 구현할 것
	2014. 4. 21 최경욱
*/
#pragma once

#include "Actor.h"

class Character : public Actor
{
public:
	Character() {}
	virtual ~Character();

	// player 상태 가져오기
	int GetUserId() { return m_UserId; }
	void SetUserId( int uid ) { m_UserId = uid; }

private:
	virtual void UpdateItSelf( float dTime );

	int				m_UserId = -1;
};

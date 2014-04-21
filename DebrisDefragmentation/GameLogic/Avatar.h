﻿#pragma once
#include "GameConfig.h"

const float ACCELERATION_WEIGHT = 1.0f;
const int MOUSE_ROTATION_WEIGHT = 10;

//전방선언
struct RIGIDBODY;

class Avatar
{
public:
	Avatar();
	~Avatar();

	CREATE_FUNC( Avatar );

	// 작성자 : 최경욱
	// 현재 바라보는 방향으로 가속도 부여
	void _GoForward(D3DXVECTOR3 viewDirection, RIGIDBODY& rigidbody);

	// 가속도 및 속도 0으로 변경
	void _Stop( RIGIDBODY& rigidbody );

	// 바라보는 방향 회전
	void _RotateDirection( float x, float y, D3DXVECTOR3& rotation );

	int		GetAvatarId() const { return m_AvatarId; }
	DWORD	GetAccelerationStartTime() const { return m_AccelerationStartTime; }
	void	SetAvatarId( int val ) { m_AvatarId = val; }
	void	SetAccelerationStartTime( DWORD val ) { m_AccelerationStartTime = val; }

	bool	IsAccelerating() const { return m_IsAccelerating; }
	void	SetIsAccelerating( bool val ) { m_IsAccelerating = val; }
	
	
protected:
	int			m_AvatarId = -1;
	bool		m_IsAccelerating = false;
	
	
	DWORD		m_AccelerationStartTime = 0;
	
};

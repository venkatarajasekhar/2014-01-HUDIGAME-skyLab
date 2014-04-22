#pragma once

#include "GameConfig.h"

// 강체 변환관련 변수들을 가짐, 연산은 Physics에서..
// 4.21 김성환
struct Rigidbody
{
	float		m_Mass = 1.0f;
	D3DXVECTOR3	m_Acceleration{ 0.0f, 0.0f, 0.0f };
	D3DXVECTOR3	m_Velocity{ 0.0f, 0.0f, 0.0f };
};
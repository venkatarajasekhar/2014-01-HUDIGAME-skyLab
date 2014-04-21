﻿#include "stdafx.h"
#include "DDScene.h"


DDScene::DDScene():
m_SceneName( L"DefaultSceneName" )
{
}

DDScene::DDScene( std::wstring sceneName ):
m_SceneName(sceneName)
{

}


DDScene::~DDScene()
{
}


KeyState DDScene::GetKeyState( int key )
{
	return DDInputSystem::GetInstance()->GetKeyState( key );
}

DDPoint DDScene::GetMousePosition()
{
	return DDInputSystem::GetInstance()->GetMousePosition();
}
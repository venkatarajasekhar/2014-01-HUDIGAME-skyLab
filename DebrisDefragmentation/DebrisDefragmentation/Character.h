﻿#pragma once
#include "DDModel.h"
class Character :
	public DDModel
{
public:
	Character();
	Character( std::wstring modelPath );
	virtual ~Character();

	CREATE_FUNC_WSTRING( Character, modelPath );
	
private:
	virtual void UpdateItSelf( float dTime );
};


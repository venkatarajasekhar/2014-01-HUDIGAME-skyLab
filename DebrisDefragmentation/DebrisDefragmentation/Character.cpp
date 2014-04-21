#include "stdafx.h"
#include "Character.h"
#include "DDRenderer.h"
#include "DDApplication.h"


Character::Character()
{
}

Character::Character( std::wstring modelPath ) : DDModel( modelPath )
{
}


Character::~Character()
{
}

void Character::UpdateItSelf( float dTime )
{
	UNREFERENCED_PARAMETER(dTime);
}

#include "stdafx.h"
#include "DDObject.h"
#include "DDRenderer.h"

DDObject::DDObject() :
m_pParent( nullptr ),
m_Position( .0f, .0f, .0f ),
m_Rotation( .0f, .0f, .0f ),
m_Scale( .0f, .0f, .0f )
{
}


DDObject::~DDObject()
{
}


void DDObject::Render()
{
	if ( m_Visible == false ) return;
		
	D3DXQUATERNION	quaternionRotation;
	DDRenderer* renderer = DDRenderer::GetInstance();
	
	D3DXMatrixIdentity( &m_Matrix );

	// rotation에서 쿼터니언 생성
	D3DXQuaternionRotationYawPitchRoll( &quaternionRotation, m_Rotation.x, m_Rotation.y, m_Rotation.z );

	// matrix를 affine변환이 적용된 형태로 변환
	D3DXMatrixAffineTransformation(&m_Matrix, NULL, NULL, &quaternionRotation, &m_Position);

	// 자신의 어파인 변환 적용
	renderer->GetDevice()->SetTransform( D3DTS_WORLD, &m_Matrix );

	// 부모의 어파인 변환을 적용
	renderer->GetDevice()->MultiplyTransform( D3DTS_WORLD, &m_pParent->GetMatrix() );
	
	for ( const auto& child : m_ChildList )
	{
		child->Render();		
	}
}

void DDObject::Update( float dTime )
{
	if ( m_Visible == false ) return;

	for ( const auto& child : m_ChildList )
	{
		child->Update( dTime );
	}
}

void DDObject::AddChild( DDObject* object )
{
	object->SetParent( this );
	m_ChildList.push_back( object );
}

void DDObject::RemoveChild( DDObject* object )
{
	for ( auto& iter = m_ChildList.begin(); iter != m_ChildList.end(); iter++ )
	{
		if ( ( *iter ) == object )
		{
			SafeDelete( *iter );
			m_ChildList.erase( iter );
			break;
		}
	}
}

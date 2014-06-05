﻿#include "stdafx.h"
#include "ParticleSystem.h"
#include "DDMacro.h"
#include "GameOption.h"

const DWORD Particle::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;

ParticleSystem::ParticleSystem()
{
	_device = 0;
	_vb = 0;
	_tex = 0;
}


ParticleSystem::~ParticleSystem()
{
	SafeRelease<LPDIRECT3DVERTEXBUFFER9>( _vb );
	SafeRelease<IDirect3DTexture9*>( _tex );
}


bool ParticleSystem::init( IDirect3DDevice9* device, LPCWSTR texFileName )
{
	// vertex buffer's size does not equal the number of particles in our system.  We
	// use the vertex buffer to draw a portion of our particles at a time.  The arbitrary
	// size we choose for the vertex buffer is specified by the _vbSize variable.

	_device = device; // save a ptr to the device

	HRESULT hr = 0;

	hr = device->CreateVertexBuffer(
		_vbSize * sizeof( Particle ),
		D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY,
		Particle::FVF,
		D3DPOOL_DEFAULT, // D3DPOOL_MANAGED can't be used with D3DUSAGE_DYNAMIC 
		&_vb,
		0 );

	if ( FAILED( hr ) )
	{
		//::MessageBox(0, "CreateVertexBuffer() - FAILED", "ParticleSystem", 0);
		return false;
	}
	std::wstring imgPath = L".\\Resources\\3DModel\\";
	imgPath.append( texFileName );

	hr = D3DXCreateTextureFromFileW(
		device,
		imgPath.c_str(),
		&_tex );

	if ( FAILED( hr ) )
	{
		::MessageBox( 0, L"D3DXCreateTextureFromFile() - FAILED", L"ParticleSystem", 0 );
		return false;
	}

	return true;
}

void ParticleSystem::reset()
{
	std::list<Attribute>::iterator i;
	for ( i = _particles.begin(); i != _particles.end(); i++ )
	{
		resetParticle( &( *i ) );
	}
}

void ParticleSystem::addParticle()
{
	Attribute attribute;

	resetParticle( &attribute );

	_particles.push_back( attribute );
}

void ParticleSystem::preRender()
{
	_device->SetRenderState( D3DRS_LIGHTING, false );
	_device->SetRenderState( D3DRS_POINTSPRITEENABLE, true );
	_device->SetRenderState( D3DRS_POINTSCALEENABLE, true );
	_device->SetRenderState( D3DRS_POINTSIZE, FtoDw( _size ) );
	_device->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDw( 0.0f ) );

	// control the size of the particle relative to distance
	_device->SetRenderState( D3DRS_POINTSCALE_A, FtoDw( 0.0f ) );
	_device->SetRenderState( D3DRS_POINTSCALE_B, FtoDw( 0.0f ) );
	_device->SetRenderState( D3DRS_POINTSCALE_C, FtoDw( 1.0f ) );

	// use alpha from texture
	_device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	_device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

	_device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
	_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
}

void ParticleSystem::postRender()
{
	_device->SetRenderState( D3DRS_LIGHTING, true );
	_device->SetRenderState( D3DRS_POINTSPRITEENABLE, false );
	_device->SetRenderState( D3DRS_POINTSCALEENABLE, false );
	_device->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
}

void ParticleSystem::RenderItSelf()
{
	//
	// Remarks:  The render method works by filling a section of the vertex buffer with data,
	//           then we render that section.  While that section is rendering we lock a new
	//           section and begin to fill that section.  Once that sections filled we render it.
	//           This process continues until all the particles have been drawn.  The benifit
	//           of this method is that we keep the video card and the CPU busy.  

	if ( !_particles.empty() )
	{
		//
		// set render states
		//

		preRender();

		_device->SetTexture( 0, _tex );
		_device->SetFVF( Particle::FVF );
		_device->SetStreamSource( 0, _vb, 0, sizeof( Particle ) );

		//
		// render batches one by one
		//

		// start at beginning if we're at the end of the vb
		if ( _vbOffset >= _vbSize )
			_vbOffset = 0;

		Particle* v = 0;

		_vb->Lock(
			_vbOffset    * sizeof( Particle ),
			_vbBatchSize * sizeof( Particle ),
			(void**)&v,
			_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD );

		DWORD numParticlesInBatch = 0;

		//
		// Until all particles have been rendered.
		//
		std::list<Attribute>::iterator i;
		for ( i = _particles.begin(); i != _particles.end(); i++ )
		{
			if ( i->_isAlive )
			{
				//
				// Copy a batch of the living particles to the
				// next vertex buffer segment
				//
				v->_position = i->_position;
				v->_color = (D3DCOLOR)i->_color;
				v++; // next element;

				numParticlesInBatch++; //increase batch counter

				// if this batch full?
				if ( numParticlesInBatch == _vbBatchSize )
				{
					//
					// Draw the last batch of particles that was
					// copied to the vertex buffer. 
					//
					_vb->Unlock();

					_device->DrawPrimitive(
						D3DPT_POINTLIST,
						_vbOffset,
						_vbBatchSize );

					//
					// While that batch is drawing, start filling the
					// next batch with particles.
					//

					// move the offset to the start of the next batch
					_vbOffset += _vbBatchSize;

					// don't offset into memory thats outside the vb's range.
					// If we're at the end, start at the beginning.
					if ( _vbOffset >= _vbSize )
						_vbOffset = 0;

					_vb->Lock(
						_vbOffset    * sizeof( Particle ),
						_vbBatchSize * sizeof( Particle ),
						(void**)&v,
						_vbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD );

					numParticlesInBatch = 0; // reset for new batch
				}
			}
		}

		_vb->Unlock();

		// its possible that the LAST batch being filled never 
		// got rendered because the condition 
		// (numParticlesInBatch == _vbBatchSize) would not have
		// been satisfied.  We draw the last partially filled batch now.

		if ( numParticlesInBatch )
		{
			_device->DrawPrimitive(
				D3DPT_POINTLIST,
				_vbOffset,
				numParticlesInBatch );
		}

		// next block
		_vbOffset += _vbBatchSize;

		//
		// reset render states
		//

		postRender();
	}
}

bool ParticleSystem::isEmpty()
{
	return _particles.empty();
}

bool ParticleSystem::isDead()
{
	std::list<Attribute>::iterator i;
	for ( i = _particles.begin(); i != _particles.end(); i++ )
	{
		// is there at least one living particle?  If yes,
		// the system is not dead.
		if ( i->_isAlive )
			return false;
	}
	// no living particles found, the system must be dead.
	return true;
}

void ParticleSystem::removeDeadParticles()
{
	std::list<Attribute>::iterator i;

	i = _particles.begin();

	while ( i != _particles.end() )
	{
		if ( i->_isAlive == false )
		{
			// erase returns the next iterator, so no need to
			// incrememnt to the next one ourselves.
			i = _particles.erase( i );
		}
		else
		{
			i++; // next in list
		}
	}
}

float ParticleSystem::GetRandomFloat( float lowBound, float highBound )
{
	if ( lowBound >= highBound ) // bad input
		return lowBound;

	// get random float in [0, 1] interval
	float f = ( rand() % 10000 ) * 0.0001f;

	// return float in [lowBound, highBound] interval. 
	return ( f * ( highBound - lowBound ) ) + lowBound;
}

void ParticleSystem::GetRandomVector(
	D3DXVECTOR3* out,
	D3DXVECTOR3* min,
	D3DXVECTOR3* max )
{
	out->x = GetRandomFloat( min->x, max->x );
	out->y = GetRandomFloat( min->y, max->y );
	out->z = GetRandomFloat( min->z, max->z );
}

DWORD ParticleSystem::FtoDw( float f )
{
	return *( (DWORD*)&f );
}

//*****************************************************************************
// Explosion System
//********************

Firework::Firework(  )
{	
	_size = 0.25f;
	_vbSize = 2048;
	_vbOffset = 0;
	_vbBatchSize = 512;
}

void Firework::SetParticles( D3DXVECTOR3* origin, int numParticles )
{
	_origin = *origin;
	for ( int i = 0; i < numParticles; i++ )
		addParticle();
}


void Firework::resetParticle( Attribute* attribute )
{
	attribute->_isAlive = true;
	attribute->_position = _origin;

	D3DXVECTOR3 min = D3DXVECTOR3( -1.0f, -1.0f, -1.0f );
	D3DXVECTOR3 max = D3DXVECTOR3( 1.0f, 1.0f, 1.0f );

	GetRandomVector(
		&attribute->_velocity,
		&min,
		&max );

	// normalize to make spherical
	D3DXVec3Normalize(
		&attribute->_velocity,
		&attribute->_velocity );

	attribute->_velocity *= FIREWORK_PARTICLE_VELOCITY;

	attribute->_color = D3DXCOLOR(
		GetRandomFloat( 0.0f, 1.0f ),
		GetRandomFloat( 0.0f, 1.0f ),
		GetRandomFloat( 0.0f, 1.0f ),
		1.0f );

	attribute->_age = 0.0f;
	attribute->_lifeTime = PARTICLE_LIFETIME; // lives for 2 seconds
}

void Firework::UpdateItSelf( float timeDelta )
{
	std::list<Attribute>::iterator i;

	for ( i = _particles.begin(); i != _particles.end(); i++ )
	{
		// only update living particles
		if ( i->_isAlive )
		{
			i->_position += i->_velocity * timeDelta;

			i->_age += timeDelta;

			if ( i->_age > i->_lifeTime ) // kill 
				i->_isAlive = false;
		}
	}
}

void Firework::preRender()
{
	ParticleSystem::preRender();

	_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
	_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	// read, but don't write particles to z-buffer
	_device->SetRenderState( D3DRS_ZWRITEENABLE, false );
}

void Firework::postRender()
{
	ParticleSystem::postRender();

	_device->SetRenderState( D3DRS_ZWRITEENABLE, true );
}

void Firework::PlayEffect( D3DXVECTOR3 origin )
{
	_origin = origin;
	reset();
}

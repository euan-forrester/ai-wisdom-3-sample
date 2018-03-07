//
// Class to represent our world: just a missile and a target for it to
// steer towards
//

#include "stdafx.h"
#include "Resource.h"
#include "GlView.h"
#include "CWorld.h"

//
// Tuning constants
//

const float MissileStartPositionFactor	= 0.25f;		// Where to start the missile from, as a fraction along our y axis
const float TargetStartPositionFactor	= -0.25f;		// Where to start the target from, as a fraction along our y axis

const float WorldSize					= 7500.0f;//10000.0f;		// Size of the world in world units

const float BackgroundZDepth			= -2.0f;		// Z depth to draw the background at

// Extents of the textures used for the missile and target
const int	MissileTextureHeight		= 64;
const int	MissileTextureWidth			= 32;
const int	MissileTextureBitDepth		= 32;

const int	TargetTextureHeight			= 32;
const int	TargetTextureWidth			= 32;
const int	TargetTextureBitDepth		= 32;

const int	ExplosionTextureHeight		= 64;
const int	ExplosionTextureWidth		= 64;
const int	ExplosionTextureBitDepth	= 32;

CWorld::CWorld()
{
	int i = 0;
	
	CString texture_directory;
	CString missile_texture_filename[NUM_MISSILE_TEXTURES];
	CString target_texture_filename[NUM_TARGET_TEXTURES];

	texture_directory.LoadString(IDS_TEXTURE_DIRECTORY);
	missile_texture_filename[eMISSILE_TEXTURE_NO_FLAME].LoadString(IDS_MISSILE_TEXTURE_NO_FLAME);
	missile_texture_filename[eMISSILE_TEXTURE_FLAME_1].LoadString(IDS_MISSILE_TEXTURE_FLAME_1);
	missile_texture_filename[eMISSILE_TEXTURE_FLAME_2].LoadString(IDS_MISSILE_TEXTURE_FLAME_2);
	missile_texture_filename[eMISSILE_TEXTURE_FLAME_3].LoadString(IDS_MISSILE_TEXTURE_FLAME_3);
	missile_texture_filename[eMISSILE_TEXTURE_EXPLOSION].LoadString(IDS_EXPLOSION_TEXTURE);

	target_texture_filename[eTARGET_TEXTURE_NORMAL].LoadString(IDS_TARGET_TEXTURE);
	target_texture_filename[eTARGET_TEXTURE_EXPLOSION].LoadString(IDS_EXPLOSION_TEXTURE);

	m_Center.x	= 0.0f;
	m_Center.y	= 0.0f;

	m_Missile.SetCurrentWorld(this);
	m_Target.SetCurrentWorld(this);

	for (i = 0; i <= eMISSILE_TEXTURE_FLAME_3; i++)
	{
		m_Missile.SetTexture(texture_directory + missile_texture_filename[i], i, MissileTextureWidth, MissileTextureHeight, MissileTextureBitDepth);
	}

	for (i = 0; i <= eTARGET_TEXTURE_NORMAL; i++)
	{
		m_Target.SetTexture(texture_directory + target_texture_filename[i], i, TargetTextureWidth, TargetTextureHeight, TargetTextureBitDepth);
	}

	m_Missile.SetTexture(texture_directory	+ missile_texture_filename[eMISSILE_TEXTURE_EXPLOSION],	eMISSILE_TEXTURE_EXPLOSION,	ExplosionTextureWidth,	ExplosionTextureHeight,	ExplosionTextureBitDepth);
	m_Target.SetTexture(texture_directory	+ target_texture_filename[eTARGET_TEXTURE_EXPLOSION],	eTARGET_TEXTURE_EXPLOSION,	ExplosionTextureWidth,	ExplosionTextureHeight,	ExplosionTextureBitDepth);
	
	m_Missile.SetTarget(&m_Target);

	ResetMissileAndTarget();
}

CWorld::~CWorld()
{

}

//
// Put the missile and target back at their start positions
//

void CWorld::ResetMissileAndTarget()
{
	m_Missile.Reset();
	m_Target.Reset();
	
	m_Missile.SetPosition(0.0f,	GetSize()	* MissileStartPositionFactor);
	m_Target.SetPosition(0.0f,	GetSize()	* TargetStartPositionFactor);
}

//
// Handle anything that needs to be done before our current timestep
// begins
//

void CWorld::BeginTimestep()
{
	// Reset all of our user desired inputs so that they can be
	// set again by HandleKeyboardState() based on the current
	// state of the keyboard

	m_Missile.SetUserDesiredAcceleration(0.0f);
	m_Missile.SetUserDesiredAngularAcceleration(0.0f);

	m_Target.SetUserDesiredVelocityX(0.0f);
	m_Target.SetUserDesiredVelocityY(0.0f);
}

//
// Move all of our components by timestep seconds.
//

void CWorld::DoTimestep(float timestep)
{
	if (m_Missile.NeedToBeReset() && m_Target.NeedToBeReset())
	{
		ResetMissileAndTarget();
	}
	
	m_Target.Move(timestep);

	m_Missile.Steer(timestep);
	m_Missile.Move(timestep);

	m_Missile.CheckCollisionWithTarget();
}

//
// Handle anything that needs to be done after our current timestep
// ends
//

void CWorld::EndTimestep()
{

}

//
// Width and height of our world in world units
//

float CWorld::GetSize()
{
	return WorldSize;
}

//
// Returns the location of the center of the world
//

CVector2 *CWorld::GetCenter()
{
	return &m_Center;
}

//
// Handles the effects of the state of one key at a time
//

void CWorld::HandleKeyboardState(eKey key, bool state)
{
	switch (key)
	{
		case eKEY_MISSILE_THRUST:
		{
			if (state)
			{
				m_Missile.SetUserDesiredAcceleration(m_Missile.GetMaxAcceleration());
			}

			break;
		}

		case eKEY_MISSILE_TURN_LEFT:
		{
			if (state)
			{
				m_Missile.SetUserDesiredAngularAcceleration(m_Missile.GetMaxAngularAcceleration());
			}

			break;
		}

		case eKEY_MISSILE_TURN_RIGHT:
		{
			if (state)
			{
				m_Missile.SetUserDesiredAngularAcceleration(-m_Missile.GetMaxAngularAcceleration());
			}

			break;
		}

		case eKEY_TARGET_MOVE_LEFT:
		{
			if (state)
			{
				m_Target.SetUserDesiredVelocityX(-m_Target.GetMaxSpeed());
			}

			break;
		}

		case eKEY_TARGET_MOVE_RIGHT:
		{
			if (state)
			{
				m_Target.SetUserDesiredVelocityX(m_Target.GetMaxSpeed());
			}

			break;
		}

		case eKEY_TARGET_MOVE_UP:
		{
			if (state)
			{
				m_Target.SetUserDesiredVelocityY(m_Target.GetMaxSpeed());
			}

			break;
		}

		case eKEY_TARGET_MOVE_DOWN:
		{
			if (state)
			{
				m_Target.SetUserDesiredVelocityY(-m_Target.GetMaxSpeed());
			}

			break;
		}

		default:
		{
			TRACE("Unknown key %d passed into CWorld::HandleKeyboardState()\n", key);

			break;
		}
	}
}

//
// Draw all of the components of our world on the specified view
//
// We've disabled depth testing, so the drawing order matters
//

int CWorld::Draw(CGlView *gl_view)
{
	gl_view->BeginDrawGLScene();

	m_Target.Draw(gl_view);
	m_Missile.Draw(gl_view);

	gl_view->EndDrawGLScene();

	return TRUE;
}
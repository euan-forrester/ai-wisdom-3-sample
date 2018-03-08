//
// Our target -- the thing the missile is trying to steer towards. 
// It has 2 control modes: either keyboard or automatic. 
//
// The desired x and y velocities from the keyboard are passed 
// into SetUserDesiredVelocityX() and SetUserDesiredVelocityY() every 
// timestep. They're either used to set the target's actual velocity, or ignored,
// depending on the target's current control mode.
//
// For automatic movement, the target follows a random path.
//


#ifndef CTARGET_H
#define CTARGET_H

#include "CVector2.h"
#include "Texture.h"

class CWorld;
class CGlView;

// Possible control modes for our target
enum eTargetControlMode
{
    eTARGET_CONTROL_AUTOMATIC = 0,
    eTARGET_CONTROL_KEYBOARD,

    NUM_TARGET_MOVEMENT_MODES,
};

// Possible states our target can be in
enum eTargetState
{
    eTARGET_STATE_MOVING = 0,
    eTARGET_STATE_EXPLODING,
    eTARGET_STATE_FINISHED_EXPLODING,

    NUM_TARGET_STATES,
};

// Possible textures to use to draw our target
enum eTargetTexture
{
    eTARGET_TEXTURE_NORMAL = 0,
    eTARGET_TEXTURE_EXPLOSION,

    NUM_TARGET_TEXTURES,
};

class CTarget
{
public:
    CTarget();
    ~CTarget()                                                                  { }

    void                Reset();

    void                SetCurrentWorld(CWorld *current_world)                  { m_pCurrentWorld = current_world; }

    void                SetControlMode(eTargetControlMode new_control_mode)     { m_ControlMode = new_control_mode; }

    void                SetPosition(float new_position_x, float new_position_y) { m_Position.x = new_position_x; m_Position.y = new_position_y; }
    void                SetPosition(CVector2 *new_position)                     { m_Position = *new_position; }
    CVector2*           GetPosition()                                           { return &m_Position; }

    void                SetUserDesiredVelocityX(float new_velocity_x)           { m_UserDesiredVelocity.x = new_velocity_x; }
    void                SetUserDesiredVelocityY(float new_velocity_y)           { m_UserDesiredVelocity.y = new_velocity_y; }

    void                Move(float timestep);

    void                SetTexture(const char *filename, int index, int width, int height, int bit_depth);
    CTexture*           GetTexture(int index)                                   { return &m_Texture[index]; }

    bool                NeedToBeReset()                                         { return (m_CurrentState == eTARGET_STATE_FINISHED_EXPLODING); }
    eTargetState        GetCurrentState()                                       { return m_CurrentState; }

    void                Explode();

    float               GetSize();
    float               GetMaxAngularVelocity();

    void                SetMaxSpeed(float max_speed)                            { m_MaxSpeed = max_speed; }
    float               GetMaxSpeed()                                           { return m_MaxSpeed; }

    int                 Draw(CGlView *gl_view);

private:                
    float               GetNumSecondsToExplode();

    CWorld*             m_pCurrentWorld;                // The world we exist in

    CVector2            m_UserDesiredVelocity;          // Velocity desired from the user

    CVector2            m_Direction;                    // The current direction we're headed
    CVector2            m_Position;                     // Our current position

    CTexture            m_Texture[NUM_TARGET_TEXTURES]; // Textures used to draw us

    float               m_MaxSpeed;                     // Our maximum speed in world units/s

    float               m_ExplosionTimeLeft;            // If m_CurrentState is eTARGET_STATE_EXPLODING, how many seconds are left before we're finished exploding?

    eTargetControlMode  m_ControlMode;                  // Our current control mode -- either keyboard or automatic
    eTargetState        m_CurrentState;                 // Our current state -- either moving, exploding, or finished exploding
};

#endif

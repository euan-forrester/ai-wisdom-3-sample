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

#include "stdafx.h"
#include "GlView.h"
#include "CWorld.h"
#include "CTarget.h"

//
// Tuning constants
//

const float TargetSize							= 100.0f;		// Size in world units
const float TargetMaxAngularVelocity			= 90.0f;		// Max angular velocity in degrees / second
const float TargetSteerToCenterOfWorldFactor	= 0.00000008f;	// Amount to steer towards the center of the world

const float TargetNumSecondsToExplode			= 1.0f;			// Num seconds it takes our target to explode once it's been hit
const float TargetExplosionSizeFactor			= 15.0f;		// By how many times does each dimension of the target's size increase as it's exploding

//
// Our constructor: set some default values for our state variables
//

CTarget::CTarget()
{
    m_ControlMode			= eTARGET_CONTROL_AUTOMATIC;

    Reset();
}

void CTarget::Reset()
{
    m_CurrentState			= eTARGET_STATE_MOVING;

    m_Direction.x			= (rand() < (RAND_MAX / 2)) ? -1.0f : 1.0f;
    m_Direction.y			= 0.0f;

    m_Position.x			= 0.0f;
    m_Position.y			= 0.0f;

    m_UserDesiredVelocity.x	= 0.0f;
    m_UserDesiredVelocity.y	= 0.0f;
}

//
// Initialises one texture from a .RAW file
//

void CTarget::SetTexture(const char *filename, int index, int width, int height, int bit_depth)
{
    m_Texture[index].ReadFile(filename, width, height, bit_depth);
}

//
// Width and height of our target in world units
//

float CTarget::GetSize()
{
    return TargetSize;
}

//
// Number of seconds it takes our target to explode
// once it's been hit
//

float CTarget::GetNumSecondsToExplode()
{
    return TargetNumSecondsToExplode;
}

//
// Maximum angular speed of our target,
// in degrees per second.
//

float CTarget::GetMaxAngularVelocity()
{
    return TargetMaxAngularVelocity;
}

//
// Causes our target to blow up
//

void CTarget::Explode()
{
    m_CurrentState		= eTARGET_STATE_EXPLODING;
    m_ExplosionTimeLeft	= GetNumSecondsToExplode();
}

//
// Move our target, based on its control mode, by timestep seconds.
//

void CTarget::Move(float timestep)
{
    switch (m_CurrentState)
    {
        case eTARGET_STATE_MOVING:
        {
            //
            // If we're moving, what we do depends on our current control mode
            //

            switch (m_ControlMode)
            {
                case eTARGET_CONTROL_AUTOMATIC:
                {
                    //
                    // For our automatic movement, randomly perturb our current direction
                    // by a small amount, and then move forward at our maximum speed
                    //

                    float max_angular_velocity	= GetMaxAngularVelocity();
                    float max_speed				= GetMaxSpeed();

                    float direction_change		= ((float)rand() / (float)RAND_MAX) * max_angular_velocity * 2.0f;
                    direction_change			-= max_angular_velocity; // Make direction_change be between max_angular_velocity and -max_angular_velocity
                    direction_change			*= timestep;

                    m_Direction.Rotate(direction_change);

                    // Steer towards the center of the world because our motion is
                    // less interesting at its edges. Modulate the steering force
                    // based on distance to the center squared

                    CVector2	vector_to_center_of_world		= *m_pCurrentWorld->GetCenter() - m_Position;
                    float		distance_to_center_of_world		= vector_to_center_of_world.GetLength();

                    CVector2	center_steering					= vector_to_center_of_world;
                    float		center_steering_amount			= distance_to_center_of_world * distance_to_center_of_world * TargetSteerToCenterOfWorldFactor;

                    center_steering.Normalize(center_steering_amount);

                    m_Direction += (center_steering * timestep);
                    m_Direction.Normalize();

                    CVector2 velocity = m_Direction;
                    velocity.Normalize(max_speed);

                    m_Position += velocity * timestep;

                    break;
                }

                case eTARGET_CONTROL_KEYBOARD:
                {
                    //
                    // Update our direction based on our desired velocity
                    //

                    float user_desired_speed	= m_UserDesiredVelocity.GetLength();
                    float max_speed				= GetMaxSpeed();

                    if (user_desired_speed > 0.0f)
                    {
                        m_Direction = m_UserDesiredVelocity;
                        m_Direction.Normalize();
                    }

                    //
                    // Move us based on our current velocity
                    //

                    if (user_desired_speed > max_speed)
                    {
                        m_UserDesiredVelocity.Normalize(max_speed);
                    }

                    m_Position += m_UserDesiredVelocity * timestep;

                    break;
                }

                default:
                {
                    TRACE("Unknown control mode %d for target!\n", m_ControlMode);

                    break;
                }
            }

            break;
        }

        case eTARGET_STATE_EXPLODING:
        {
            m_ExplosionTimeLeft -= timestep;

            if (m_ExplosionTimeLeft < 0.0f)
            {
                m_ExplosionTimeLeft	= 0.0f;
                m_CurrentState		= eTARGET_STATE_FINISHED_EXPLODING;
            }

            // Fall through to the next case
        }

        case eTARGET_STATE_FINISHED_EXPLODING:
        {
            // Nothing to do if we're not moving

            break;
        }

        default:
        {
            TRACE("Unknown target state: %d\n", m_CurrentState);

            break;
        }
    }

    //
    // Simple logic to keep us within the world
    //

    float my_half_size		= GetSize() / 2.0f;
    float world_half_size	= m_pCurrentWorld->GetSize() / 2.0f;

    float furthest_negative	= -world_half_size	+ my_half_size;
    float furthest_positive	= world_half_size	- my_half_size;

    m_Position.x			= Clamp(m_Position.x, furthest_negative, furthest_positive);
    m_Position.y			= Clamp(m_Position.y, furthest_negative, furthest_positive);
}

//
// Draw our target on the specified view
//

int CTarget::Draw(CGlView *gl_view)
{
    eTargetTexture	texture_to_use		= eTARGET_TEXTURE_NORMAL;
    float			target_half_size	= GetSize() / 2.0f;
    float			texture_alpha		= 1.0f;

    switch (m_CurrentState)
    {
        case eTARGET_STATE_MOVING:
        {
            // Nothing to do -- just use the default values above

            break;
        }

        case eTARGET_STATE_EXPLODING:
        {
            texture_to_use = eTARGET_TEXTURE_EXPLOSION;

            // Make the explosion scale and fade over time
            float explosion_fraction_complete	= (GetNumSecondsToExplode() - m_ExplosionTimeLeft) / GetNumSecondsToExplode();

            float min_explosion_size			= target_half_size;
            float max_explosion_size			= min_explosion_size * TargetExplosionSizeFactor;
            target_half_size					= ((max_explosion_size - min_explosion_size) * explosion_fraction_complete) + min_explosion_size;

            texture_alpha						= 1.0f - explosion_fraction_complete;

            break;
        }

        case eTARGET_STATE_FINISHED_EXPLODING:
        {
            return TRUE; // Nothing to draw if we're done exploding

            break;
        }

        default:
        {
            TRACE("Unknown target state: %d\n", m_CurrentState);

            break;
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    if (m_Texture[texture_to_use].GetData() != NULL)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, 4, m_Texture[texture_to_use].GetWidth(),
            m_Texture[texture_to_use].GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
            m_Texture[texture_to_use].GetData());
    }

    glLoadIdentity();
    glTranslatef(m_Position.x, m_Position.y, 0.0f);
    glRotatef(m_Direction.GetAngle(), 0.0f, 0.0f, -1.0f);

    glColor4f(1.0f, 1.0f, 1.0f, texture_alpha);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0); glVertex2f(-target_half_size, -target_half_size);
        glTexCoord2f(0.0, 0.0); glVertex2f(-target_half_size,  target_half_size);
        glTexCoord2f(1.0, 0.0); glVertex2f( target_half_size,  target_half_size);
        glTexCoord2f(1.0, 1.0); glVertex2f( target_half_size, -target_half_size);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return TRUE;
}
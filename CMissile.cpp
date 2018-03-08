//
// Our missile. It has 2 control modes: either keyboard or PID controller. 
//
// The desired forward and angular accelerations from the keyboard are passed 
// into SetUserDesiredAcceleration() and SetUserDesiredAngularAcceleration() every 
// timestep. They're either used to set the missile's actual accelerations, or ignored,
// depending on the missile's current control mode.
//

#include "stdafx.h"
#include "math.h"
#include "GlView.h"
#include "CWorld.h"
#include "CGraph.h"
#include "CMissile.h"

//
// Tuning constants
//

const float MissileHeight                   = 400.0f;   // Height in world units
const float MissileWidth                    = 200.0f;   // Width in world units
const float MissileMaxAngularAcceleration   = 180.0f;   // Max acceleration in degrees / second^2

const float MissileDragFactor               = 0.001f;   // Amount of drag to apply
const float MissileRotationalDragFactor     = 0.005f;   // Amount of angular drag to apply

const float MissileNumSecondsToExplode      = 0.5f;     // Num seconds to explode once target has been hit
const float MissileExplosionSizeFactor      = 1.5f;     // By how many times does each dimension of the missile's size increase as it's exploding

//
// Steering model
//

#define MISSILE_STEERING_MODEL_NUM_CONTROL_POINTS 10

const float MissileSteeringModelMinXValue = 0.0f;
const float MissileSteeringModelMaxXValue = 90.0f;

const float MissileSteeringModelControlPoint[MISSILE_STEERING_MODEL_NUM_CONTROL_POINTS] =
{
    // Process error    // Desired derivative
    // (degrees)        // of process error
                        // (degrees per second)
    /* 0 */             0.0f,
    /* 10 */            -2.0f,
    /* 20 */            -10.0f,
    /* 30 */            -20.0f,
    /* 40 */            -20.0f,
    /* 50 */            -20.0f,
    /* 60 */            -20.0f,
    /* 70 */            -20.0f,
    /* 80 */            -20.0f,
    /* 90 */            -20.0f,
};

//
// Set some default values for our state variables
//

void CMissile::Init()
{
    m_ControlMode               = eMISSILE_CONTROL_PID;
    m_pTarget                   = NULL;

    m_SteeringAdaptiveController.SetCoefficients(0.0f, 0.0f, 0.0f);

    m_RotationalDragFactor      = 0.0f;
    m_MaxAngularAcceleration    = 0.0f;

    m_PidOutputScale            = 1.0f;

    for (int i = 0; i < NUM_MISSILE_TEXTURES; i++)
    {
        m_Texture[i].Free();
    }
}

void CMissile::Reset()
{
    m_CurrentState          = eMISSILE_STATE_FLYING;

    m_Position.x            = 0.0f;
    m_Position.y            = 0.0f;

    m_Direction.x           = 0.0f;
    m_Direction.y           = -1.0f;

    m_AngularVelocity       = 0.0f;
    m_Speed                 = 0.0f;

    m_Acceleration          = 0.0f;
    m_AngularAcceleration   = 0.0f;
}

//
// Initialises one texture from a .RAW file
//

void CMissile::SetTexture(const char *filename, int index, int width, 
                          int height, int bit_depth)
{
    ASSERT((index >= 0) && (index < NUM_MISSILE_TEXTURES));

    m_Texture[index].ReadFile(filename, width, height, bit_depth);
}

//
// Width and height of our missile in world units
//

float CMissile::GetHeight()
{
    return MissileHeight;
}

float CMissile::GetWidth()
{
    return MissileWidth;
}

//
// Number of seconds it takes our missile to explode
// once it's hit its target
//

float CMissile::GetNumSecondsToExplode()
{
    return MissileNumSecondsToExplode;
}

//
// Update our steering PID controller, and set the new forward and 
// angular acceleration of our missile based on its current control mode.
//

void CMissile::Steer(float timestep)
{
    if (m_CurrentState != eMISSILE_STATE_FLYING)
    {
        m_SteeringAdaptiveController.ResetErrorHistory();

        return;
    }

    //
    // Always update our PID controllers, regardless of our current
    // control mode, so that we will have a proper error history when
    // switching from keyboard control to PID control.
    //
    // Note that this may cause problems with integral windup.
    //

    if (m_pTarget)
    {
        //
        // First, figure out our steering, based on the error between
        // our current heading and the direction of the target
        //

        CVector2 vector_to_target = *(m_pTarget->GetPosition()) - m_Position;

        float heading_error = m_Direction.GetAngle() - vector_to_target.GetAngle();

        // Make heading_error be between -180 and 180 degrees
        if (heading_error > 180.0f)
        {
            heading_error -= 360.0f;
        }
        else if (heading_error < -180.0f)
        {
            heading_error += 360.0f;
        }

        // Our model relates the heading error to the desired derivative of the heading error.
        // Thus, our "actual behavior value" is the current derivative of our heading error,
        // except that we need to change the sign so that if the heading error is moving towards
        // positive infinity, actual_heading_behavior should be positive, otherwise it
        // should be negative

        float model_behavior_value  = GetModelBehaviorValue(heading_error);
        float d_term_value          = m_SteeringAdaptiveController.GetTermValue(eD_COEFFICIENT);
        float actual_behavior_value = fabs(d_term_value);

        if ((heading_error * d_term_value) < 0.0f)
        {
            actual_behavior_value = -actual_behavior_value;
        }

        m_SteeringAdaptiveController.SetAdaptationEnabled(m_ControlMode == eMISSILE_CONTROL_ADAPTIVE_PID);

        m_SteeringAdaptiveController.Update(timestep, heading_error, model_behavior_value, actual_behavior_value);
    }

    //
    // Now we're ready to update our current forward and angular acclerations
    // based on our current control mode
    //

    float   desired_acceleration            = 0.0f;
    float   desired_angular_acceleration    = 0.0f;

    switch (m_ControlMode)
    {
        case eMISSILE_CONTROL_ADAPTIVE_PID:
        case eMISSILE_CONTROL_PID:
        {
            desired_acceleration            = GetMaxAcceleration();
            desired_angular_acceleration    = m_SteeringAdaptiveController.GetOutput() * m_PidOutputScale;

            break;
        }

        case eMISSILE_CONTROL_KEYBOARD:
        {
            desired_acceleration            = m_UserDesiredAcceleration;
            desired_angular_acceleration    = m_UserDesiredAngularAcceleration;

            break;
        }

        default:
        {
            TRACE("Unknown missile control mode: %d\n", m_ControlMode);

            break;
        }
    }

    // Make sure that our desired accelerations don't exceed their maximum values
    desired_acceleration                    = Clamp(desired_acceleration,           0.0f,                           GetMaxAcceleration());
    desired_angular_acceleration            = Clamp(desired_angular_acceleration,   -GetMaxAngularAcceleration(),   GetMaxAngularAcceleration());

    SetAcceleration(desired_acceleration);
    SetAngularAcceleration(desired_angular_acceleration);
}

//
// Move our missile, based on its current forward and angular acceleration,
// by timestep seconds.
//

void CMissile::Move(float timestep)
{
    switch (m_CurrentState)
    {
        case eMISSILE_STATE_FLYING:
        {
            //
            // Apply our accelerations
            //

            m_Speed += m_Acceleration * timestep;

            // We're always moving in the direction that we're facing
            CVector2 velocity = m_Direction;
            velocity.Normalize(m_Speed);

            m_AngularVelocity += m_AngularAcceleration * timestep;

            //
            // Model a bit of drag. Drag is proportional to
            // velocity squared.
            //

            // Drag on our speed

            float drag      = -m_Speed * (float)fabs(m_Speed); // Be sure to preserve m_Speed's sign when squaring it
            drag            *= (MissileDragFactor * timestep);

            m_Speed         += drag;

            // Drag on our angular velocity

            float rotational_drag   = -m_AngularVelocity * (float)fabs(m_AngularVelocity); // Be sure to preserve m_Speed's sign when squaring it
            rotational_drag         *= (m_RotationalDragFactor * timestep);

            m_AngularVelocity       += rotational_drag;

            //
            // Update our direction
            //

            float delta_angle = m_AngularVelocity * timestep;

            m_Direction.Rotate(delta_angle);
            m_Direction.Normalize();

            //
            // Update our position
            //

            m_Position += velocity * timestep;

            break;
        }

        case eMISSILE_STATE_EXPLODING:
        {
            m_ExplosionTimeLeft -= timestep;

            if (m_ExplosionTimeLeft < 0.0f)
            {
                m_ExplosionTimeLeft = 0.0f;
                m_CurrentState      = eMISSILE_STATE_FINISHED_EXPLODING;
            }

            // Fall through to the next case
        }

        case eMISSILE_STATE_FINISHED_EXPLODING:
        {
            // We're not moving, so there's nothing to do

            break;
        }

        default:
        {
            TRACE("Unknown missile state: %d\n", m_CurrentState);

            break;
        }
    }

    //
    // Simple logic to keep us within the world
    //

    float my_half_size      = max(GetWidth(), GetHeight()) / 2.0f;
    float world_half_size   = m_pCurrentWorld->GetSize() / 2.0f;

    float furthest_negative = -world_half_size  + my_half_size;
    float furthest_positive = world_half_size   - my_half_size;

    m_Position.x            = Clamp(m_Position.x, furthest_negative, furthest_positive);
    m_Position.y            = Clamp(m_Position.y, furthest_negative, furthest_positive);

    //DumpState();
}

//
// Check to see if we've hit our target, and explode if we have
//

void CMissile::CheckCollisionWithTarget()
{
    bool impact_has_occured = false;

    if ((m_pTarget->GetCurrentState()   == eTARGET_STATE_MOVING) &&
        (GetCurrentState()              == eMISSILE_STATE_FLYING))
    {
        // This is an axis-aligned bounding box test, so we
        // won't take the orientation of our missile into account

        CVector2*   target_position     = m_pTarget->GetPosition();
        float       target_half_size    = m_pTarget->GetSize() / 2.0f;
        float       missile_half_size   = max(GetWidth(), GetHeight()) / 2.0f;

        CVector2    missile_bbox_min(m_Position.x       - missile_half_size,    m_Position.y        - missile_half_size);
        CVector2    missile_bbox_max(m_Position.x       + missile_half_size,    m_Position.y        + missile_half_size);

        CVector2    target_bbox_min(target_position->x  - target_half_size,     target_position->y  - target_half_size);
        CVector2    target_bbox_max(target_position->x  + target_half_size,     target_position->y  + target_half_size);

        // Test if the boxes aren't overlapping, then invert the result
        impact_has_occured = !( (missile_bbox_min.x > target_bbox_max.x)    ||
                                (missile_bbox_min.y > target_bbox_max.y)    ||
                                (missile_bbox_max.x < target_bbox_min.x)    ||
                                (missile_bbox_max.y < target_bbox_max.y));
    }

    if (impact_has_occured)
    {
        m_CurrentState      = eMISSILE_STATE_EXPLODING;
        m_ExplosionTimeLeft = GetNumSecondsToExplode();

        m_pTarget->Explode();
    }
}

//
// Draw our missile on the specified view
//

int CMissile::Draw(CGlView *gl_view)
{
    eMissileTexture texture_to_use      = eMISSILE_TEXTURE_NO_FLAME;
    float           missile_half_width  = GetWidth() / 2.0f;
    float           missile_half_height = GetHeight() / 2.0f;
    float           texture_alpha       = 1.0f;

    switch (m_CurrentState)
    {
        case eMISSILE_STATE_FLYING:
        {
            if (m_Acceleration > 0.1f)
            {
                texture_to_use = (eMissileTexture)((rand() % (eMISSILE_TEXTURE_FLAME_3 - eMISSILE_TEXTURE_FLAME_1 + 1)) + eMISSILE_TEXTURE_FLAME_1);
            }

            break;
        }

        case eMISSILE_STATE_EXPLODING:
        {
            texture_to_use = eMISSILE_TEXTURE_EXPLOSION;

            // Make the explosion scale and fade over time
            float explosion_fraction_complete           = (GetNumSecondsToExplode() - m_ExplosionTimeLeft) / GetNumSecondsToExplode();

            float min_explosion_size                    = max(missile_half_width, missile_half_height);
            float max_explosion_size                    = min_explosion_size * MissileExplosionSizeFactor;
            missile_half_width = missile_half_height    = ((max_explosion_size - min_explosion_size) * explosion_fraction_complete) + min_explosion_size;

            texture_alpha                               = 1.0f - explosion_fraction_complete;

            break;
        }

        case eMISSILE_STATE_FINISHED_EXPLODING:
        {
            return TRUE; // Nothing to draw if we're done exploding

            break;
        }

        default:
        {
            TRACE("Unknown missile state: %d\n", m_CurrentState);

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
    glRotatef(GetAngle(), 0.0f, 0.0f, -1.0f);

    glColor4f(1.0f, 1.0f, 1.0f, texture_alpha);

    glBegin(GL_QUADS);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(-missile_half_width,  missile_half_height);
        glTexCoord2f(0.0f, 0.0f); glVertex2f( missile_half_width,  missile_half_height);
        glTexCoord2f(0.0f, 1.0f); glVertex2f( missile_half_width, -missile_half_height);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(-missile_half_width, -missile_half_height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    return TRUE;
}

//
// Debug printing of our current state
//

void CMissile::DumpState()
{
    TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    TRACE(">>> Position:             [%f, %f]\n",           m_Position.x,   m_Position.y);
    TRACE(">>> Direction:            [%f, %f]\n",           m_Direction.x,  m_Direction.y);
    TRACE(">>> Speed:                %f units/s\n",         m_Speed);
    TRACE(">>> Angular velocity:     %f degrees/s\n",       m_AngularVelocity);
    TRACE(">>> Acceleration:         %f units/s^2\n",       m_Acceleration);
    TRACE(">>> Angular acceleration: %f degrees/s^2\n\n",   m_AngularAcceleration);
    TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

//
// Calculates our desired heading error derivative
//

float CMissile::GetModelBehaviorValue(float heading_error)
{
    // Set up our graph

    CGraph model_graph(MISSILE_STEERING_MODEL_NUM_CONTROL_POINTS, MissileSteeringModelMinXValue, MissileSteeringModelMaxXValue);

    for (int i = 0; i < MISSILE_STEERING_MODEL_NUM_CONTROL_POINTS; i++)
    {
        model_graph.SetControlPoint(i, MissileSteeringModelControlPoint[i]);
    }

    // Now, interpolate our model behavior value

    float model_behavior_value = model_graph.GetValue(fabs(heading_error));

    return model_behavior_value;
}
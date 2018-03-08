//
// Our missile. It has 2 control modes: either keyboard or PID controller. 
//
// The desired forward and angular accelerations from the keyboard are passed 
// into SetUserDesiredAcceleration() and SetUserDesiredAngularAcceleration() every 
// timestep. They're either used to set the missile's actual accelerations, or ignored,
// depending on the missile's current control mode.
//


#ifndef CMISSILE_H
#define CMISSILE_H

#include "CVector2.h"
#include "Texture.h"
#include "CModelReferenceAdaptiveController.h"

class CGlView;
class CWorld;
class CTarget;

// Possible control modes for our missile
enum eMissileControlMode
{
    eMISSILE_CONTROL_ADAPTIVE_PID = 0,
    eMISSILE_CONTROL_PID,
    eMISSILE_CONTROL_KEYBOARD,

    NUM_MISSILE_CONTROL_MODES,
};

// Possible textures to use to draw our missile
enum eMissileTexture
{
    eMISSILE_TEXTURE_NO_FLAME = 0,
    eMISSILE_TEXTURE_FLAME_1,
    eMISSILE_TEXTURE_FLAME_2,
    eMISSILE_TEXTURE_FLAME_3,
    eMISSILE_TEXTURE_EXPLOSION,

    NUM_MISSILE_TEXTURES,
};

// Possible states our missile can be in
enum eMissileState
{
    eMISSILE_STATE_FLYING = 0,
    eMISSILE_STATE_EXPLODING,
    eMISSILE_STATE_FINISHED_EXPLODING,

    NUM_MISSILE_STATES,
};

class CMissile
{
public:
    CMissile()                                                                                  { Init(); Reset(); }
    ~CMissile()                                                                                 { }

    void                                Init();
    void                                Reset();
    void                                SetCurrentWorld(CWorld *current_world)                  { m_pCurrentWorld = current_world; }

    void                                SetControlMode(eMissileControlMode new_control_mode)    { m_ControlMode = new_control_mode; }
    void                                SetTarget(CTarget *new_target)                          { m_pTarget = new_target; }

    void                                ResetSteering()                                         { m_SteeringAdaptiveController.Reset(); }

    void                                SetSteeringAdaptationRule(eAdaptationRule adaptation_rule)                      { m_SteeringAdaptiveController.SetAdaptationRule(adaptation_rule); }
    void                                SetSteeringPIDCoefficients(float new_p, float new_i, float new_d)               { m_SteeringAdaptiveController.SetCoefficients(new_p, new_i, new_d); }
    void                                SetSteeringTimeslice(float timeslice)                                           { m_SteeringAdaptiveController.SetTimeslice(timeslice); }
    void                                SetSteeringCoefficientClamp(ePIDCoefficient coefficient, float min, float max)  { m_SteeringAdaptiveController.SetCoefficientClamp(coefficient, min, max); }
    void                                SetSteeringUpdateThreshold(ePIDCoefficient coefficient, float threshold)        { m_SteeringAdaptiveController.SetUpdateThreshold(coefficient, threshold); }
    void                                SetSteeringAdaptationGain(ePIDCoefficient coefficient, float adaptation_gain)   { m_SteeringAdaptiveController.SetAdaptationGain(coefficient, adaptation_gain); }
    void                                SetSteeringAlpha(ePIDCoefficient coefficient, float alpha)                      { m_SteeringAdaptiveController.SetAlpha(coefficient, alpha); }

    float                               GetSteeringCoefficient(ePIDCoefficient coefficient)                             { return m_SteeringAdaptiveController.GetCoefficient(coefficient); }

    void                                SetUserDesiredAcceleration(float acceleration)                  { m_UserDesiredAcceleration         = acceleration; }
    void                                SetUserDesiredAngularAcceleration(float angular_acceleration)   { m_UserDesiredAngularAcceleration  = angular_acceleration; }

    void                                SetRotationalDragFactor(float rotational_drag_factor)           { m_RotationalDragFactor = rotational_drag_factor; }

    void                                Steer(float timestep);
    void                                Move(float timestep);
    void                                CheckCollisionWithTarget();

    void                                SetPosition(float new_position_x, float new_position_y)     { m_Position.x = new_position_x; m_Position.y = new_position_y; }
    void                                SetPosition(CVector2 *new_position)                         { m_Position = *new_position; }
    CVector2*                           GetPosition()                                               { return &m_Position; }

    bool                                NeedToBeReset()                                             { return (m_CurrentState == eMISSILE_STATE_FINISHED_EXPLODING); }
    eMissileState                       GetCurrentState()                                           { return m_CurrentState; }

    CVector2*                           GetDirection()                                              { return &m_Direction; }
    float                               GetAngle()                                                  { return m_Direction.GetAngle(); }

    void                                SetTexture(const char *filename, int index, int width, int height, int bit_depth);
    CTexture*                           GetTexture(int index)                                       { return &m_Texture[index]; }

    float                               GetHeight();
    float                               GetWidth();

    void                                SetMaxAngularAcceleration(float max_angular_acceleration)   { m_MaxAngularAcceleration = max_angular_acceleration; }
    float                               GetMaxAngularAcceleration()                                 { return m_MaxAngularAcceleration; }

    void                                SetMaxAcceleration(float max_acceleration)                  { m_MaxAcceleration = max_acceleration; }
    float                               GetMaxAcceleration()                                        { return m_MaxAcceleration; }

    void                                SetPIDOutputScale(float pid_output_scale)                   { m_PidOutputScale = pid_output_scale; }

    int                                 Draw(CGlView *gl_view);

    void                                DumpState();

private:
    float                               GetNumSecondsToExplode();

    void                                SetAcceleration(float acceleration)                         { m_Acceleration        = acceleration; }
    void                                SetAngularAcceleration(float angular_acceleration)          { m_AngularAcceleration = angular_acceleration; }

    float                               GetModelBehaviorValue(float heading_error);

    eMissileControlMode                 m_ControlMode;                      // Our current control mode (PID or keyboard)
    eMissileState                       m_CurrentState;                     // Our current state (flying, exploding, or finished exploding)

    float                               m_UserDesiredAcceleration;          // Acceleration forward that the user at the keyboard would like us to have. Will be ignored if in PID control mode
    float                               m_UserDesiredAngularAcceleration;   // Angular acceleration that the user at the keyboard would like us to have. Will be ignored if in PID control mode

    float                               m_Acceleration;                     // Current acceleration forward in distance/s^2
    float                               m_AngularAcceleration;              // Current angular accleration in degrees/s^2
    float                               m_MaxAcceleration;                  // Maximum acceleration allowed in world units/s^2
    float                               m_RotationalDragFactor;             // Drag to apply to the missile's angular velocity
    float                               m_MaxAngularAcceleration;           // Maximum angular acceleration allowed in degrees/s^2

    CVector2                            m_Position;                         // Current position of the missile
    float                               m_Speed;                            // Current speed of the missile (always in the direction of m_Direction)
    CVector2                            m_Direction;                        // Current direction the missile is facing
    float                               m_AngularVelocity;                  // Current rate of change of m_Direction in degrees/second. +ve for counterclockwise, -ve for clockwise

    float                               m_ExplosionTimeLeft;                // If m_CurrentState is eMISSILE_STATE_EXPLODING, how many seconds are left before we're finished exploding?

    CModelReferenceAdaptiveController   m_SteeringAdaptiveController;       // Our adaptive PID controller for steering
    float                               m_PidOutputScale;                   // Scale to apply to our PID output

    CTexture                            m_Texture[NUM_MISSILE_TEXTURES];    // Texture to use when drawing this missile

    CWorld*                             m_pCurrentWorld;                    // World that we reside in
    CTarget*                            m_pTarget;                          // The target that we're trying to hit
};

#endif
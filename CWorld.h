//
// Class to represent our world: just a missile and a target for it to
// steer towards
//

#ifndef CWORLD_H
#define CWORLD_H

#include "CMissile.h"
#include "CTarget.h"
#include "Texture.h"

class CGlView;

// List of all of the keys that we're interested in
enum eKey
{
    eKEY_MISSILE_THRUST = 0,
    eKEY_MISSILE_TURN_LEFT,
    eKEY_MISSILE_TURN_RIGHT,

    eKEY_TARGET_MOVE_LEFT,
    eKEY_TARGET_MOVE_RIGHT,
    eKEY_TARGET_MOVE_UP,
    eKEY_TARGET_MOVE_DOWN,

    NUM_KEYS,
};

class CWorld
{
public:
    CWorld();
    ~CWorld();

    void                BeginTimestep();
    void                DoTimestep(float timestep);
    void                EndTimestep();

    void                HandleKeyboardState(eKey key, bool state);

    float               GetSize();
    CVector2*           GetCenter();

    CMissile*           GetMissile()                                { return &m_Missile; }
    CTarget*            GetTarget()                                 { return &m_Target; }

    int                 Draw(CGlView *gl_view);

private:
    void                ResetMissileAndTarget();

    CVector2            m_Center;                   // Location of the center of our world

    CMissile            m_Missile;                  // Our missile
    CTarget             m_Target;                   // The target the missile is steering towards
};

#endif

//
// Simple 2D vector class
//

#ifndef CVECTOR2_H
#define CVECTOR2_H

class CVector2
{
public:
    CVector2()  { }
    CVector2(float initial_x, float initial_y);
    ~CVector2() { }

    CVector2    operator+(const CVector2 &v1);
    CVector2&   operator+=(const CVector2 &v1);
    CVector2    operator-(const CVector2 &v1);
    CVector2    operator-();
    CVector2    operator*(float scale);
    CVector2&   operator*=(float scale);

    float       GetLength();
    float       GetAngle();     // Returns angle in degrees
    void        Rotate(float angle_in_degrees);
    void        Normalize(float new_length = 1.0f);

    float       x;
    float       y;
};

extern float    GetDistanceBetween(CVector2 *v1, CVector2 *v2);
extern float    GetDotProduct(CVector2 *v1, CVector2 *v2);
extern float    Clamp(float current_value, float min_value, float max_value);
extern float    Sign(float x);
extern bool     Equal(float x1, float x2);

#endif
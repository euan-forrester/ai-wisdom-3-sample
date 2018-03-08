//
// Simple 2D vector class
//

#include "stdafx.h"
#include "math.h"
#include "CVector2.h"

#define PI 3.14159265f

#define RAD2DEG(x) ((x) * 180.0f / PI)	// Convert radians to degrees
#define DEG2RAD(x) ((x) * PI / 180.0f)	// Convert degrees to radians

CVector2::CVector2(float initial_x, float initial_y) : x(initial_x), y(initial_y)
{

}

CVector2 CVector2::operator+(const CVector2 &v1)
{
    CVector2 v2(x + v1.x, y + v1.y);

    return v2;
}

CVector2 &CVector2::operator+=(const CVector2 &v1)
{
    x += v1.x;
    y += v1.y;

    return *this;
}

CVector2 CVector2::operator-(const CVector2 &v1)
{
    CVector2 v2(x - v1.x, y - v1.y);

    return v2;
}

CVector2 CVector2::operator-()
{
    CVector2 v1(-x, -y);

    return v1;
}

CVector2 CVector2::operator*(float scale)
{
    CVector2 v1(x * scale, y * scale);

    return v1;
}

CVector2 &CVector2::operator*=(float scale)
{
    x *= scale;
    y *= scale;

    return *this;
}

float CVector2::GetLength()
{
    return (float)sqrt(GetDotProduct(this, this));
}

//
// Returns the angle this vector is pointing at, between 0.0 and 359.9 degrees.
// 0.0 means pointing fully along the x axis. 
//

float CVector2::GetAngle()
{
    return RAD2DEG((float)atan2(x, y));
}

//
// Rotate this vector by angle_in_degrees. Positive values are counterclockwise,
// negative values are clockwise
//

void CVector2::Rotate(float angle_in_degrees)
{
    float angle_in_radians	= DEG2RAD(angle_in_degrees);

    float cosine			= (float)cos(angle_in_radians);
    float sine				= (float)sin(angle_in_radians);

    float new_x				= (x * cosine)	- (y * sine);
    float new_y				= (x * sine)	+ (y * cosine);

    x						= new_x;
    y						= new_y;
}

//
// Make this vector have length new_length
//

void CVector2::Normalize(float new_length)
{
    float current_length = GetLength();

    if (fabs(current_length) > 0.0001f)
    {
        float scale = new_length / current_length;

        x *= scale;
        y *= scale;
    }
    else
    {
        x = new_length;
        y = 0.0f;
    }
}

//
// Returns the distance between two position vectors
//

float GetDistanceBetween(CVector2 *v1, CVector2 *v2)
{
    CVector2 difference = *v1 - *v2;

    return difference.GetLength();
}

//
// Returns the dot product of two vectors
//

float GetDotProduct(CVector2 *v1, CVector2 *v2)
{
    return (v1->x * v2->x) + (v1->y * v2->y);
}

//
// Clamp current_value to be between min_value and max_value
//

float Clamp(float current_value, float min_value, float max_value)
{
    return min(max(current_value, min_value), max_value);
}

//
// Return the sign of x
//

float Sign(float x)
{
    if (x > 0.0f)
    {
        return 1.0f;
    }
    else if (x < 0.0f)
    {
        return -1.0f;
    }
    else
    {
        return 0.0f;
    }
}

//
// Return true if x1 is within some epsilon of x2
//

bool Equal(float x1, float x2)
{
    return (fabs(x1 - x2) < 0.001f);
}
//
// Implements a graph with a fixed number of control points, and linear interpolation between them
//
// Initialize with the number of control points you want to use, and the values along the X axis
// that they span over. Then, set each one with SetControlPoint(). Lastly, you can get the Y
// value for any X by calling GetYValue().
//
// As an example, say you wanted 3 control points over a range from 0 to 10 along the X axis. That
// would mean that you have control points at X = 0, 5, and 10. You could then set the
// Y values at each of those points using SetControlPoint() to be 2, 4, and 8. This would mean that
// your graph consisted of the points (0, 2), (5, 4), and (10, 8). If you called GetYValue(2.5), 
// it would return 3.0.
//

#include "stdafx.h"

class CGraph
{
public:
    CGraph(int num_control_points, float min_x_value, float max_x_value);
    ~CGraph();

    void	SetControlPoint(int index, float y_value)		{ ASSERT((index >= 0) && (index < m_NumControlPoints)); m_pControlPoint[index] = y_value; }
    float	GetValue(float x_value);

private:
    float	m_MinXValue;
    float	m_MaxXValue;

    int		m_NumControlPoints;
    float*	m_pControlPoint;
};
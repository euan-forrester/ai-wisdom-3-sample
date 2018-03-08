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

#include "CGraph.h"
#include "CVector2.h"

CGraph::CGraph(int num_control_points, float min_x_value, float max_x_value)
{
    m_NumControlPoints	= num_control_points;
    m_MinXValue			= min_x_value;
    m_MaxXValue			= max_x_value;

    m_pControlPoint		= new float[m_NumControlPoints];

    ASSERT(m_NumControlPoints > 1);
    ASSERT((m_MinXValue < m_MaxXValue) && !Equal(m_MinXValue, m_MaxXValue));
}

CGraph::~CGraph()
{
    delete [] m_pControlPoint;
}

float CGraph::GetValue(float x_value)
{
    //
    // First, check if our x value is outside of the range
    //

    if (x_value <= m_MinXValue)
    {
        return m_pControlPoint[0];
    }
    else if (x_value >= m_MaxXValue)
    {
        return m_pControlPoint[m_NumControlPoints - 1];
    }
    else
    {
        //
        // Our x value is inside of our range, so we must linearily interpolate
        //

        float	exact_control_point			= (x_value - m_MinXValue) / ((m_MaxXValue - m_MinXValue) / (float)(m_NumControlPoints - 1));
        int		left_index					= (int)exact_control_point;
        int		right_index					= left_index + 1;
        float	fractional_control_point	= exact_control_point - (float)left_index;

        float y_value = m_pControlPoint[left_index] + (m_pControlPoint[right_index] - m_pControlPoint[left_index]) * fractional_control_point;

        return y_value;
    }
}

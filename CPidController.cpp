//
// Class to represent a PID Controller. 
//
// To use, first set the P, I, and D coefficients with SetCoefficients(), 
// then begin recording error values with Record(). Use GetOutput()
// to calculate the current output of the controller.
//

#include "stdafx.h"
#include "CPidController.h"

//
// Reset our controller to contain no error terms
//

void CPidController::Clear()
{
    m_CurrentIndex		= -1;
    m_PreviousIndex		= -1;
    m_NumErrorsRecorded	= 0;

    m_CurrentIntegral	= 0.0f;

    for (int i = 0; i < NUM_ERROR_SLOTS; i++)
    {
        m_Error[i]		= 0.0f;
        m_Timestep[i]	= 0.0f;
    }
}

//
// Record an error term and its associated timestep.
//
// Update our current integral value as well. The integral of a
// function is the area under its curve. In this case, we have NUM_ERROR_SLOTS
// samples of values from our error function, and wish to approximate its 
// integral. We see that we have a series of samples and their associated 
// timesteps. Each sample/timestep pair can represent a rectangle of area
// on our graph. By summing together these areas, we can approximate the integral
// of our error function.
//
// The limit as the size of the timesteps approach zero is the actual integral
// of the error function.
//
// For more information about numerical integration, please see chapter 4 of
// the Numerical Recipes books, available online at http://www.nr.com/
//

void CPidController::Record(float error, float timestep)
{
    m_PreviousIndex				= m_CurrentIndex;
    m_CurrentIndex				= (m_CurrentIndex + 1) % NUM_ERROR_SLOTS;

    if (m_NumErrorsRecorded == NUM_ERROR_SLOTS)
    {
        m_CurrentIntegral		-= m_Error[m_CurrentIndex] * m_Timestep[m_CurrentIndex];
    }

    m_Error[m_CurrentIndex]		= error;
    m_Timestep[m_CurrentIndex]	= timestep;

    m_CurrentIntegral			+= error * timestep;

    m_NumErrorsRecorded			= min(m_NumErrorsRecorded + 1, NUM_ERROR_SLOTS);
}

//
// Returns the last error term recorded
//

float CPidController::GetError()
{
    if (m_NumErrorsRecorded >= 1)
    {
        ASSERT((m_CurrentIndex >= 0) && (m_CurrentIndex < NUM_ERROR_SLOTS));

        return m_Error[m_CurrentIndex];
    }
    else
    {
        return 0.0f;
    }
}

//
// Returns the integral of the last NUM_ERROR_SLOTS error terms recorded.
//
// See CPidController::Record() for how m_CurrentIntegral is calculated.
// Alternately, see CPidController::CalculateIntegralDirectly() for
// how to calculate the integral from scratch.
//

float CPidController::GetErrorIntegral()
{
    return m_CurrentIntegral;
}

//
// Returns the current instantaneous derivative the error.
//
// The derivative of a variable is its rate of change. We can estimate its
// current rate of change by dividing the difference between two successive 
// samples of the variable by the time elapsed between them.
//
// The limit as the size of the timesteps approaches zero is the actual derivative
// of the error function.
//

float CPidController::GetErrorDerivative()
{
    if (m_NumErrorsRecorded >= 2)
    {
        ASSERT((m_CurrentIndex	>= 0)	&& (m_CurrentIndex	< NUM_ERROR_SLOTS));
        ASSERT((m_PreviousIndex	>= 0)	&& (m_PreviousIndex	< NUM_ERROR_SLOTS));

        float difference	= m_Error[m_CurrentIndex] - m_Error[m_PreviousIndex];
        float time_interval	= m_Timestep[m_CurrentIndex];

        if (time_interval > 0.001f)
        {
            return (difference / time_interval);
        }
        else
        {
            return 999999.0f;
        }
    }
    else
    {
        return 0.0f;
    }
}

//
// Calculates the current output of our controller
//

float CPidController::GetOutput()
{
    return	m_P_Coefficient * GetError()			+
            m_I_Coefficient * GetErrorIntegral()	+
            m_D_Coefficient * GetErrorDerivative();
}

//
// Used for debugging to determine the amount of floating point
// error that has accumulated in m_CurrentIntegral, by calculating
// the current integral from scratch.
//
// See CPidController::Record() for an explanation of approximating
// the integral of a variable.
//

float CPidController::CalculateIntegralDirectly()
{
    float integral = 0.0f;

    for (int i = 0; i < m_NumErrorsRecorded; i++)
    {
        integral += (m_Error[i] * m_Timestep[i]);
    }

    return integral;
}

//
// Debug printing of our current state
//

void CPidController::DumpState()
{
    TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
    TRACE(">>> Num errors recorded: %d. Current index: %d. Previous index: %d.\n", m_NumErrorsRecorded, m_CurrentIndex, m_PreviousIndex);

    for (int i = 0; i < NUM_ERROR_SLOTS; i++)
    {
        TRACE(">>> Slot %d: Error: %f Timestep: %f\n", i, m_Error[i], m_Timestep[i]);
    }

    TRACE(">>>\n");
    TRACE(">>> Current P term: Coefficient %f * Error      %f = %f\n", m_P_Coefficient, GetError(),				m_P_Coefficient * GetError());
    TRACE(">>> Current I term: Coefficient %f * Integral   %f = %f\n", m_I_Coefficient, GetErrorIntegral(),		m_I_Coefficient * GetErrorIntegral());
    TRACE(">>> Current D term: Coefficient %f * Derivative %f = %f\n", m_D_Coefficient, GetErrorDerivative(),	m_D_Coefficient * GetErrorDerivative());
    TRACE(">>>\n");
    TRACE(">>> Final output: %f\n", GetOutput());
    TRACE(">>> Integral: %f. Calculated directly: %f\n", GetErrorIntegral(), CalculateIntegralDirectly());
    TRACE(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

//
// Class to represent a PID Controller. 
//
// To use, first set the P, I, and D coefficients with SetCoefficients(), 
// then begin recording error values with Record(). Use GetOutput()
// to calculate the current output of the controller.
//

#ifndef CPIDCONTOLLER_H
#define CPIDCONTOLLER_H

#define NUM_ERROR_SLOTS 10

class CPidController
{
public:
	CPidController()						{ SetCoefficients(0.0f, 0.0f, 0.0f); Clear(); }
	~CPidController()						{ }

	void	SetCoefficients(float p_coefficient, float i_coefficient, float d_coefficient) { m_P_Coefficient = p_coefficient; m_I_Coefficient = i_coefficient; m_D_Coefficient = d_coefficient; }

	void	Record(float error, float timestep);

	void	Clear();
	bool	IsFull()						{ return (m_NumErrorsRecorded == NUM_ERROR_SLOTS); }

	float	GetError();
	float	GetErrorIntegral();
	float	GetErrorDerivative();

	float	GetOutput();

	void	DumpState();

private:
	float	CalculateIntegralDirectly();

	float	m_P_Coefficient;				// Our current P coefficient
	float	m_I_Coefficient;				// Our current I coefficient
	float	m_D_Coefficient;				// Our current D coefficient

	float	m_Error[NUM_ERROR_SLOTS];		// The last NUM_ERROR_SLOTS error terms that have been recorded
	float	m_Timestep[NUM_ERROR_SLOTS];	// The last NUM_ERROR_SLOTS timesteps that have been recorded

	int		m_CurrentIndex;					// Index into m_Error[] anmd m_Timestep[] of the last recorded error
	int		m_PreviousIndex;				// Index into m_Error[] anmd m_Timestep[] of the next-to-last recorded error
	int		m_NumErrorsRecorded;			// Number of error values that have been recorded so far. Between 0 and NUM_ERROR_SLOTS.

	float	m_CurrentIntegral;				// The current value of our integral term
};

#endif
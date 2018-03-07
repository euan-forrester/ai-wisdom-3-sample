//
// Class to represent a Model Reference Adaptive Controller.
//
// To use, first set it up with SetAdaptationRule(), SetTimeslice(), SetClamp(), SetUpdateThreshold(),
// SetAdaptationGain(), and SetAlpha() (used with the Normalized MIT Rule only), then
// every frame call SetModelBehaviorValue() with whatever value your model outputs, then Update(). 
// The result can be gotten with GetOutput()
//

#include "CPidController.h"

enum ePIDCoefficient
{
	eP_COEFFICIENT = 0,
	eI_COEFFICIENT,
	eD_COEFFICIENT,

	NUM_PID_COEFFICIENTS,
};

enum eAdaptationRule
{
	eADAPT_MIT_RULE = 0,
	eADAPT_SIGN_SIGN_RULE,
	eADAPT_SIGN_DATA_RULE,
	eADAPT_SIGN_ERROR_RULE,
	eADAPT_NORMALIZED_MIT_RULE,

	NUM_UPDATE_RULES,
};

class CModelReferenceAdaptiveController
{
public:
	CModelReferenceAdaptiveController()														{ Reset(); }
	~CModelReferenceAdaptiveController()													{ }

	void			Reset();
	void			ResetErrorHistory()														{ m_PidController.Clear(); }

	void			Update(float timestep, float process_error, float model_behavior_value, float actual_behavior_value);
	float			GetOutput()																{ return m_PidController.GetOutput(); }
	float			GetCoefficient(ePIDCoefficient coefficient)								{ return m_Coefficient[coefficient]; }
	float			GetTermValue(ePIDCoefficient coefficient);

	void			SetAdaptationEnabled(bool adaptation_enabled)							{ m_AdaptationEnabled = adaptation_enabled; }
	void			SetAdaptationRule(eAdaptationRule adaptation_rule)						{ m_AdaptationRule = adaptation_rule; }
	void			SetTimeslice(float timeslice)											{ m_Timeslice = timeslice; }
	void			SetCoefficientClamp(ePIDCoefficient coefficient, float min, float max)	{ m_MinCoefficient[coefficient] = min; m_MaxCoefficient[coefficient] = max; }
	void			SetUpdateThreshold(ePIDCoefficient coefficient, float threshold)		{ m_UpdateThreshold[coefficient] = threshold; }
	void			SetAdaptationGain(ePIDCoefficient coefficient, float adaptation_gain)	{ m_AdaptationGain[coefficient] = adaptation_gain; }
	void			SetAlpha(ePIDCoefficient coefficient, float alpha)						{ m_Alpha[coefficient] = alpha; }

	void			SetCoefficients(float p_coefficient, float i_coefficient, float d_coefficient);
	
private:
	float			GetCoefficientDerivative(ePIDCoefficient current_term, float model_error, float timestep);
	float			GetSensitivityDerivative(ePIDCoefficient current_term, float model_error, float timestep);

	// Current state
	float			m_TotalTimeElapsed;
	float			m_Coefficient[NUM_PID_COEFFICIENTS];
	float			m_PreviousModelError;
	float			m_PreviousCoefficientDerivative[NUM_PID_COEFFICIENTS];
	bool			m_AdaptationEnabled;
    
	// Tuning values
	eAdaptationRule	m_AdaptationRule;
	float			m_Timeslice;
	float			m_AdaptationGain[NUM_PID_COEFFICIENTS];
	float			m_UpdateThreshold[NUM_PID_COEFFICIENTS];
	float			m_Alpha[NUM_PID_COEFFICIENTS];
	float			m_MinCoefficient[NUM_PID_COEFFICIENTS];
	float			m_MaxCoefficient[NUM_PID_COEFFICIENTS];

	CPidController	m_PidController;
};
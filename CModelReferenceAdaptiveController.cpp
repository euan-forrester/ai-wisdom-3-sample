//
// Class to represent a Model Reference Adaptive Controller.
//
// To use, first set it up with SetAdaptationRule(), SetTimeslice(), SetClamp(), SetUpdateThreshold(),
// SetAdaptationGain(), and SetAlpha() (used with the Normalized MIT Rule only), then
// every frame call SetModelBehaviorValue() with whatever value your model outputs, then Update(). 
// The result can be gotten with GetOutput()
//

#include "stdafx.h"
#include "math.h"
#include "CVector2.h"

#include "CModelReferenceAdaptiveController.h"

void CModelReferenceAdaptiveController::Reset()
{
	m_AdaptationRule						= eADAPT_MIT_RULE;
	m_Timeslice								= 0.0f;
	m_TotalTimeElapsed						= 0.0f;
	m_PreviousModelError					= 0.0f;
	m_AdaptationEnabled						= true;

	for (int i = 0; i < NUM_PID_COEFFICIENTS; i++)
	{
		m_Coefficient[i]					= 0.0f;
		m_AdaptationGain[i]					= 0.0f;
		m_UpdateThreshold[i]				= 0.0f;
		m_Alpha[i]							= 0.0f;
		m_MinCoefficient[i]					= 0.0f;
		m_MaxCoefficient[i]					= 0.0f;
		m_PreviousCoefficientDerivative[i]	= 0.0f;
	}

	m_PidController.Clear();
}

void CModelReferenceAdaptiveController::Update(float timestep, float process_error, 
											   float model_behavior_value, float actual_behavior_value)
{
	m_PidController.Record(process_error, timestep);

	m_TotalTimeElapsed += timestep;

	// Update each coefficient using a round-robin system

	ePIDCoefficient	current_term			= (ePIDCoefficient)((int)(m_TotalTimeElapsed / m_Timeslice) % NUM_PID_COEFFICIENTS);
	float			current_term_value		= GetTermValue(current_term);
	float			model_error				= actual_behavior_value - model_behavior_value;
	float			coefficient_derivative	= 0.0f;

	//TRACE("Heading error: %f. Actual: %f. Model: %f Model error: %f\n", process_error, actual_behavior_value, model_behavior_value, model_error);

	// Make sure that the term is big enough to tune (see the section 
	// Instability Resulting from Lack of Excitation)

	if ((fabs(current_term_value) > m_UpdateThreshold[current_term]) && m_AdaptationEnabled)
	{
		coefficient_derivative = GetCoefficientDerivative(current_term, model_error, timestep);

		//TRACE("\tUpdating coefficient %d. Prev. value: %f. Derivative: %f\n", current_term, m_Coefficient[current_term], coefficient_derivative); 

		m_Coefficient[current_term] += coefficient_derivative * timestep;

		// Clamp each coefficient to prevent the arms race problem discussed in 
		// Calculating the Sensitivity Derviative

		m_Coefficient[current_term] = Clamp(m_Coefficient[current_term], m_MinCoefficient[current_term], m_MaxCoefficient[current_term]);

		//TRACE("\tNew value: %f\n", m_Coefficient[current_term]);
	}

	// Now we can update our coefficients

	m_PidController.SetCoefficients(m_Coefficient[eP_COEFFICIENT], m_Coefficient[eI_COEFFICIENT], m_Coefficient[eD_COEFFICIENT]);

	// And remember the previous values of our coefficient derivative and model error

	for (int i = 0; i < NUM_PID_COEFFICIENTS; i++)
	{
		m_PreviousCoefficientDerivative[i] = 0.0f;
	}

	m_PreviousCoefficientDerivative[current_term] = coefficient_derivative;

	m_PreviousModelError = model_error;
}

void CModelReferenceAdaptiveController::SetCoefficients(float p_coefficient, float i_coefficient, float d_coefficient)	
{ 
	m_Coefficient[eP_COEFFICIENT] = p_coefficient;
	m_Coefficient[eI_COEFFICIENT] = i_coefficient;
	m_Coefficient[eD_COEFFICIENT] = d_coefficient;
		
	m_PidController.SetCoefficients(p_coefficient, i_coefficient, d_coefficient); 
}

float CModelReferenceAdaptiveController::GetTermValue(ePIDCoefficient coefficient)
{
	switch (coefficient)
	{
		case eP_COEFFICIENT:
		{
			return m_PidController.GetError();

			break;
		}

		case eI_COEFFICIENT:
		{
			return m_PidController.GetErrorIntegral();

			break;
		}

		case eD_COEFFICIENT:
		{
			return m_PidController.GetErrorDerivative();

			break;
		}

		default:
		{
			// Unknown term
			ASSERT(0);

			break;
		}
	};

	return 0.0f;
}

float CModelReferenceAdaptiveController::GetCoefficientDerivative(ePIDCoefficient current_term, 
																  float model_error, float timestep)
{
	float adaptation_gain			= m_AdaptationGain[current_term];
	float sensitivity_derivative	= GetSensitivityDerivative(current_term, model_error, timestep);

	switch (m_AdaptationRule)
	{
		case eADAPT_MIT_RULE:
		{
			return -adaptation_gain * model_error * sensitivity_derivative;

			break;
		}
	
		case eADAPT_SIGN_SIGN_RULE:
		{
			return -adaptation_gain * Sign(model_error) * Sign(sensitivity_derivative);

			break;
		}

		case eADAPT_SIGN_DATA_RULE:
		{
			return -adaptation_gain * model_error * Sign(sensitivity_derivative);

			break;
		}

		case eADAPT_SIGN_ERROR_RULE:
		{
			return -adaptation_gain * Sign(model_error) * sensitivity_derivative;

			break;
		}

		case eADAPT_NORMALIZED_MIT_RULE:
		{
			return (-adaptation_gain * model_error * sensitivity_derivative) /
				(m_Alpha[current_term] + sensitivity_derivative * sensitivity_derivative);

			break;
		}

		default:
		{
			// Unknown adaptation rule
			ASSERT(0);

			break;
		}
	};

	return 0.0f;
}

const float MaxSensitivityDerivative = 1000.0f;

float CModelReferenceAdaptiveController::GetSensitivityDerivative(ePIDCoefficient current_term, 
																  float model_error, float timestep)
{
	float model_error_derivative = (model_error - m_PreviousModelError) / timestep;
	float coefficient_derivative = m_PreviousCoefficientDerivative[current_term];

	if (fabs(coefficient_derivative) > 0.0000001f)
	{
		return Clamp(model_error_derivative / coefficient_derivative, -MaxSensitivityDerivative, MaxSensitivityDerivative);
	}
	else if (fabs(model_error_derivative) < 0.0001f)
	{
		// This is the division of zero by zero case, as discussed in Calculating the Sensitivity Derivative
		return 0.0f;
	}
	else if (model_error_derivative > 0.0f)
	{
		// This is one of the division by zero cases, as discussed in Calculating the Sensitivity Derivative
		return 1.0f;
	}
	else
	{
		// This is the other division by zero case, as discussed in Calculating the Sensitivity Derivative
		return -1.0f;
	}
}

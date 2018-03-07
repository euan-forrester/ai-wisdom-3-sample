// MainDlg.cpp : implementation file
//

#include "stdafx.h"

#include "Mmsystem.h"
#include "time.h"

#include "AdaptivePIDControllersApp.h"
#include "MainDlg.h"
#include ".\maindlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
// Tuning constants
//

// Initial values for the radio buttons

eMissileControlRadioButtonValue	MissileInitialControlType	= eRADIO_MISSILE_CONTROL_PID;
eTargetControlRadioButtonValue	TargetInitialControlType	= eRADIO_TARGET_CONTROL_AUTOMATIC;

// Initial values for the sliders
const float	MissileInitialSteeringPCoefficient		= 2.0f;
const float	MissileInitialSteeringICoefficient		= 0.5f;
const float	MissileInitialSteeringDCoefficient		= 2.9f;
													
const float	MissileInitialMaxAcceleration			= 1000.0f;	// World units / second^2
const float	MissileInitialMaxAngularAcceleration	= 180.0f;	// Degrees / second^2
const float MissileInitialPIDOutputScale			= 1.0f;
const float	TargetInitialMaxSpeed					= 1250.0f;	// World units / second
					
const float	MissileInitialRotationalDragFactor		= 0.005f;

// Setup parameters for the sliders
const float	PIDSliderRange							= 30.0f;
const float	PIDSliderIncrement						= 0.1f;
const float	PIDSliderLargeChange					= 1.0f;
const float	PIDSliderTickFrequency					= 5.0f;
													
const float	MissileAccelerationSliderMinValue		= 0.0f;
const float	MissileAccelerationSliderMaxValue		= 3000.0f;
const float	MissileAccelerationSliderIncrement		= 50.0f;
const float	MissileAccelerationSliderLargeChange	= 500.0f;
const float	MissileAccelerationSliderTickFrequency	= 500.0f;

const float	MissileRotationalDragFactorSliderMinValue		= 0.0f;
const float	MissileRotationalDragFactorSliderMaxValue		= 0.25f;
const float	MissileRotationalDragFactorSliderIncrement		= 0.005f;
const float	MissileRotationalDragFactorSliderLargeChange	= 0.05f;
const float	MissileRotationalDragFactorSliderTickFrequency	= 0.05f;

const float	MissileAngularAccelerationSliderMinValue		= 0.0f;
const float	MissileAngularAccelerationSliderMaxValue		= 3600.0f;
const float	MissileAngularAccelerationSliderIncrement		= 22.5f;
const float	MissileAngularAccelerationSliderLargeChange		= 90.0f;
const float	MissileAngularAccelerationSliderTickFrequency	= 90.0f;

const float	MissilePIDOutputScaleSliderMinValue				= 0.0f;
const float	MissilePIDOutputScaleSliderMaxValue				= 10.0f;
const float	MissilePIDOutputScaleSliderIncrement			= 0.1f;
const float	MissilePIDOutputScaleSliderLargeChange			= 0.5f;
const float	MissilePIDOutputScaleSliderTickFrequency		= 0.5f;

const float	TargetSpeedSliderMinValue				= 0.0f;
const float	TargetSpeedSliderMaxValue				= 3000.0f;
const float	TargetSpeedSliderIncrement				= 50.0f;
const float	TargetSpeedSliderLargeChange			= 200.0f;
const float	TargetSpeedSliderTickFrequency			= 200.0f;

// Timing
const UINT	WorldUpdateFrequencyMilliseconds		= 30;
const UINT	WorldTimerIDNumber						= 1;

const float	WorldMaxTimestep						= 0.25f; // Maximum timestep in seconds

// Adaptive controller tuning values

const eAdaptationRule	MissileSteeringAdaptationRule			= eADAPT_MIT_RULE;
const float 			MissileSteeringTimeslice				= 0.33f;
const float 			MissileSteeringPTermUpdateThreshold		= 1.0f;
const float 			MissileSteeringITermUpdateThreshold		= 1.0f;
const float 			MissileSteeringDTermUpdateThreshold		= 1.0f;
const float 			MissileSteeringPTermAdaptationGain		= 0.0005f;
const float 			MissileSteeringITermAdaptationGain		= 0.0005f;
const float 			MissileSteeringDTermAdaptationGain		= 0.0001f;
const float 			MissileSteeringPTermAlpha				= 0.0f;
const float 			MissileSteeringITermAlpha				= 0.0f;
const float 			MissileSteeringDTermAlpha				= 0.0f;

const float				MissileSteeringMaxPCoefficient			= 30.0f;
const float				MissileSteeringMinPCoefficient			= 1.0f;
const float				MissileSteeringMaxICoefficient			= 7.0f;
const float				MissileSteeringMinICoefficient			= 0.0f;
const float				MissileSteeringMaxDCoefficient			= 6.0f;
const float				MissileSteeringMinDCoefficient			= 0.0f;

void CSliderCtrlWithCEdit::Init(CEdit *edit, float min_value, float max_value, 
								float increment, float large_change, float tick_frequency, 
								int number_format)	
{ 
	m_pCEdit		= edit; 
	m_MinValue		= min_value; 
	m_MaxValue		= max_value;
	m_Increment		= increment; 
	m_LargeChange	= large_change;
	m_TickFrequency	= tick_frequency;
	m_NumberFormat	= number_format; 

	int min_integer				= (int)((m_MinValue			/ m_Increment) + 0.5f);
	int max_integer				= (int)((m_MaxValue			/ m_Increment) + 0.5f);
	int increment_integer		= (int)((1.0f				/ m_Increment) + 0.5f);
	int large_change_integer	= (int)((m_LargeChange		/ m_Increment) + 0.5f);
	int tick_frequency_integer	= (int)((m_TickFrequency	/ m_Increment) + 0.5f);

	SetRange(min_integer, max_integer);
	SetLineSize(increment_integer);
	SetPageSize(large_change_integer);
	SetTicFreq(tick_frequency_integer);
}

void CSliderCtrlWithCEdit::SetValue(float new_value)
{
	int new_value_integer = (int)((new_value / m_Increment) + 0.5f);
	
	SetPos(new_value_integer);

	UpdateTextBox();
}

float CSliderCtrlWithCEdit::GetValue()
{
	return (float)GetPos() * m_Increment;
}

void CSliderCtrlWithCEdit::UpdateTextBox()
{
	CString number_format;
	number_format.LoadString(m_NumberFormat);

	CString text;
	text.Format(number_format, GetValue());

	m_pCEdit->SetWindowText(text);
}

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMainDlg dialog



CMainDlg::CMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent)
	, m_RadioTargetControlMode(TargetInitialControlType)
	, m_RadioMissileControlMode(MissileInitialControlType)
	, m_PauseWorld(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_MISSILE_CONTROL_ADAPTIVE_PID, m_RadioMissileControlMode);
	DDX_Radio(pDX, IDC_RADIO_TARGET_CONTROL_AUTOMATIC, m_RadioTargetControlMode);
	DDX_Check(pDX, IDC_CHECK_PAUSE_WORLD, m_PauseWorld);
	DDX_Control(pDX, IDC_EDIT_MISSILE_STEERING_P, m_TextBoxMissileSteeringPControl);
	DDX_Control(pDX, IDC_EDIT_MISSILE_STEERING_I, m_TextBoxMissileSteeringIControl);
	DDX_Control(pDX, IDC_EDIT_MISSILE_STEERING_D, m_TextBoxMissileSteeringDControl);
	DDX_Control(pDX, IDC_EDIT_MISSILE_ACCELERATION, m_TextBoxMissileAccelerationControl);
	DDX_Control(pDX, IDC_EDIT_TARGET_SPEED, m_TextBoxTargetSpeedControl);
	DDX_Control(pDX, IDC_SLIDER_MISSILE_STEERING_P, m_SliderMissileSteeringP);
	DDX_Control(pDX, IDC_SLIDER_MISSILE_STEERING_I, m_SliderMissileSteeringI);
	DDX_Control(pDX, IDC_SLIDER_MISSILE_STEERING_D, m_SliderMissileSteeringD);
	DDX_Control(pDX, IDC_SLIDER_MISSILE_ACCELERATION, m_SliderMissileAcceleration);
	DDX_Control(pDX, IDC_SLIDER_MISSILE_ROTATIONAL_DRAG, m_SliderMissileRotationalDrag);
	DDX_Control(pDX, IDC_SLIDER_MISSILE_MAX_ANGULAR_ACCELERATION, m_SliderMissileAngularAcceleration);
	DDX_Control(pDX, IDC_SLIDER_TARGET_SPEED, m_SliderTargetSpeed);
	DDX_Control(pDX, IDC_SLIDER_MISSILE_PID_OUTPUT_SCALE, m_SliderMissilePIDOutputScale);
	DDX_Control(pDX, IDC_EDIT_MISSILE_ROTATIONAL_DRAG, m_TextBoxMissileRotationalDrag);
	DDX_Control(pDX, IDC_EDIT_MISSILE_MAX_ANGULAR_ACCELERATION, m_TextBoxMissileAngularAcceleration);
	DDX_Control(pDX, IDC_EDIT_MISSILE_PID_OUTPUT_SCALE, m_TextBoxMissilePIDOutputScale);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_RESET_SLIDERS, OnBnClickedButtonResetSliders)
	ON_BN_CLICKED(IDC_BUTTON_HELP_P_COEFFICIENT, OnBnClickedButtonHelpPCoefficient)
	ON_BN_CLICKED(IDC_BUTTON_HELP_I_COEFFICIENT, OnBnClickedButtonHelpICoefficient)
	ON_BN_CLICKED(IDC_BUTTON_HELP_D_COEFFICIENT, OnBnClickedButtonHelpDCoefficient)
END_MESSAGE_MAP()


// CMainDlg message handlers

BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Seed random number generator

	srand((unsigned)time(NULL));

	// Init our keystate
	for (int i = 0; i < NUM_KEYS; i++)
	{
		m_KeyState[i] = false;
	}

	// Setup our timer
	m_Timer = SetTimer(WorldTimerIDNumber, WorldUpdateFrequencyMilliseconds, NULL);
	
	if (!m_Timer)
	{
		MessageBox("Unable to setup a system timer");
	}

	m_PreviousTime = timeGetTime();

	// Hook up our OpenGL window to our picture control
	CStatic *pclStatic = (CStatic *)GetDlgItem(IDC_OPENGLWIN);

	m_pclGlView = new CGlView(pclStatic);
	m_pclGlView->OnCreate();

	ResizeGLScene();

	// Init our sliders

	m_SliderMissileSteeringP.Init(&m_TextBoxMissileSteeringPControl,				-PIDSliderRange,							PIDSliderRange,								PIDSliderIncrement,							PIDSliderLargeChange,							PIDSliderTickFrequency,							IDS_PID_TEXTBOX_NUMBER_FORMAT);
	m_SliderMissileSteeringI.Init(&m_TextBoxMissileSteeringIControl,				-PIDSliderRange,							PIDSliderRange,								PIDSliderIncrement,							PIDSliderLargeChange,							PIDSliderTickFrequency,							IDS_PID_TEXTBOX_NUMBER_FORMAT);
	m_SliderMissileSteeringD.Init(&m_TextBoxMissileSteeringDControl,				-PIDSliderRange,							PIDSliderRange,								PIDSliderIncrement,							PIDSliderLargeChange,							PIDSliderTickFrequency,							IDS_PID_TEXTBOX_NUMBER_FORMAT);

	m_SliderMissileAcceleration.Init(&m_TextBoxMissileAccelerationControl,			MissileAccelerationSliderMinValue,			MissileAccelerationSliderMaxValue,			MissileAccelerationSliderIncrement,			MissileAccelerationSliderLargeChange,			MissileAccelerationSliderTickFrequency,			IDS_ACCELERATION_SPEED_TEXTBOX_NUMBER_FORMAT);
	m_SliderMissileRotationalDrag.Init(&m_TextBoxMissileRotationalDrag,				MissileRotationalDragFactorSliderMinValue,	MissileRotationalDragFactorSliderMaxValue,	MissileRotationalDragFactorSliderIncrement,	MissileRotationalDragFactorSliderLargeChange,	MissileRotationalDragFactorSliderTickFrequency,	IDS_ROTATIONAL_DRAG_NUMBER_FORMAT);
	m_SliderMissileAngularAcceleration.Init(&m_TextBoxMissileAngularAcceleration,	MissileAngularAccelerationSliderMinValue,	MissileAngularAccelerationSliderMaxValue,	MissileAngularAccelerationSliderIncrement,	MissileAngularAccelerationSliderLargeChange,	MissileAngularAccelerationSliderTickFrequency,	IDS_ACCELERATION_SPEED_TEXTBOX_NUMBER_FORMAT);
	m_SliderMissilePIDOutputScale.Init(&m_TextBoxMissilePIDOutputScale,				MissilePIDOutputScaleSliderMinValue,		MissilePIDOutputScaleSliderMaxValue,		MissilePIDOutputScaleSliderIncrement,		MissilePIDOutputScaleSliderLargeChange,			MissilePIDOutputScaleSliderTickFrequency,		IDS_ROTATIONAL_DRAG_NUMBER_FORMAT);

	m_SliderTargetSpeed.Init(&m_TextBoxTargetSpeedControl,							TargetSpeedSliderMinValue,					TargetSpeedSliderMaxValue,					TargetSpeedSliderIncrement,					TargetSpeedSliderLargeChange,					TargetSpeedSliderTickFrequency,					IDS_ACCELERATION_SPEED_TEXTBOX_NUMBER_FORMAT);

	// Set the sliders to their initial values
	OnBnClickedButtonResetSliders();


	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMainDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	m_World.Draw(m_pclGlView);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMainDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMainDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	ResizeGLScene();

	// Force refresh
	InvalidateRect(NULL);
}

BOOL CMainDlg::DestroyWindow() 
{
	delete m_pclGlView;
	
	KillTimer(m_Timer);

	return CDialog::DestroyWindow();
}

void CMainDlg::OnBnClickedButtonResetSliders()
{
	m_World.GetMissile()->ResetSteering();

	m_World.GetMissile()->SetSteeringPIDCoefficients(MissileInitialSteeringPCoefficient, MissileInitialSteeringICoefficient, MissileInitialSteeringDCoefficient);

	m_SliderMissileSteeringP.SetValue(MissileInitialSteeringPCoefficient);
	m_SliderMissileSteeringI.SetValue(MissileInitialSteeringICoefficient);
	m_SliderMissileSteeringD.SetValue(MissileInitialSteeringDCoefficient);
	m_SliderMissileAcceleration.SetValue(MissileInitialMaxAcceleration);
	m_SliderMissileAngularAcceleration.SetValue(MissileInitialMaxAngularAcceleration);
	m_SliderTargetSpeed.SetValue(TargetInitialMaxSpeed);
	m_SliderMissileRotationalDrag.SetValue(MissileInitialRotationalDragFactor);
	m_SliderMissilePIDOutputScale.SetValue(MissileInitialPIDOutputScale);
}

void CMainDlg::OnBnClickedButtonHelpPCoefficient()
{
	DisplayHelpMessageBox(IDS_HELP_P_COEFFICIENT, IDS_HELP_P_COEFFICIENT_2);
}

void CMainDlg::OnBnClickedButtonHelpICoefficient()
{
	DisplayHelpMessageBox(IDS_HELP_I_COEFFICIENT, IDS_HELP_I_COEFFICIENT_2);
}

void CMainDlg::OnBnClickedButtonHelpDCoefficient()
{
	DisplayHelpMessageBox(IDS_HELP_D_COEFFICIENT, IDS_HELP_D_COEFFICIENT_2);
}

void CMainDlg::DisplayHelpMessageBox(UINT help_message_id_part_1, UINT help_message_id_part_2)
{
	CString caption;
	CString help_text_part_1;
	CString help_text_part_2;
	CString help_text_part_3;
	CString help_text_part_4;

	caption.LoadString(IDS_HELP_CAPTION);
	help_text_part_1.LoadString(help_message_id_part_1);
	help_text_part_2.LoadString(IDS_ERROR_EXPLANATION);
	help_text_part_3.LoadString(help_message_id_part_2);
	help_text_part_4.LoadString(IDS_EQUATION);

	MessageBox(help_text_part_1 + help_text_part_2 + help_text_part_3 + help_text_part_4, caption, MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
}

LONG CMainDlg::WindowProc(UINT msg, UINT wParam, LONG lParam) 
{ 
	switch (msg) 
	{ 
		case WM_HSCROLL: 
		case WM_VSCROLL: 
		{
			CSliderCtrlWithCEdit* slider = (CSliderCtrlWithCEdit*)this->FromHandle((HWND)lParam); 
			
			slider->UpdateTextBox();
			
			return TRUE; 
		}
    };
	
	return CDialog::WindowProc(msg, wParam, lParam); 
} 

void CMainDlg::ResizeGLScene()
{
    RECT rect;
	GetClientRect(&rect);

    int iWidth = -(rect.right - rect.left);
    int iHeight = rect.top - rect.bottom;
	
	m_pclGlView->ReSizeGLScene(iWidth, iHeight, &m_World);
}

// Taken from http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winui/WinUI/WindowsUserInterface/UserInput/VirtualKeyCodes.asp
#define VK_A	0x41
#define VK_D	0x44
#define VK_W	0x57
#define VK_S	0x53

void CMainDlg::ReadKeyboardState()
{
	// Only read the keyboard state if one of the windows of this application
	// has the focus
	
	if (GetFocus())
	{	
		m_KeyState[eKEY_MISSILE_THRUST]		= (GetAsyncKeyState(VK_NUMPAD8)	!= 0);
		m_KeyState[eKEY_MISSILE_TURN_LEFT]	= (GetAsyncKeyState(VK_NUMPAD4)	!= 0);
		m_KeyState[eKEY_MISSILE_TURN_RIGHT]	= (GetAsyncKeyState(VK_NUMPAD6)	!= 0);

		m_KeyState[eKEY_TARGET_MOVE_LEFT]	= (GetAsyncKeyState(VK_A)		!= 0);
		m_KeyState[eKEY_TARGET_MOVE_RIGHT]	= (GetAsyncKeyState(VK_D)		!= 0);
		m_KeyState[eKEY_TARGET_MOVE_UP]		= (GetAsyncKeyState(VK_W)		!= 0);
		m_KeyState[eKEY_TARGET_MOVE_DOWN]	= (GetAsyncKeyState(VK_S)		!= 0);
	}
	else
	{
		for (int i = 0; i < NUM_KEYS; i++)
		{
			m_KeyState[i] = false;
		}
	}
}

void CMainDlg::HandleUIControls()
{
	//
	// Set our missile and target control modes based on what the user 
	// has selected
	//

	switch (m_RadioMissileControlMode)
	{
		case eRADIO_MISSILE_CONTROL_ADAPTIVE_PID:
		{
			m_World.GetMissile()->SetControlMode(eMISSILE_CONTROL_ADAPTIVE_PID);

			break;
		}

		case eRADIO_MISSILE_CONTROL_PID:
		{
			m_World.GetMissile()->SetControlMode(eMISSILE_CONTROL_PID);

			break;
		}

		case eRADIO_MISSILE_CONTROL_KEYBOARD:
		{
			m_World.GetMissile()->SetControlMode(eMISSILE_CONTROL_KEYBOARD);

			break;
		}
		
		default:
		{
			TRACE("Unknown value for m_RadioMissileControlMode: %d\n", m_RadioMissileControlMode);

			break;
		}
	};
	
	switch (m_RadioTargetControlMode)
	{
		case eRADIO_TARGET_CONTROL_AUTOMATIC:
		{
			m_World.GetTarget()->SetControlMode(eTARGET_CONTROL_AUTOMATIC);

			break;
		}

		case eRADIO_TARGET_CONTROL_KEYBOARD:
		{
			m_World.GetTarget()->SetControlMode(eTARGET_CONTROL_KEYBOARD);

			break;
		}

		default:
		{
			TRACE("Unknown value for m_RadioTargetControlMode: %d\n", m_RadioTargetControlMode);

			break;
		}		
	}

	//
	// If the missile is under adaptive control, update the sliders to be the
	// calculated values, otherwise send the P, I, and D values the user has 
	// selected over to the PID controller
	//

	if (m_RadioMissileControlMode == eRADIO_MISSILE_CONTROL_ADAPTIVE_PID)
	{
		float p_value = m_World.GetMissile()->GetSteeringCoefficient(eP_COEFFICIENT);
		float i_value = m_World.GetMissile()->GetSteeringCoefficient(eI_COEFFICIENT);
		float d_value = m_World.GetMissile()->GetSteeringCoefficient(eD_COEFFICIENT);

		m_SliderMissileSteeringP.SetValue(p_value);
		m_SliderMissileSteeringI.SetValue(i_value);
		m_SliderMissileSteeringD.SetValue(d_value);
	}
	else
	{
		float p_value = m_SliderMissileSteeringP.GetValue();
		float i_value = m_SliderMissileSteeringI.GetValue();
		float d_value = m_SliderMissileSteeringD.GetValue();

		m_World.GetMissile()->SetSteeringPIDCoefficients(p_value, i_value, d_value);
	}

	//
	// Set the missile's steering properties
	//
	
	m_World.GetMissile()->SetSteeringAdaptationRule(MissileSteeringAdaptationRule);
	m_World.GetMissile()->SetSteeringTimeslice(MissileSteeringTimeslice);
	m_World.GetMissile()->SetSteeringCoefficientClamp(eP_COEFFICIENT, MissileSteeringMinPCoefficient, MissileSteeringMaxPCoefficient);
	m_World.GetMissile()->SetSteeringCoefficientClamp(eI_COEFFICIENT, MissileSteeringMinICoefficient, MissileSteeringMaxICoefficient);
	m_World.GetMissile()->SetSteeringCoefficientClamp(eD_COEFFICIENT, MissileSteeringMinDCoefficient, MissileSteeringMaxDCoefficient);
	m_World.GetMissile()->SetSteeringUpdateThreshold(eP_COEFFICIENT, MissileSteeringPTermUpdateThreshold);
	m_World.GetMissile()->SetSteeringUpdateThreshold(eI_COEFFICIENT, MissileSteeringITermUpdateThreshold);
	m_World.GetMissile()->SetSteeringUpdateThreshold(eD_COEFFICIENT, MissileSteeringDTermUpdateThreshold);
	m_World.GetMissile()->SetSteeringAdaptationGain(eP_COEFFICIENT, MissileSteeringPTermAdaptationGain);
	m_World.GetMissile()->SetSteeringAdaptationGain(eI_COEFFICIENT, MissileSteeringITermAdaptationGain);
	m_World.GetMissile()->SetSteeringAdaptationGain(eD_COEFFICIENT, MissileSteeringDTermAdaptationGain);
	m_World.GetMissile()->SetSteeringAlpha(eP_COEFFICIENT, MissileSteeringPTermAlpha);
	m_World.GetMissile()->SetSteeringAlpha(eI_COEFFICIENT, MissileSteeringITermAlpha);
	m_World.GetMissile()->SetSteeringAlpha(eD_COEFFICIENT, MissileSteeringDTermAlpha);

	//
	// Set the missile's physics properties
	//

	float missile_acceleration				= m_SliderMissileAcceleration.GetValue();
	float missile_angular_acceleration		= m_SliderMissileAngularAcceleration.GetValue();
	float missile_rotational_drag_factor	= m_SliderMissileRotationalDrag.GetValue();
	float missile_pid_output_scale			= m_SliderMissilePIDOutputScale.GetValue();

	m_World.GetMissile()->SetMaxAcceleration(missile_acceleration);
	m_World.GetMissile()->SetMaxAngularAcceleration(missile_angular_acceleration);
	m_World.GetMissile()->SetRotationalDragFactor(missile_rotational_drag_factor);
	m_World.GetMissile()->SetPIDOutputScale(missile_pid_output_scale);

	//
	// Set the target's properties
	//

	float target_speed = m_SliderTargetSpeed.GetValue();
	
	m_World.GetTarget()->SetMaxSpeed(target_speed);
}

void CMainDlg::OnTimer(UINT nIDEvent) 
{
	//
	// Begin our timestep
	//

	m_World.BeginTimestep();

	//
	// Handle keyboard input
	//

	ReadKeyboardState();

	for (int i = 0; i < NUM_KEYS; i++)
	{
		m_World.HandleKeyboardState((eKey)i, m_KeyState[i]);
	}

	//
	// Handle data entered into the controls on the dialog box
	//

	UpdateData(TRUE);

	HandleUIControls();

	//
	// Figure out how long it's been since we were called,
	// and update the world by that amount of time
	//

	DWORD current_time = timeGetTime();

	if (!m_PauseWorld)
	{
		float timestep	= (float)(current_time - m_PreviousTime) / 1000.0f;
		timestep		= min(timestep, WorldMaxTimestep);

		m_World.DoTimestep(timestep);
	}

	m_PreviousTime = current_time;

	//
	// End our timestep
	//

	m_World.EndTimestep();

	//
	// Redraw the screen
	//

	PostMessage(WM_PAINT);
	
	//
	// Call the base function
	//

	CDialog::OnTimer(nIDEvent);
}
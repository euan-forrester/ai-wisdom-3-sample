// MainDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "GlView.h"
#include "CWorld.h"

// The order here must match the order of the corresponding radio buttons 
// on the dialog box
enum eMissileControlRadioButtonValue
{
    eRADIO_MISSILE_CONTROL_ADAPTIVE_PID = 0,
    eRADIO_MISSILE_CONTROL_PID,
    eRADIO_MISSILE_CONTROL_KEYBOARD,

    NUM_RADIO_MISSILE_CONTROL_MODES,
};

// The order here must match the order of the corresponding radio buttons 
// on the dialog box
enum eTargetControlRadioButtonValue
{
    eRADIO_TARGET_CONTROL_AUTOMATIC = 0,
    eRADIO_TARGET_CONTROL_KEYBOARD,

    NUM_TARGET_CONTROL_MODES,
};

class CSliderCtrlWithCEdit : public CSliderCtrl
{
public:
    void	Init(CEdit *edit, float min_value, float max_value, float increment, float large_change, float tick_frequency, int number_format);

    void	SetValue(float new_value);
    float	GetValue();

    void	UpdateTextBox();

private:
    CEdit*	m_pCEdit;
    float	m_MinValue;
    float	m_MaxValue;
    float	m_Increment;
    float	m_LargeChange;
    float	m_TickFrequency;
    int		m_NumberFormat;
};

// CMainDlg dialog
class CMainDlg : public CDialog
{
// Construction
public:
    CMainDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
    enum { IDD = IDD_INTELLIGENTSTEERINGUSINGADAPTIVEPIDCONTROLLERS_DIALOG };

    protected:
    virtual BOOL			DestroyWindow();
    virtual void			DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
    HICON					m_hIcon;

    // Generated message map functions
    virtual BOOL			OnInitDialog();
    afx_msg void			OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void			OnPaint();
    afx_msg void			OnSize(UINT nType, int cx, int cy);
    afx_msg void			OnTimer(UINT nIDEvent);
    afx_msg HCURSOR			OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    virtual LONG			WindowProc(UINT msg, UINT wParam, LONG lParam);

public:
    afx_msg void			OnBnClickedButtonResetSliders();

private:
    void					DisplayHelpMessageBox(UINT help_message_id_part_1, UINT help_message_id_part_2);

    void					ReadKeyboardState();
    void					HandleUIControls();

    void					ResizeGLScene();

    CGlView*				m_pclGlView;
    CWorld					m_World;

    UINT_PTR				m_Timer;
    DWORD					m_PreviousTime;

    bool					m_KeyState[NUM_KEYS];

public:
    afx_msg void			OnBnClickedButtonHelpPCoefficient();
    afx_msg void			OnBnClickedButtonHelpICoefficient();
    afx_msg void			OnBnClickedButtonHelpDCoefficient();

private:
    int						m_RadioTargetControlMode;
    int						m_RadioMissileControlMode;
    int						m_PauseWorld;
    CEdit					m_TextBoxMissileSteeringPControl;
    CEdit					m_TextBoxMissileSteeringIControl;
    CEdit					m_TextBoxMissileSteeringDControl;
    CEdit					m_TextBoxMissileAccelerationControl;
    CEdit					m_TextBoxTargetSpeedControl;
    CEdit 					m_TextBoxMissileRotationalDrag;
    CEdit 					m_TextBoxMissileAngularAcceleration;
    CEdit					m_TextBoxMissilePIDOutputScale;
    CSliderCtrlWithCEdit	m_SliderMissileSteeringP;
    CSliderCtrlWithCEdit	m_SliderMissileSteeringI;
    CSliderCtrlWithCEdit	m_SliderMissileSteeringD;
    CSliderCtrlWithCEdit	m_SliderMissileAcceleration;
    CSliderCtrlWithCEdit	m_SliderMissileRotationalDrag;
    CSliderCtrlWithCEdit	m_SliderMissileAngularAcceleration;
    CSliderCtrlWithCEdit	m_SliderMissilePIDOutputScale;
    CSliderCtrlWithCEdit	m_SliderTargetSpeed;
public:
    afx_msg void OnBnClickedRadioMissileControlAdaptivePid();
};

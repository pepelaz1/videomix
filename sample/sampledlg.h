
// sampledlg.h : header file
//

#pragma once
#include "player.h"
#include "afxwin.h"
#include "afxcmn.h"

// CSampleDlg dialog
class CSampleDlg : public CDialogEx
{
private:
	CPlayer m_player;
	CPlayerWindow m_playerWnd;

	void MoveControls();
	void InitPlayer();
public:
	void Finalize();
	// Construction
public:
	CSampleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nTimerID);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC,CWnd* pWnd, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPlayBtn();
	afx_msg void OnBnClickedPauseBtn();
	afx_msg void OnBnClickedStopBtn();
	afx_msg void OnBnClickedStepBtn();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CButton m_btnPlay;
	CButton m_btnPause;
	CButton m_btnStop;
	CButton m_btnStep;
	
	afx_msg void OnMove(int x, int y);
	CStatic m_stcFile1;
	CStatic m_stcFile2;
	CEdit m_edtFile1;
	CEdit m_edtFile2;
	CButton m_btnBrowse1;
	CButton m_btnBrowse2;
	afx_msg void OnBnClickedBrowseBtn();
	afx_msg void OnClose();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	CSliderCtrl m_slider;
	afx_msg void OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedBrowseBtn2();
	CStatic m_stcBBox;
	CStatic m_stcBBoxX;
	CEdit m_edtBBoxX;
	CStatic m_stcBBoxY;
	CEdit m_edtBBoxY;
	CStatic m_stcBBoxWidth;
	CEdit m_edtBBoxWidth;
	CStatic m_stcBBoxHeight;
	CEdit m_edtBBoxHeight;
	CButton m_btnBBoxSet;
	afx_msg void OnBnClickedStepBtn2();
};

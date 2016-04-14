#pragma once


// CPlayerWindow
class CPlayer;
class CPlayerWindow : public CWnd
{
	DECLARE_DYNAMIC(CPlayerWindow)

public:
	CPlayerWindow();
	virtual ~CPlayerWindow();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint( );
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
	CPlayer *m_player;
	CPoint m_ptStart;
	CPoint m_ptDragStart;
	CPoint m_ptEnd;
	BOOL m_bIsDragging;

	CPoint &MapPoint(CPoint &pt, RECT &r);
public:
	void SetPlayer(CPlayer *player);	
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};



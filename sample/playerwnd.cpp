// playerwnd.cpp : implementation file
//

#include "stdafx.h"
#include "sample.h"
#include "playerwnd.h"


// CPlayerWindow

IMPLEMENT_DYNAMIC(CPlayerWindow, CWnd)

CPlayerWindow::CPlayerWindow() 
: m_ptStart(), m_ptEnd(), m_player(NULL), m_bIsDragging(FALSE)
{

}

CPlayerWindow::~CPlayerWindow()
{
}


BEGIN_MESSAGE_MAP(CPlayerWindow, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()


BOOL CPlayerWindow::OnEraseBkgnd(CDC* pDC)
{
	return 1;
}

void CPlayerWindow::OnPaint()
{
	CPaintDC dc(this);
	CBrush b(RGB(0,0,0));
	RECT r;
	GetClientRect(&r);
	dc.FillRect(&r,&b);

	RECT r1;
	if ( m_ptStart.x < m_ptEnd.x)
	{
		r1.left = m_ptStart.x;
		r1.right = m_ptEnd.x;
	}
	else
	{
		r1.left = m_ptEnd.x;
		r1.right = m_ptStart.x;
	}
	
	if ( m_ptStart.y < m_ptEnd.y)
	{
		r1.top = m_ptStart.y;
		r1.bottom = m_ptEnd.y;
	}
	else
	{
		r1.top = m_ptEnd.y;
		r1.bottom = m_ptStart.y;
	}
	
	//dc.DrawFocusRect(&r1);
}


void CPlayerWindow::OnLButtonDown(UINT nFlags, CPoint point)
{
	RECT r;
	m_player->GetBoundingBox(&r);
	CPoint pt = MapPoint(point, r);

	CRect rr = r;
	BOOL bb = rr.PtInRect(pt);
	if (bb)
	{
		m_bIsDragging = true;
		m_ptDragStart = pt;
		SetCapture();
	}
	else
	{
		m_ptStart = point;
		SetCapture();
	}
	CWnd::OnLButtonDown(nFlags, point);

}


void CPlayerWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( nFlags & MK_LBUTTON)
	{
		if (!m_bIsDragging)
		{
			m_ptEnd = point;
			if (m_player)
			{
				int vw = m_player->GetVideoWidth();
				int vh = m_player->GetVideoHeight();

				if (vw == -1 || vh == -1)
					return;

				RECT r;
				if ( m_ptStart.x < m_ptEnd.x)
				{
					r.left = m_ptStart.x;
					r.right = m_ptEnd.x;
				}
				else
				{
					r.left = m_ptEnd.x;
					r.right = m_ptStart.x;
				}

				if ( m_ptStart.y < m_ptEnd.y)
				{
					r.top = m_ptStart.y;
					r.bottom = m_ptEnd.y;
				}
				else
				{
					r.top = m_ptEnd.y;
					r.bottom = m_ptStart.y;
				}

				RECT clr;
				GetClientRect(&clr);

				int w = r.right-r.left;
				int h = r.bottom-r.top;						

				double a = (double)clr.right/clr.bottom;
				double b = (double)vw/vh;
				int actw = 0, acth = 0;
				int dw = 0, dh = 0;
				double kw = 0, kh = 0;

				int leftb = 0, topb = 0, wb = 0, hb = 0;
				if ( a > b ) // Black areas at sides
				{
					actw = clr.bottom * b;
					acth = clr.bottom;
				}
				else // Black areas at top/bottom
				{
					actw = clr.right;
					acth = clr.right / b;
				}

				dw = (clr.right - actw)/2 ;
				dh = (clr.bottom - acth)/2;

				kw = (double)clr.right / actw;
				kh = (double)clr.bottom / acth;

				leftb = (((double)vw / (clr.right-clr.left)) * (r.left-dw))*kw;
				topb = ((double)vh / (clr.bottom-clr.top)) * ((clr.bottom - r.bottom - dh) )*kh;
				wb = ((double)vw / (clr.right-clr.left)) * w * kw; 
				hb = ((double)vh / (clr.bottom-clr.top)) * h * kh;


				char str[MAX_PATH];
				sprintf_s(str, MAX_PATH,"left = %d, top = %d, width = %d, height = %d\n", leftb, topb, wb, hb);
				OutputDebugStringA(str);

				if (leftb >= 0 && topb >= 0 && (leftb+wb) < vw && (topb+hb) < vh-1 )
					m_player->SetBoundingBox(leftb, topb, wb, hb);
			}
		}
		else
		{
			if (m_player)
			{
				int vw = m_player->GetVideoWidth();
				int vh = m_player->GetVideoHeight();

				RECT r;
				m_player->GetBoundingBox(&r);
				CPoint pt = MapPoint(point, r);

				int dx = pt.x - m_ptDragStart.x;
				int dy = pt.y - m_ptDragStart.y;

				r.left += dx;
				r.top += dy;
				r.right += dx;
				r.bottom += dy;

				if (r.left >= 0 && r.top >= 0 && r.right < vw && r.bottom < vh-1 )
				{
					m_player->SetBoundingBox(r.left, r.top, r.right-r.left, r.bottom-r.top);	
					m_ptDragStart = pt;
				}
			}
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CPlayerWindow::SetPlayer(CPlayer *player)
{
	m_player = player;
}

void CPlayerWindow::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bIsDragging = FALSE;
	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

CPoint &CPlayerWindow::MapPoint(CPoint &pt, RECT &r)
{
	CPoint result = pt;
	int vw = m_player->GetVideoWidth();
	int vh = m_player->GetVideoHeight();

	if (vw != -1 && vh != -1)
	{
		RECT clr;
		GetClientRect(&clr);

		int w = r.right-r.left;
		int h = r.bottom-r.top;		

		double a = (double)clr.right/clr.bottom;
		double b = (double)vw/vh;
		int actw = 0, acth = 0;
		int dw = 0, dh = 0;
		double kw = 0, kh = 0;

		int leftb = 0, topb = 0, wb = 0, hb = 0;
		if ( a > b ) // Black areas at sides
		{
			actw = clr.bottom * b;
			acth = clr.bottom;
		}
		else // Black areas at top/bottom
		{
			actw = clr.right;
			acth = clr.right / b;
		}

		dw = (clr.right - actw)/2 ;
		dh = (clr.bottom - acth)/2;

		kw = (double)clr.right / actw;
		kh = (double)clr.bottom / acth;

		result.x = (((double)vw / (clr.right-clr.left)) * (result.x-dw))*kw;
		result.y = ((double)vh / (clr.bottom-clr.top)) * ((clr.bottom - result.y - dh) )*kh;
	}
	return result;
}

void CPlayerWindow::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
}

void CPlayerWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
}
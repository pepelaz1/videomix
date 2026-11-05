// playerwnd.cpp : implementation file
//

#include "stdafx.h"
#include "sample.h"
#include "playerwnd.h"


// CPlayerWindow

IMPLEMENT_DYNAMIC(CPlayerWindow, CWnd)

CPlayerWindow::CPlayerWindow() 
: m_ptStart(), m_ptEnd(), m_player(NULL), m_bIsDragging(FALSE), m_bboxChangedDuringDrag(FALSE)
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
	m_bboxChangedDuringDrag = FALSE;
	RECT r = {};
	CPoint videoPoint;
	if (m_player && m_player->GetBoundingBox(&r) && MapPoint(point, videoPoint))
	{
		m_bIsDragging = CRect(r).PtInRect(videoPoint);
		m_ptDragStart = videoPoint;
		m_ptStart = videoPoint;
		SetCapture();
	}
	CWnd::OnLButtonDown(nFlags, point);
}


void CPlayerWindow::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetCapture() == this && (nFlags & MK_LBUTTON) && m_player)
	{
		CPoint videoPoint;
		if (!MapPoint(point, videoPoint))
			return;
		const int vw = m_player->GetVideoWidth();
		const int vh = m_player->GetVideoHeight();
		if (vw <= 0 || vh <= 0)
			return;

		if (m_bIsDragging)
		{
			RECT r = {};
			if (m_player->GetBoundingBox(&r))
			{
				const int dx = videoPoint.x - m_ptDragStart.x;
				const int dy = videoPoint.y - m_ptDragStart.y;
				if (r.left + dx >= 0 && r.top + dy >= 0 &&
					r.right + dx < vw && r.bottom + dy < vh - 1)
				{
					m_player->SetBoundingBox(r.left + dx, r.top + dy,
						r.right - r.left, r.bottom - r.top);
					m_ptDragStart = videoPoint;
					m_bboxChangedDuringDrag = TRUE;
				}
			}
		}
		else
		{
			const int left = min(m_ptStart.x, videoPoint.x);
			const int bottom = min(m_ptStart.y, videoPoint.y);
			const int width = abs(videoPoint.x - m_ptStart.x);
			const int height = abs(videoPoint.y - m_ptStart.y);
			if (width >= 2 && height >= 1 && left >= 0 && bottom >= 0 &&
				left + width < vw && bottom + height < vh - 1)
			{
				m_player->SetBoundingBox(left, bottom, width, height);
				m_bboxChangedDuringDrag = TRUE;
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
	if (GetCapture() == this)
		ReleaseCapture();
	if (m_bboxChangedDuringDrag && GetParent())
		GetParent()->PostMessage(WM_APP + 1);
	m_bboxChangedDuringDrag = FALSE;
	CWnd::OnLButtonUp(nFlags, point);
}

bool CPlayerWindow::MapPoint(const CPoint &point, CPoint &videoPoint)
{
	if (!m_player)
		return false;
	const int vw = m_player->GetVideoWidth();
	const int vh = m_player->GetVideoHeight();
	CRect client;
	GetClientRect(&client);
	if (vw <= 0 || vh <= 0 || client.Width() <= 0 || client.Height() <= 0)
		return false;

	const double scale = min(client.Width() / double(vw), client.Height() / double(vh));
	if (scale <= 0.0)
		return false;
	const double displayWidth = vw * scale;
	const double displayHeight = vh * scale;
	const double left = (client.Width() - displayWidth) / 2.0;
	const double top = (client.Height() - displayHeight) / 2.0;
	if (point.x < left || point.x >= left + displayWidth ||
		point.y < top || point.y >= top + displayHeight)
		return false;

	videoPoint.x = min(vw - 1, int((point.x - left) / scale));
	videoPoint.y = min(vh - 1, int((top + displayHeight - 1 - point.y) / scale));
	return true;
}

void CPlayerWindow::OnRButtonDown(UINT nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
}

void CPlayerWindow::OnRButtonUp(UINT nFlags, CPoint point)
{
}

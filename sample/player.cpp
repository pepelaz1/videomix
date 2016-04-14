#include "stdafx.h"

#pragma warning( disable : 4995)

#include "../mixerfilter/IVideoMixingFilter.h"
#include "../srcfilter/IVideoSourceFilter.h"

CPlayer::CPlayer() :
	m_owner(NULL), m_owner_p(NULL), m_vmix(NULL), m_vdc(NULL), m_vmix_p(NULL), m_vdc_p(NULL)
{
	ZeroMemory(m_file1, MAX_PATH);
	ZeroMemory(m_file2, MAX_PATH);
}

CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Init(HWND owner, HWND owner_p)
{
	HRESULT hr = S_OK;
	Reset();

	m_owner = owner;
	m_owner_p = owner_p;
		
	hr = m_gb.CoCreateInstance(CLSID_FilterGraph);
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_LAV_Splitter_Source, L"File Source 1", m_src1, m_gb); 
	if (FAILED(hr))
		return hr;

	CComPtr<IFileSourceFilter>src1;
	hr = m_src1.QueryInterface(&src1);
	if (FAILED(hr))
		return hr;

	hr = src1->Load(m_file1, NULL);
	if (FAILED(hr))
		return hr;

	src1.Release();
		
	hr = AddFilter(CLSID_LAV_Splitter_Source, L"File Source 2", m_src2, m_gb); 
	if (FAILED(hr))
		return hr;
	
	CComPtr<IFileSourceFilter>src2;
	hr = m_src2.QueryInterface(&src2);
	if (FAILED(hr))
		return hr;

	hr = src2->Load(m_file2, NULL);
	if (FAILED(hr))
		return hr;

	src2.Release();

	hr = AddFilter(CLSID_LAV_Video_Decoder, L"LAV 1", m_lav1, m_gb); 
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_LAV_Video_Decoder, L"LAV 2", m_lav2, m_gb); 
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_VideoMixingFilter, L"Video Mixing Filter", m_vmix, m_gb); 
	if (FAILED(hr))
		return hr;

	hr = Connect(m_src1, m_lav1, m_gb);
	if (FAILED(hr))
		return hr;

	hr = Connect(m_src2, m_lav2, m_gb);
	if (FAILED(hr))
		return hr;
	
	//hr = Connect(m_ffdshow1, m_vmix);
	//hr = Connect(m_lav1, m_vmix);
	hr = m_gb->ConnectDirect(GetPin(m_lav1, L"Output"), GetPin(m_vmix, L"Input 0"),NULL);
	if (FAILED(hr))
		return hr;

	///////////////////////////////
	//this is the problem connection! -- mf fixed by changing this to ConnectDirect
	//////////////////////////////
	//hr = Connect(m_ffdshow2, m_vmix);
	//hr = Connect(m_lav2, m_vmix);
	hr = m_gb->ConnectDirect(GetPin(m_lav2, L"Output"), GetPin(m_vmix, L"Input 1"),NULL);
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_EnhancedVideoRenderer, L"Enchanced Video Renderer", m_evr, m_gb); 
	if (FAILED(hr))
		return hr;

	hr = Connect(m_vmix, m_evr, m_gb);
	if (FAILED(hr))
		return hr;

	AddToRot(m_gb,&m_rot);

	hr = m_gb.QueryInterface(&m_mc);
	if (FAILED(hr))
		return hr;

	hr = m_gb.QueryInterface(&m_ms);
	if (FAILED(hr))
		return hr;

	hr = m_gb.QueryInterface(&m_vfs);
	if (FAILED(hr))
		return hr;

	hr = m_evr.QueryInterface(&m_gs);
	if (FAILED(hr))
		return hr;

	hr = m_gs->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl, (void **)&m_vdc);
	if (FAILED(hr))
		return hr;

	hr = m_gs->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoPresenter, (void **)&m_vp);
	if (FAILED(hr))
		return hr;

	TuneVideoWindow();

	// Second graph
	hr = m_gb_p.CoCreateInstance(CLSID_FilterGraph);
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_VideoSourceFilter, L"Image Source 1", m_src1_p, m_gb_p); 
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_VideoSourceFilter, L"Image Source 2", m_src2_p, m_gb_p); 
	if (FAILED(hr))
		return hr;

	SetVideoParams_p(m_src1_p, m_vdc);
	SetVideoParams_p(m_src2_p, m_vdc);

	hr = AddFilter(CLSID_VideoMixingFilter, L"Video Mixing Filter (Pause)", m_vmix_p, m_gb_p); 
	if (FAILED(hr))
		return hr;

	//hr = Connect(m_src1_p, m_vmix_p, m_gb_p);
	//if (FAILED(hr))
	//	return hr;

	hr = m_gb_p->ConnectDirect(GetPin(m_src1_p, L"Out"), GetPin(m_vmix_p, L"Input 0"),NULL);
	if (FAILED(hr))
		return hr;

	//hr = Connect(m_src2_p, m_vmix_p, m_gb_p);
	//if (FAILED(hr))
	//	return hr;

	hr = m_gb_p->ConnectDirect(GetPin(m_src2_p, L"Out"), GetPin(m_vmix_p, L"Input 1"),NULL);
	if (FAILED(hr))
		return hr;

	hr = AddFilter(CLSID_EnhancedVideoRenderer, L"Enchanced Video Renderer (Pause)", m_evr_p, m_gb_p); 
	if (FAILED(hr))
		return hr;

	hr = Connect(m_vmix_p, m_evr_p, m_gb_p);
	if (FAILED(hr))
		return hr;

	hr = m_gb_p.QueryInterface(&m_mc_p);
	if (FAILED(hr))
		return hr;

	hr = m_evr_p.QueryInterface(&m_gs_p);
	if (FAILED(hr))
		return hr;

	hr = m_gs_p->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl, (void **)&m_vdc_p);
	if (FAILED(hr))
		return hr;

	hr = m_gs_p->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoPresenter, (void **)&m_vp_p);
	if (FAILED(hr))
		return hr;


	m_vdc_p->SetVideoWindow(m_owner_p);
	UpdateVWPos_p();

	return hr;
}

void CPlayer::TuneVideoWindow()
{
	m_vdc->SetVideoWindow(m_owner);
	UpdateVWPos();
}

void CPlayer::UpdateVWPos()
{
	if (m_vdc)
	{	
		RECT r;
		GetClientRect(m_owner, &r);
		m_vdc->SetVideoPosition(NULL,&r);
	}
}


void CPlayer::UpdateVWPos_p()
{
	if (m_vdc_p)
	{	
		RECT r;
		GetClientRect(m_owner_p, &r);
		m_vdc_p->SetVideoPosition(NULL,&r);
	}
}


void CPlayer::RepaintVW()
{
	if (m_vdc)
	{
		m_vdc->RepaintVideo();
	}
}

void CPlayer::RepaintVW_p()
{
	if (m_vdc_p)
	{
		m_vdc_p->RepaintVideo();
	}
}

void CPlayer::SetVideoParams_p(CComPtr<IBaseFilter> &src, CComPtr<IMFVideoDisplayControl> &vdc)
{
	CComPtr<IVideoSourceFilter>pSrc;
	HRESULT hr = src->QueryInterface(IID_IVideoSourceFilter, (void **)&pSrc);
	if (SUCCEEDED(hr))
	{
		SIZE dimensions;
		SIZE ratio;
		m_vdc->GetNativeVideoSize(&dimensions, &ratio);
		pSrc->SetVideoSize(dimensions);
	}
}

void CPlayer::Grab()
{
	CComPtr<IVideoMixingFilter>m;
	m_vmix->QueryInterface(IID_IVideoMixingFilter, (void **)&m);

	CComPtr<IVideoSourceFilter>s1;
	m_src1_p->QueryInterface(IID_IVideoSourceFilter, (void **)&s1);

	CComPtr<IVideoSourceFilter>s2;
	m_src2_p->QueryInterface(IID_IVideoSourceFilter, (void **)&s2);

	m->Grab(s1,s2);
	m.Release();
	s1.Release();
	s2.Release();
}

void CPlayer::Reset()
{
	RemoveFromRot(m_rot);
	if (m_mc) m_mc.Release();
	if (m_ms) m_ms.Release();
	if (m_gs) m_gs.Release();
	if (m_vfs) m_vfs.Release();
	if (m_vdc) m_vdc.Release();

//	if (m_ivlc)
//		m_ivlc.Release();
	
	if (m_src1) m_src1.Release();
	if (m_src2)	m_src2.Release();
	
	//if (m_ffdshow1)
	//	m_ffdshow1.Release();

	//if (m_ffdshow2)
	//	m_ffdshow2.Release();

	if (m_lav1)	m_lav1.Release();
	if (m_lav2)	m_lav2.Release();


	if (m_vmix)	m_vmix.Release();
	if (m_evr)m_evr.Release();
    if (m_gb) m_gb.Release();

	// Second graph	
	if (m_src1_p) m_src1_p.Release();
	if (m_src2_p) m_src2_p.Release();
	if (m_vmix_p) m_vmix_p.Release();
	if (m_evr_p) m_evr_p.Release();
	if (m_mc_p) m_mc_p.Release();
	if (m_gb_p)	m_gb_p.Release();
}

HRESULT CPlayer::Play()
{
	HRESULT hr = S_OK;
	if (!m_mc)
		return S_FALSE;

	
	
	//RefreshBBox(m_vmix, m_vmix_p);

	hr = m_mc->Run();
	if (FAILED(hr))
		return hr;

	Sleep(1000);

	hr = m_mc_p->Stop();
	if (FAILED(hr))
		return hr;


	return hr;
}

HRESULT CPlayer::Pause()
{

	HRESULT hr = S_OK;
	if (!m_mc)
		return S_FALSE;

	Grab();
	
		
	hr = m_mc_p->Run();
	if (FAILED(hr))
		return hr;

	Sleep(300);
	
	hr = m_mc->Pause();
	if (FAILED(hr))
		return hr;

	return hr;
}

void CPlayer::SwitchVW(BOOL play)
{
	RECT r1;
	r1.left = 0;
	r1.top = 0;
	r1.right = 1;
	r1.bottom = 1;

	HRESULT hr;
	if (play)
	{
		
		//RECT r;
		//GetClientRect(m_owner, &r);
		//m_vdc->SetVideoPosition(NULL,&r);
		//m_vdc_p->SetVideoPosition(NULL, &r1);
	}
	else
	{
	//	hr = m_vp->ProcessMessage(MFVP_MESSAGE_FLUSH,0);
		
		//RECT r;
		//GetClientRect(m_owner_p, &r);
		//m_vdc->SetVideoPosition(NULL, &r1);
		//m_vdc_p->SetVideoPosition(NULL,&r);
	}
}

HRESULT CPlayer::Stop()
{
	HRESULT hr = S_OK;
	if (!m_mc)
		return S_FALSE;

	hr = m_mc_p->Stop();
	if (FAILED(hr))
		return hr;


	hr = m_mc->Stop();
	if (FAILED(hr))
		return hr;


	SetPosition(0);
	return hr;
}

HRESULT CPlayer::StepForward()
{
	HRESULT hr = S_OK;
	if (!m_vfs)
		return S_FALSE;

	hr = m_vfs->Step(1, NULL);
	if (FAILED(hr))
		return hr;

	Pause();

	return hr;
}

HRESULT CPlayer::AddFilter(const GUID &guid, LPWSTR name, CComPtr<IBaseFilter> &filter, CComPtr<IGraphBuilder> &gb) 
{
	HRESULT hr = S_OK;
	CComPtr<IBaseFilter> flt;
	hr = flt.CoCreateInstance(guid);
	if (FAILED(hr))
		return hr;

	filter = flt;

	hr = gb->AddFilter(filter, name);
	if (FAILED(hr))
		return hr;	
	
	return hr;
}

HRESULT CPlayer::Connect(CComPtr<IBaseFilter>&sflt, CComPtr<IBaseFilter>&dflt, CComPtr<IGraphBuilder> &gb)
{
	HRESULT hr = S_OK;
	CComPtr<IEnumPins>senm;
	sflt->EnumPins(&senm);

	CComPtr<IEnumPins>denm;
	dflt->EnumPins(&denm);
	
	CComPtr<IPin>opin;
	ULONG ftch1 = 0;
	while(senm->Next(1,&opin,&ftch1) == S_OK)
	{
		PIN_DIRECTION spd;
		opin->QueryDirection(&spd);
		if (spd == PINDIR_OUTPUT)
		{
			CComPtr<IPin>ipin;
			ULONG ftch2 = 0;
			while(denm->Next(1,&ipin,&ftch2) == S_OK)
			{
				PIN_DIRECTION dpd;
				ipin->QueryDirection(&dpd);
				if (dpd == PINDIR_INPUT)
				{
					hr = gb->Connect(opin,ipin);
					if (SUCCEEDED(hr))
						return hr;
				}
				ipin.Release();
			}
		}
		opin.Release();
	}
	return hr;
}


//mf added below
CComPtr<IPin> CPlayer::GetPin(IBaseFilter *pFilter, LPCOLESTR pinname)
{
	CComPtr<IEnumPins>  pEnum;
	CComPtr<IPin>       pPin;
	
	HRESULT hr = pFilter->EnumPins(&pEnum);
	if (FAILED(hr))
		return NULL;

	while(pEnum->Next(1, &pPin, 0) == S_OK)
	{
		PIN_INFO pinfo;
		pPin->QueryPinInfo(&pinfo);
		
		BOOL found = !wcsicmp(pinname, pinfo.achName);
		if (pinfo.pFilter) pinfo.pFilter->Release();
		if (found)
			return pPin;
		pPin.Release();
	}
	printf("Pin not found!\n");
	return NULL;
}


HRESULT CPlayer::AddToRot(IUnknown *graph, DWORD *reg) 
{
    CComPtr<IMoniker> moniker = NULL;
    CComPtr<IRunningObjectTable>rot = NULL;

    if (FAILED(GetRunningObjectTable(0, &rot))) 
    {
        return E_FAIL;
    }
    
    const size_t STRING_LENGTH = 256;

    WCHAR wsz[STRING_LENGTH];
    StringCchPrintfW(wsz, STRING_LENGTH, L"FilterGraph %08x pid %08x", (DWORD_PTR)graph, GetCurrentProcessId());
    
    HRESULT hr = CreateItemMoniker(L"!", wsz, &moniker);
    if (SUCCEEDED(hr)) 
    {
        hr = rot->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, 
							graph, 
							moniker, 
							reg);
    }
    return hr;
}  

void CPlayer::RemoveFromRot(DWORD reg)
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT)))
	{
        pROT->Revoke(reg);
        pROT->Release();
    }
}


void CPlayer::SetFile(WCHAR *file1, WCHAR *file2)
{
	lstrcpy(m_file1, file1);
	lstrcpy(m_file2, file2);
}

LONGLONG CPlayer::GetDuration()
{
	if (!m_ms)
		return 0;

	LONGLONG duration;
	HRESULT hr = m_ms->GetDuration(&duration);
	if (FAILED(hr))
		return 0;

	return duration;
}

LONGLONG CPlayer::GetPosition()
{
	if (!m_ms)
		return 0;

	LONGLONG position, stop;
	HRESULT hr = m_ms->GetPositions(&position, &stop);
	if (FAILED(hr))
		return 0;

	return position;
}

void CPlayer::SetPosition(LONGLONG position)
{
	if (!m_ms)
		return;

	HRESULT hr = m_ms->SetPositions(&position, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning);
	if (FAILED(hr))
		return ;
}



void CPlayer::SetBoundingBox(CComPtr<IBaseFilter> &vmix, int x, int y, int width, int height)
{
	CComPtr<IVideoMixingFilter>pMix;
	HRESULT hr = vmix->QueryInterface(IID_IVideoMixingFilter, (void **)&pMix);
	if (SUCCEEDED(hr))
	{
		pMix->SetBoundingBox(x,y,width,height);
		pMix.Release();
	}
}

void CPlayer::SetBoundingBox(int x, int y, int width, int height)
{
	if (m_vmix)
		SetBoundingBox(m_vmix,x,y,width,height);
	if (m_vmix_p)
		SetBoundingBox(m_vmix_p,x,y,width,height);	
}

void  CPlayer::GetBoundingBox(RECT *r)
{
	if (m_vmix)
	{
		CComPtr<IVideoMixingFilter>pMix;
		HRESULT hr = m_vmix->QueryInterface(IID_IVideoMixingFilter, (void **)&pMix);
		if (SUCCEEDED(hr))
		{
			pMix->GetBoundingBox(r);
			pMix.Release();
		}
	}
}

int  CPlayer::GetVideoWidth()
{
	if (!m_vdc)
		return -1;

	SIZE dimensions;
	SIZE ratio;
	m_vdc->GetNativeVideoSize(&dimensions, &ratio);
	return dimensions.cx;
}

int  CPlayer::GetVideoHeight()
{
	if (!m_vdc)
		return -1;

	SIZE dimensions;
	SIZE ratio;
	m_vdc->GetNativeVideoSize(&dimensions, &ratio);
	return dimensions.cy;
}

void CPlayer::GetBBox(RECT &r)
{
	if (m_vmix)
	{
		CComPtr<IVideoMixingFilter>pMix;
		HRESULT hr = m_vmix->QueryInterface(IID_IVideoMixingFilter, (void **)&pMix);
		if (SUCCEEDED(hr))
		{
			pMix->GetBoundingBox(&r);
			pMix.Release();
		}
	}
}


void CPlayer::RefreshBBox(CComPtr<IBaseFilter> &vmix_src, CComPtr<IBaseFilter> &vmix_dst)
{
	RECT r;
	CComPtr<IVideoMixingFilter>src;
	vmix_src->QueryInterface(IID_IVideoMixingFilter, (void **)&src);
	//src->Flush();
	src->GetBoundingBox(&r);


	CComPtr<IVideoMixingFilter>dst;
	vmix_dst->QueryInterface(IID_IVideoMixingFilter, (void **)&dst);
	dst->Flush();
	//dst->SetBoundingBox(r.left, r.top, r.right - r.left, r.bottom - r.top);
}
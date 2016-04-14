#pragma once

//#include "..\filter\ivlcsrc.h"
#include <evr.h>
#include <InitGuid.h>


DEFINE_GUID(CLSID_VideoMixingFilter, 
0xcf5d21b1, 0xe85a, 0x40f7, 0x82, 0xf, 0xd4, 0x12, 0x50, 0x4e, 0x1a, 0x19);

DEFINE_GUID(CLSID_VideoSourceFilter, 
0x71ce819b, 0xda9f, 0x496d, 0xb8, 0xc0, 0xaa, 0x8a, 0x4f, 0x56, 0x18, 0x75);

//DEFINE_GUID(CLSID_HaaliMediaSplitter, 
//0x55DA30FC, 0xF16B, 0x49FC, 0xBA, 0xA5, 0xAE, 0x59, 0xFC, 0x65, 0xF8, 0x2D);

//DEFINE_GUID(CLSID_FFDShow, 
//0x04FE9017, 0xF873, 0x410E, 0x87, 0x1E, 0xAB, 0x91, 0x66, 0x1A, 0x4E, 0xF7);

//using latest LAV Video Decoder
DEFINE_GUID(CLSID_LAV_Video_Decoder,
0xEE30215D, 0x164F, 0x4A92, 0xA4, 0xEB, 0x9D, 0x4C, 0x13, 0x39, 0x0F, 0x9F);

//using latest LAV Splitter Source
DEFINE_GUID(CLSID_LAV_Splitter_Source,
0xB98D13E7, 0x55DB, 0x4385, 0xA3, 0x3D, 0x09, 0xFD, 0x1B, 0xA2, 0x63, 0x38);


using namespace std;
class CPlayer
{
private:
	HWND m_owner;

	WCHAR m_file1[MAX_PATH];
	WCHAR m_file2[MAX_PATH];
	DWORD m_rot;

	CComPtr<IGraphBuilder>m_gb;
	CComPtr<IBaseFilter>m_src1;
	CComPtr<IBaseFilter>m_src2;
	//CComPtr<IBaseFilter>m_ffdshow1;
	//CComPtr<IBaseFilter>m_ffdshow2;
	CComPtr<IBaseFilter>m_lav1;
	CComPtr<IBaseFilter>m_lav2;
	CComPtr<IBaseFilter>m_vmix;
	CComPtr<IBaseFilter>m_evr;
	CComPtr<IMFGetService> m_gs;
	CComPtr<IMFVideoDisplayControl> m_vdc;
	CComPtr<IMFVideoPresenter> m_vp;

	CComPtr<IMediaControl>m_mc;
	CComPtr<IMediaSeeking>m_ms;
	CComPtr<IVideoWindow>m_vw;
	CComPtr<IVideoFrameStep>m_vfs;
	

	HWND m_owner_p;
	CComPtr<IGraphBuilder>m_gb_p;
	CComPtr<IBaseFilter>m_src1_p;
	CComPtr<IBaseFilter>m_src2_p;

	CComPtr<IBaseFilter>m_vmix_p;
	CComPtr<IBaseFilter>m_evr_p;
	CComPtr<IMFGetService> m_gs_p;
	CComPtr<IMFVideoDisplayControl> m_vdc_p;
	CComPtr<IMFVideoPresenter> m_vp_p;
	CComPtr<IMediaControl>m_mc_p;
	
	HRESULT AddFilter(const GUID &guid, LPWSTR name, CComPtr<IBaseFilter>&filter, CComPtr<IGraphBuilder> &gb);
	HRESULT Connect(CComPtr<IBaseFilter>&sflt, CComPtr<IBaseFilter>&dflt, CComPtr<IGraphBuilder> &gb);
	CComPtr<IPin> GetPin(IBaseFilter *pFilter, LPCOLESTR pinname);

	HRESULT AddToRot(IUnknown *graph, DWORD *reg);
	void RemoveFromRot(DWORD reg);
	void TuneVideoWindow();

	void SetBoundingBox(CComPtr<IBaseFilter> &vmix, int x, int y, int width, int height);
	void SetVideoParams_p(CComPtr<IBaseFilter> &src1, CComPtr<IMFVideoDisplayControl> &vdc);
	void Grab();
	void RefreshBBox(CComPtr<IBaseFilter> &vmix_src, CComPtr<IBaseFilter> &vmix_dst);
public:
	CPlayer();
	~CPlayer();

	HRESULT Init(HWND owner, HWND owner_p);
	void Reset();

	HRESULT Play();
	HRESULT Pause();
	HRESULT Stop();
	HRESULT StepForward();
	void SwitchVW(BOOL play);
	void UpdateVWPos();
	void RepaintVW();
	void UpdateVWPos_p();
	void RepaintVW_p();
	void SetFile(WCHAR *file1, WCHAR *file2);
	LONGLONG GetDuration();
	LONGLONG GetPosition();
	void SetPosition(LONGLONG position);
	void SetBoundingBox(int x, int y, int width, int height);
	void GetBoundingBox(RECT *r);
	int GetVideoWidth();
	int GetVideoHeight();
	void GetBBox(RECT &r);
};


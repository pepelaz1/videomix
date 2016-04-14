//------------------------------------------------------------------------------
// File: PushSource.H
//
// Desc: DirectShow sample code - In-memory push mode source filter
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#include <strsafe.h>

// UNITS = 10 ^ 7  
// UNITS / 30 = 30 fps;
// UNITS / 20 = 20 fps, etc
const REFERENCE_TIME FPS_30 = UNITS / 30;
const REFERENCE_TIME FPS_20 = UNITS / 20;
const REFERENCE_TIME FPS_10 = UNITS / 10;
const REFERENCE_TIME FPS_5  = UNITS / 5;
const REFERENCE_TIME FPS_4  = UNITS / 4;
const REFERENCE_TIME FPS_3  = UNITS / 3;
const REFERENCE_TIME FPS_2  = UNITS / 2;
const REFERENCE_TIME FPS_1  = UNITS / 1;

const REFERENCE_TIME rtDefaultFrameLength = FPS_10;

// Filter name strings
#define g_wszPushBitmap     L"Video Source Filter"

// Number of bitmap files to load in the CPushPinBitmapSet class
#define NUM_FILES 5

#include "IVideoSourceFilter.h"

/**********************************************
 *
 *  Class declarations
 *
 **********************************************/

class CPushPinBitmap : public CSourceStream
{
protected:
    int m_FramesWritten;				// To track where we are in the file
    BOOL m_bZeroMemory;                 // Do we need to clear the buffer?
    CRefTime m_rtSampleTime;	        // The time stamp for each sample

   // BITMAPINFO *m_pBmi;                 // Pointer to the bitmap header
	BITMAPINFO m_bmi;                 // Pointer to the bitmap header
  //  DWORD       m_cbBitmapInfo;         // Size of the bitmap header
	
	// File opening variables 
	//HANDLE m_hFile;                     // Handle returned from CreateFile
  //  BYTE * m_pFile;                     // Points to beginning of file buffer
	BYTE * m_pImage;                    // Points to pixel bits                                      

    int m_iFrameNumber;
    const REFERENCE_TIME m_rtFrameLength;

    CCritSec m_cSharedState;            // Protects our internal state
    CImageDisplay m_Display;            // Figures out our media type for us

    virtual HRESULT OnThreadCreate(void);
  //  virtual HRESULT OnThreadDestroy(void) {return NOERROR;};
//    virtual HRESULT OnThreadStartPlay(void) {return NOERROR;};
public:

    CPushPinBitmap(HRESULT *phr, CSource *pFilter);
    ~CPushPinBitmap();

    // Override the version that offers exactly one media type
    HRESULT GetMediaType(CMediaType *pMediaType);
    HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest);
    HRESULT FillBuffer(IMediaSample *pSample);
    
    // Quality control
	// Not implemented because we aren't going in real time.
	// If the file-writing filter slows the graph down, we just do nothing, which means
	// wait until we're unblocked. No frames are ever dropped.
    STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q)
    {
        return E_FAIL;
    }

	HRESULT SetVideoSize (SIZE size);
	HRESULT SetRawData(BYTE *data);
};



class CPushSourceBitmap : public CSource, public IVideoSourceFilter
{
	DECLARE_IUNKNOWN
private:
    // Constructor is private because you have to use CreateInstance
    CPushSourceBitmap(IUnknown *pUnk, HRESULT *phr);
    ~CPushSourceBitmap();

    CPushPinBitmap *m_pPin;

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);
public:
    static CUnknown * WINAPI CreateInstance(IUnknown *pUnk, HRESULT *phr);  

	// IVideoSourceFilter implementation
	STDMETHODIMP SetVideoSize (SIZE size);
	STDMETHODIMP SetRawData (BYTE *data);
};



/** @file

MODULE				: VideoMixingFilter

FILE NAME			: VideoMixingFilter.h

DESCRIPTION			: Video Mixing Filter for RGB24 and RGB32 Media
					  
LICENSE: Software License Agreement (BSD License)

Copyright (c) 2008 - 2012, CSIR
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* Neither the name of the CSIR nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

===========================================================================
*/
#pragma once

#include "VideoMixingBase.h"
#include "CSettingsInterface.h"
#include  "FilterParameters.h"


// {CF5D21B1-E85A-40F7-820F-D412504E1A19}
static const GUID CLSID_VideoMixingFilter = 
{ 0xcf5d21b1, 0xe85a, 0x40f7, { 0x82, 0xf, 0xd4, 0x12, 0x50, 0x4e, 0x1a, 0x19 } };


// {55D79085-D458-4F8F-B2B2-E84D19A1C470}
static const GUID CLSID_VideoMixingProperties = 
{ 0x55d79085, 0xd458, 0x4f8f, { 0xb2, 0xb2, 0xe8, 0x4d, 0x19, 0xa1, 0xc4, 0x70 } };


#ifdef _BUILD_FOR_SHORT
typedef short yuvType;
#else
typedef signed char yuvType ;
#endif

#include <list>
using namespace std;

#include "IVIdeoMixingFilter.h"

// Forward declarations
class PicConcatBase;

/**
 * \ingroup DirectShowFilters
 * Video Mixing Filter for RGB24 and RGB32 Media
 */
class VideoMixingFilter	:	public VideoMixingBase,
							public CSettingsInterface,
							public ISpecifyPropertyPages,
							public IVideoMixingFilter

{
public:
	DECLARE_IUNKNOWN;

	/// Constructor
	VideoMixingFilter();
	/// Destructor
	~VideoMixingFilter();

	/// Static object-creation method (for the class factory)
	static CUnknown * WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *pHr); 
	
	///  From CBasePin
	virtual HRESULT CheckOutputType(const CMediaType* pMediaType);

	
	virtual void DoBoundingBoxCompositeOverwrite(BYTE* pImgBB, BYTE* pImgDest, int start_x, int start_y, int width_bb, int height_bb);

	/// Overridden from VideoMixingBase 
	virtual HRESULT ReceiveFirstSample(IMediaSample *pSample);
	/// Overridden from VideoMixingBase 
	virtual HRESULT ReceiveSecondSample(IMediaSample *pSample);
	/// Overridden from VideoMixingBase 
	virtual HRESULT CreateVideoMixer(const CMediaType *pMediaType, int nIndex);
	/// Overridden from VideoMixingBase 
	virtual HRESULT SetOutputDimensions(BITMAPINFOHEADER* pBmih1, BITMAPINFOHEADER* pBmih2);

	virtual STDMETHODIMP BeginFlush(int nIndex);
	virtual STDMETHODIMP EndFlush(int nIndex);

	HRESULT GenerateOutputSample(IMediaSample *pSample, int nIndex);
	virtual HRESULT StopStreaming();

	virtual void initParameters();

	/// For configuration dialog
	STDMETHODIMP GetPages(CAUUID *pPages)
	{
		if (pPages == NULL) return E_POINTER;
		pPages->cElems = 1;
		pPages->pElems = (GUID*)CoTaskMemAlloc(sizeof(GUID));
		if (pPages->pElems == NULL) 
		{
			return E_OUTOFMEMORY;
		}
		pPages->pElems[0] = CLSID_VideoMixingProperties;
		return S_OK;
	}

	/// For configuration dialog
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv)
	{
		if (riid == IID_ISpecifyPropertyPages)
		{
			return GetInterface(static_cast<ISpecifyPropertyPages*>(this), ppv);
		}
		else if(riid == (IID_ISettingsInterface))
		{
			return GetInterface((ISettingsInterface*) this, ppv);
		}
		else if(riid == (IID_IVideoMixingFilter))
		{
			return GetInterface((IVideoMixingFilter*) this, ppv);
		}
		else
		{
			// Call the parent class.
			return VideoMixingBase::NonDelegatingQueryInterface(riid, ppv);
		}
	}

	// IVideoMixingFilter
	STDMETHODIMP SetBoundingBox (int x, int y,int width,int height);

private:

	/// Pointers to our last received Media Sample
	//BYTE* m_pSampleBuffers[2];
	// Sizes
	int m_nSampleSizes[2];

	/// 0 = horizontal, 1 = vertical
	int m_nOrientation;

	// Picture concatenation class
	PicConcatBase* m_pPicConcat;

	/// Stores bytes per needed to store pixel according to media type
	double m_nBytesPerPixel;

	
	list<BYTE *>m_FirstQueue;
	list<BYTE *>m_SecondQueue;
	CCritSec m_QueuesLock;
	void ClearQueues();

	// Bounding box stuff
	CCritSec m_csBBox;

	int m_nBBoxX;
	int m_nBBoxY;
	int m_nBBoxWidth;
	int m_nBBoxHeight;
};

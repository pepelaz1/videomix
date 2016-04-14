/** @file

MODULE				: VideoMixingFilter

FILE NAME			: VideoMixingFilter.cpp

DESCRIPTION			: 
					  
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
#include "stdafx.h"

// CSIR includes
#include "VideoMixingFilter.h"

#include "CommonDefs.h"
#include "PicConcatRGB24Impl.h"
#include "PicConcatRGB32Impl.h"


VideoMixingFilter::VideoMixingFilter()
	:VideoMixingBase(NAME("Video Mixing Filter"), 0, CLSID_VideoMixingFilter),
	m_pPicConcat(NULL), m_nBBoxX(0), m_nBBoxY(0), m_nBBoxWidth(0),m_nBBoxHeight(0), m_FirstBuff(NULL), m_SecondBuff(NULL), m_flush(FALSE)
{
//	m_pSampleBuffers[0] = NULL;
//	m_pSampleBuffers[1] = NULL;

	m_nSampleSizes[0] = 0;
	m_nSampleSizes[1] = 0;

	// Init parameters
	initParameters();
}

VideoMixingFilter::~VideoMixingFilter()
{
	if (m_pPicConcat)
	{
		delete m_pPicConcat;
		m_pPicConcat = NULL;
	}

	ClearQueues();

	if (m_FirstBuff) 
	{
		delete m_FirstBuff;
		m_FirstBuff = NULL;
	}

	if (m_SecondBuff) 
	{
		delete m_SecondBuff;
		m_SecondBuff = NULL;
	}
}

void VideoMixingFilter::ClearQueues()
{
	CAutoLock lck(&m_QueuesLock);
	while (m_FirstQueue.size() > 0)
	{
		BYTE *p = m_FirstQueue.front();
		delete p;
		m_FirstQueue.pop_front();
	}
	m_FirstQueue.clear();

	while (m_SecondQueue.size() > 0)
	{
		BYTE *p = m_SecondQueue.front();
		delete p;
		m_SecondQueue.pop_front();
	}
	m_SecondQueue.clear();
}

CUnknown * WINAPI VideoMixingFilter::CreateInstance( LPUNKNOWN pUnk, HRESULT *pHr )
{
	VideoMixingFilter* pFilter = new VideoMixingFilter();
	return pFilter;
}

void VideoMixingFilter::initParameters()
{
	addParameter(ORIENTATION, &m_nOrientation, 0);
}

STDMETHODIMP VideoMixingFilter::SetBoundingBox (int x, int y,int width,int height)
{
	CAutoLock lck(&m_csBBox);
	m_nBBoxX = x;
	m_nBBoxY = y;
	m_nBBoxWidth = width;
	m_nBBoxHeight = height;
	return S_OK;
}

STDMETHODIMP VideoMixingFilter::GetBoundingBox (RECT *r)
{
	r->left = m_nBBoxX;
	r->top = m_nBBoxY;
	r->right = m_nBBoxX + m_nBBoxWidth;
	r->bottom = m_nBBoxY + m_nBBoxHeight;
	return S_OK;
}

STDMETHODIMP VideoMixingFilter::Grab (IVideoSourceFilter *s1, IVideoSourceFilter *s2)
{
	//m_QueuesLock.Lock();
	s1->SetRawData(m_FirstBuff);
	s2->SetRawData(m_SecondBuff);
	//m_QueuesLock.Unlock();
	return S_OK;
}


STDMETHODIMP  VideoMixingFilter::Flush()
{
	m_flush = TRUE;
	/*if ( m_FirstBuff && m_SecondBuff)
	{
		m_FirstQueue.push_back(m_FirstBuff);
		m_SecondQueue.push_back(m_SecondBuff);
	}*/
	return S_OK;
}

void VideoMixingFilter::DoBoundingBoxCompositeOverwrite(BYTE* pImgBB, BYTE* pImgDest, int start_x, int start_y, int width_bb, int height_bb)
{
	CAutoLock lck(&m_csBBox);

	int bpp = m_nBytesPerPixel;
	int video_width = m_nOutputWidth;
	int video_height = m_nOutputHeight;

	if ( start_y + height_bb >= m_nOutputHeight-1)
		return;

	int x = 0;
	int y = 0;
		
	//half image test
	//memcpy(pSrc1, pSrc2, bpp*video_width*(m_nOutputHeight /2 ));
//	memcpy(pSrc1, pImgDest, bpp*video_width*video_height);
	
	//get pointers to the first byte of the bounding box
	int bb_index = (start_y*video_width*bpp) + (start_x*bpp);
//	int bb_index = (start_x*video_height*bpp) + (start_y*bpp);


//	pImgDest += bb_index;
//	pImgBB += bb_index;

	//do a red line on top/bottom of BB
	for (x = 0; x < width_bb*bpp; x=x+bpp)
	{
		pImgDest[bb_index + x] = 0xFF;
		pImgDest[bb_index + x + 1] = 0xFF;
		pImgDest[bb_index + x + 2] = 0xFF;
		//bb_index += bpp;
	}
	
	bb_index += bpp*video_width; //advance to start of next bb line

	//now overwrite source1 with source 2 inside the bounding box
	for(y = 0; y < height_bb; y++)
	{
		//do a red pixel on the left
		pImgDest[bb_index] = 0xFF;
		pImgDest[bb_index + 1] = 0xFF;
		pImgDest[bb_index + 2] = 0xFF;
		bb_index += bpp; //advance to next pixel

		x = width_bb*bpp - bpp;
		while (x > 0)
		{
			pImgDest[bb_index] = pImgBB[bb_index];
			x--;
			bb_index++;
		}
		
		//do a red pixel on the right
		pImgDest[bb_index] = 0xFF;
		pImgDest[bb_index + 1] = 0xFF;
		pImgDest[bb_index + 2] = 0xFF;
		
		bb_index += bpp*(video_width - width_bb); //go to start of next line
	}
	
	//do a red line on top/bottom of BB
	for (x = 0; x < width_bb*bpp; x=x+bpp)
	{
		pImgDest[bb_index + x] = 0xFF;
		pImgDest[bb_index + x + 1] = 0xFF;
		pImgDest[bb_index + x + 2] = 0xFF;
	}
	

}//end Insert.

HRESULT VideoMixingFilter::GenerateOutputSample(IMediaSample *pSample, int nIndex)
{ 
	if (!m_FirstQueue.size() || !m_SecondQueue.size())
		return S_OK;
	
//	if (m_flush)
//	{
//		return S_OK;
//	}

	//mf added
	bool readyForReceive = false;

	// Prepare output sample
	AM_SAMPLE2_PROPERTIES * const pProps = m_vInputPins[nIndex]->SampleProps();
	//DbgLog((LOG_TRACE,0,TEXT("Video Mixer: Sample Received from input 0: start time: %I64d end time: %I64d"), pProps->tStart, pProps->tStop));
	if (pProps->dwStreamId != AM_STREAM_MEDIA) {
		return m_vInputPins[nIndex]->Receive(pSample);
	}


	
	IMediaSample * pOutSample;
	// Set up the output sample
	HRESULT hr = InitializeOutputSample(pSample, &pOutSample, nIndex, 0);
	if (FAILED(hr)) {
		return hr;
	}

	BYTE *pBufferOut = NULL;
	hr = pOutSample->GetPointer(&pBufferOut);
	if (FAILED(hr))
	{
		return hr;
	}

	/*
	////////////////////////////////////////
	////Set orientation
	if (m_nOrientation == 0)
	{
		// Horizontal 3 (RIGHT)
		m_pPicConcat->SetOrientation(3);
	}
	else
	{
		// Vertical = 1 (BOTTOM)
		m_pPicConcat->SetOrientation(1);
	}

	// Concat the pictures
	
	
	if (m_pSampleBuffers[0] && m_pSampleBuffers[1])
		m_pPicConcat->Concat(m_pSampleBuffers[0], m_pSampleBuffers[1], pBufferOut);
	else if (m_pSampleBuffers[0])
	{
		int nWidth = m_pPicConcat->Get1stWidth();
		int nHeight = m_pPicConcat->Get1stHeight();
		memcpy(pBufferOut, m_pSampleBuffers[0], nWidth * nHeight* m_nBytesPerPixel);
	}
	else
	{
		int nWidth = m_pPicConcat->Get2ndWidth();
		int nHeight = m_pPicConcat->Get2ndHeight();
		memcpy(pBufferOut, m_pSampleBuffers[1], nWidth * nHeight* m_nBytesPerPixel);
	}
	//////////////////////////////////////////	
	*/

	////////////////////////
	// MF test below start
	///////////////////////

	//if (m_pSampleBuffers[0] )
	//{
	//m_QueuesLock.Lock();
	//{
		//wchar_t str[256];
		//wsprintf(str,L"output - first queue size = %d, second_queue_size = %d index=%d\n", m_FirstQueue.size(),m_SecondQueue.size(), nIndex );
		//OutputDebugString(str);

		
		BYTE *pFirst = m_FirstQueue.front();
		/*
		memcpy(pBufferOut + (int)m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight/2, pFirst 
			+ (int)m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight/2, m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight/2); 
		*/
		memcpy(pBufferOut,pFirst, m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight); //copy whole 1st image
		//delete pFirst;
		
		BYTE *pSecond = m_SecondQueue.front();
		//memcpy(pBufferOut, pSecond, m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight/2 ); //overwrite half
		//if (!m_flush)
		DoBoundingBoxCompositeOverwrite(pSecond,pBufferOut, m_nBBoxX, m_nBBoxY, m_nBBoxWidth, m_nBBoxHeight);
		//delete pSecond;
			
		if (m_FirstQueue.size() > 0)
		{
			BYTE *pb =  m_FirstQueue.front();
			if (!m_FirstBuff)
				m_FirstBuff = new BYTE[(int)(m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight)];
			memcpy(m_FirstBuff,pb,m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight);
			m_FirstQueue.pop_front();	
			delete pb;
		}

		if (m_SecondQueue.size() > 0)
		{
			BYTE *pb =  m_SecondQueue.front();
			if (!m_SecondBuff)
				m_SecondBuff = new BYTE[(int)(m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight)];
			memcpy(m_SecondBuff,pb,m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight);
			m_SecondQueue.pop_front();	
			delete pb;
		}

		//if (nIndex == 0)
		//{

			
		//}
	//}
	//m_QueuesLock.Unlock();

	///these are 1 frame out of sync when i step
/*	if (m_pSampleBuffers[0] )
	{
		//write top half of input 0 to top half of pBufferOut
		memcpy(pBufferOut + (int)m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight/2, m_pSampleBuffers[0] + (int)m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight/2, m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight/2); 
		//memcpy(pBufferOut, m_pSampleBuffers[1], m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight/2 ); //overwrite half
		
	}
	if (m_pSampleBuffers[1] )
	{
		//write bottom half of input 1 to bottom half of pBufferOut
		memcpy(pBufferOut, m_pSampleBuffers[1], m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight/2 ); //overwrite half
	}*/

	//MF end test

	pOutSample->SetActualDataLength(m_nOutputSize);
	pOutSample->SetSyncPoint(TRUE);

	// Stop the clock and log it (if PERF is defined)
	MSR_STOP(m_idTransform);

	if (FAILED(hr)) {
		DbgLog((LOG_TRACE,1,TEXT("Error from transform")));
	} 
	else 
	{
		// the Transform() function can return S_FALSE to indicate that the
		// sample should not be delivered; we only deliver the sample if it's
		// really S_OK (same as NOERROR, of course.)
		if (hr == NOERROR) 
		{
			//if (readyForReceive) //mf added
				hr = m_vOutputPins[0]->GetInputPin()->Receive(pOutSample);
		} 
		else 
		{
			// S_FALSE returned from Transform is a PRIVATE agreement
			// We should return NOERROR from Receive() in this cause because returning S_FALSE
			// from Receive() means that this is the end of the stream and no more data should
			// be sent.
			if (S_FALSE == hr) 
			{
				//  Release the sample before calling notify to avoid
				//  deadlocks if the sample holds a lock on the system
				//  such as DirectDraw buffers do
				pOutSample->Release();
				return NOERROR;
			}
		}
	}
	// release the output buffer. If the connected pin still needs it,
	// it will have addrefed it itself.

	pOutSample->Release();
	
	return hr;
}

STDMETHODIMP VideoMixingFilter::Pause()
{
	HRESULT hr = VideoMixingBase::Pause();
	//ClearQueues();
	return hr;
}

STDMETHODIMP VideoMixingFilter::BeginFlush(int nIndex)
{
	ClearQueues();
	return VideoMixingBase::BeginFlush(nIndex);
}
STDMETHODIMP  VideoMixingFilter::EndFlush(int nIndex)
{
	return VideoMixingBase::EndFlush(nIndex);
}

HRESULT VideoMixingFilter::StopStreaming()
{
	HRESULT hr = VideoMixingBase::StopStreaming();
	if (FAILED(hr))
		return hr;
	
	ClearQueues();
	
	if (m_FirstBuff)
	{
		delete m_FirstBuff;
		m_FirstBuff = NULL;
	}

	if (m_SecondBuff)
	{
		delete m_SecondBuff;
		m_SecondBuff = NULL;
	}
	
	return hr;
}

HRESULT VideoMixingFilter::BeforeReceive()
{
	if (m_flush)
	{
		//ClearQueues();
		if ( m_FirstBuff && m_SecondBuff)
		{
			/*CAutoLock lck(&m_QueuesLock);
			list<BYTE *>::iterator it = m_FirstQueue.begin();
			for(;it != m_FirstQueue.end() ; ++it)
			{
				BYTE * p = (*it);
				memcpy(p, m_FirstBuff,m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight);
			}

			list<BYTE *>::iterator it2 = m_SecondQueue.begin();
			for(;it2 != m_SecondQueue.end() ; ++it2)
			{
				BYTE * p = (*it2);
				memcpy(p, m_SecondBuff,m_nBytesPerPixel*m_nOutputWidth*m_nOutputHeight);
			}*/
		}

		/*while (m_FirstQueue.size() > 0)
		{
			BYTE *p = m_FirstQueue.front();
			delete p;
			m_FirstQueue.pop_front();
		}
		m_FirstQueue.clear();

		while (m_SecondQueue.size() > 0)
		{
			BYTE *p = m_SecondQueue.front();
			delete p;
			m_SecondQueue.pop_front();
		}
		m_SecondQueue.clear();*/


		m_flush = FALSE;
	}
	return S_OK;
}

HRESULT VideoMixingFilter::ReceiveFirstSample( IMediaSample *pSample )
{
	// Copy the secondary sample into our buffer
	if (pSample)
	{
		BYTE *pBuffer = NULL;
		HRESULT hr = pSample->GetPointer(&pBuffer);
		if (FAILED(hr))
		{
			return hr;
		}

		// Copy the buffer
		//memcpy(m_pSampleBuffers[0], pBuffer, m_nSampleSizes[0]);
		m_QueuesLock.Lock();
		BYTE *buff = new BYTE[m_nSampleSizes[0]];
		memcpy(buff, pBuffer, m_nSampleSizes[0]);
		m_FirstQueue.push_back(buff);
		
		wchar_t str[256];
		wsprintf(str,L"first - first queue size = %d, second_queue_size = %d\n", m_FirstQueue.size(),m_SecondQueue.size() );
		OutputDebugString(str);

		//m_evFirst.Set();
		OutputDebugString(L"First\n");
		hr = GenerateOutputSample(pSample, 0);
		m_QueuesLock.Unlock();

		if (m_FirstQueue.size() > 1)
			Sleep(10);

		return hr;
	}
	else
	{
		return E_POINTER;
	}
}

HRESULT VideoMixingFilter::ReceiveSecondSample( IMediaSample *pSample )
{
	/*if (m_flush)
	{
		ClearQueues();
		m_flush = FALSE;
	}*/
	

	// Copy the secondary sample into our buffer
	if (pSample)
	{
		BYTE *pBuffer = NULL;
		HRESULT hr = pSample->GetPointer(&pBuffer);
		if (FAILED(hr))
		{
			return hr;
		}

		// Copy the buffer
		/*memcpy(m_pSampleBuffers[1], pBuffer, m_nSampleSizes[1]);
		m_evSecond.Set();*/
	//	OutputDebugString(L"Second\n");

		m_QueuesLock.Lock();
		BYTE *buff = new BYTE[m_nSampleSizes[1]];
		memcpy(buff, pBuffer, m_nSampleSizes[1]);
		m_SecondQueue.push_back(buff);
	
		//wchar_t str[256];
		//wsprintf(str,L"second - first queue size = %d, second_queue_size = %d\n", m_FirstQueue.size(),m_SecondQueue.size() );
		//OutputDebugString(str);


		hr = GenerateOutputSample(pSample, 1);
		m_QueuesLock.Unlock();

		if (m_SecondQueue.size() > 1)
			Sleep(10);
		return hr;
	}
	else
	{
		return E_POINTER;
	}

	/*
	m_pSecondaryStreamSample = pSample;
	AM_SAMPLE2_PROPERTIES * const pProps = m_vInputPins[nIndex]->SampleProps();
	// Check if the samples end time is AFTER the following primary samples start-time indicating that this
	// is the sample we want to lock
	//Get the new start and stop times
	m_tSecondarySampleStartTime = pProps->tStart;
	m_tSecondarySampleStopTime = pProps->tStop;
	//if (m_tSecondarySampleStopTime >= m_tPrimarySampleStopTime)
	{
		// Add a reference so that we can lock the sample
		m_pSecondaryStreamSample->AddRef();

		BYTE *pBuffer = NULL;
		// Copy the secondary sample into our buffer
		if (pSample)
		{
			HRESULT hr = pSample->GetPointer(&pBuffer);
			if (FAILED(hr))
			{
				return hr;
			}

			// Copy the buffer
			memcpy(m_pSecondarySampleBuffer, pBuffer, m_nSecondaryBufferSize);
		}
	}
	return S_OK;
	*/
}

HRESULT VideoMixingFilter::CreateVideoMixer( const CMediaType *pMediaType, int nIndex )
{
	// Create temporary sample buffers
	VIDEOINFOHEADER* pVih = (VIDEOINFOHEADER*) pMediaType->pbFormat;
	BITMAPINFOHEADER bmih = pVih->bmiHeader;

	m_nSampleSizes[nIndex] = DIBSIZE(bmih);
	/*if (m_pSampleBuffers[nIndex])
	{
		// Recreate in case dimensions have changed
		delete[] m_pSampleBuffers[nIndex];
	}
	m_pSampleBuffers[nIndex] = new BYTE[m_nSampleSizes[nIndex]];*/


	// Create appropriate picture concatenator
	if (!m_pPicConcat)
	{
		if (pMediaType->subtype == MEDIASUBTYPE_RGB24)
		{
			m_pPicConcat = new PicConcatRGB24Impl();
			m_nBytesPerPixel = BYTES_PER_PIXEL_RGB24;
		}
		else if (pMediaType->subtype == MEDIASUBTYPE_RGB32)
		{
			m_pPicConcat = new PicConcatRGB32Impl();
			m_nBytesPerPixel = BYTES_PER_PIXEL_RGB32;
		}
	}
	return S_OK;
}

HRESULT VideoMixingFilter::SetOutputDimensions( BITMAPINFOHEADER* pBmih1, BITMAPINFOHEADER* pBmih2 )
{
	if (pBmih1 && pBmih2)
	{
		// Verify that the dimensions match and set output width and height
		switch (m_nOrientation)
		{
		case 0: 
			{
				// Height must be the same
				if (pBmih1->biHeight != pBmih2->biHeight) return E_FAIL;
				m_nOutputWidth = pBmih1->biWidth + pBmih2->biWidth;
				m_nOutputHeight = pBmih1->biHeight;
				break;
			}
		case 1: 
			{
				// Width must be the same
				if (pBmih1->biWidth!= pBmih2->biWidth) return E_FAIL; 
				m_nOutputWidth = pBmih1->biWidth;
				m_nOutputHeight = pBmih1->biHeight + pBmih2->biHeight;
				break;
			}
		}
		
		//mf m_nOutputSize =  m_nSampleSizes[0] + m_nSampleSizes[1];
		m_nOutputHeight = pBmih1->biHeight; //mf
		m_nOutputWidth = pBmih1->biWidth; //mf
		m_nOutputSize =  m_nSampleSizes[0]; //mf
		
		// Setup the picture concatenator
		if (m_pPicConcat)
		{
			m_pPicConcat->Set1stDimensions(pBmih1->biWidth, pBmih1->biHeight);
			m_pPicConcat->Set2ndDimensions(pBmih2->biWidth, pBmih2->biHeight);
			m_pPicConcat->SetOutDimensions(m_nOutputWidth, m_nOutputHeight);
		}
	}
	else
	{
		if (pBmih1)
			m_pPicConcat->Set1stDimensions(pBmih1->biWidth, pBmih1->biHeight);
		else if (pBmih2)
			m_pPicConcat->Set2ndDimensions(pBmih2->biWidth, pBmih2->biHeight);
	}

	return S_OK;
}

HRESULT VideoMixingFilter::CheckOutputType( const CMediaType* pMediaType )
{
	if (m_nBytesPerPixel == BYTES_PER_PIXEL_RGB24)
	{
		if (*(pMediaType->Subtype()) == MEDIASUBTYPE_RGB24)
		{
			return S_OK;
		}
	}
	else if (m_nBytesPerPixel == BYTES_PER_PIXEL_RGB32)
	{
		if (*(pMediaType->Subtype()) == MEDIASUBTYPE_RGB32)
		{
			return S_OK;
		}
	}
	return S_FALSE;
}


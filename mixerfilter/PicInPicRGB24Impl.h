/** @file

MODULE						: PicInPicRGB24Impl

TAG								: PIPRGB24I

FILE NAME					: PicInPicRGB24Impl.h

DESCRIPTION				: An RGB24 implementation derived from the general
										PicInPicBase() class. Insert a packed RGB24 image
										into another packed RGB24 image.

COPYRIGHT					: (c)CSIR 2007-2010 all rights resevered

LICENSE						: Software License Agreement (BSD License)

RESTRICTIONS			: Redistribution and use in source and binary forms, with or without 
										modification, are permitted provided that the following conditions 
										are met:

										* Redistributions of source code must retain the above copyright notice, 
										this list of conditions and the following disclaimer.
										* Redistributions in binary form must reproduce the above copyright notice, 
										this list of conditions and the following disclaimer in the documentation 
										and/or other materials provided with the distribution.
										* Neither the name of the CSIR nor the names of its contributors may be used 
										to endorse or promote products derived from this software without specific 
										prior written permission.

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
#ifndef _PICINPICRGB24IMPL_H
#define _PICINPICRGB24IMPL_H


#include "PicInPicBase.h"

/**
 * \ingroup ImageLib
 * An RGB24 implementation derived from the general
 * PicInPicBase() class. Insert a packed RGB24 image
 * into another packed RGB24 image.
 */
class PicInPicRGB24Impl: public PicInPicBase
{
public:
	// Construction and destruction.
	PicInPicRGB24Impl(void) { }
	PicInPicRGB24Impl(int width, int height, int subWidth, int subHeight, int xPos, int yPos): PicInPicBase(width,height,subWidth,subHeight,xPos,yPos) { }
	virtual ~PicInPicRGB24Impl(void) {}

protected:
	// Interface.
	void DoInsert(void* pSubImg, void* pImg);
    void DoInsertWithBorder(void* pSubImg, void* pImg);
};//end _PICINPICRGB24IMPL_H.


#endif

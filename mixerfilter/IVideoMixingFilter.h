#ifndef __IVIDEOMIXINGFILTER__
#define __IVIDEOMIXINGFILTER__

#ifdef __cplusplus
extern "C" {
#endif

	// {3DB9FE5A-A4C6-4098-88B5-0FD0444446BF}
	DEFINE_GUID(IID_IVideoMixingFilter, 
	0x3db9fe5a, 0xa4c6, 0x4098, 0x88, 0xb5, 0xf, 0xd0, 0x44, 0x44, 0x46, 0xbf);



    DECLARE_INTERFACE_(IVideoMixingFilter, IUnknown)
    {
		STDMETHOD(SetBoundingBox) (THIS_
					int x,
					int y,
					int width,
					int height
					) PURE;	
    };

#ifdef __cplusplus
}
#endif

#endif // __IVIDEOMIXINGFILTER__


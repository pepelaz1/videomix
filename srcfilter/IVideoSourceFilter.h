#ifndef __IVIDEOSOURCEFILTER__
#define __IVIDEOSOURCEFILTER__

#ifdef __cplusplus
extern "C" {
#endif

	// {1CE67B34-5A17-4C5F-A97F-7583E920B5B8}
	DEFINE_GUID(IID_IVideoSourceFilter, 
	0x1ce67b34, 0x5a17, 0x4c5f, 0xa9, 0x7f, 0x75, 0x83, 0xe9, 0x20, 0xb5, 0xb8);



    DECLARE_INTERFACE_(IVideoSourceFilter, IUnknown)
    {
		STDMETHOD(SetVideoSize) (THIS_
					SIZE size
					) PURE;	


		STDMETHOD(SetRawData) (THIS_
					BYTE *data
					) PURE;	
    };

#ifdef __cplusplus
}
#endif

#endif // __IVIDEOSOURCEFILTER__


#ifndef __7ZIP_IN_STREAM_WRAPPER_H__
#define __7ZIP_IN_STREAM_WRAPPER_H__
#include "CPP/Common/MyWindows.h"
#include "CPP/Common/MyCom.h"

class C7ZipInStreamWrapper:
    public IInStream,
    public IStreamGetSize,
    public CMyUnknownImp
{
public:
    C7ZipInStreamWrapper(C7ZipInStream * pInStream);
    virtual ~C7ZipInStreamWrapper() {}

public:
    MY_UNKNOWN_IMP2(IInStream, IStreamGetSize)

    STDMETHOD(Read)(void *data, UInt32 size, UInt32 *processedSize) throw();
    STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64 *newPosition) throw();

    STDMETHOD(GetSize)(UInt64 *size) throw();

private:
    C7ZipInStream * m_pInStream;
};

#endif //__7ZIP_IN_STREAM_WRAPPER_H__

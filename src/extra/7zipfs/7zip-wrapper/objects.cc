//
// Created by igor on 3/31/24.
//

#include "objects.hh"
#include "com.hh"
#include "CPP/7zip/IStream.h"
#include "Common/MyCom.h"

namespace z7 {
	C7ZipObjectPtrArray::C7ZipObjectPtrArray (bool auto_release)
		: m_bAutoRelease (auto_release) {
	}

	C7ZipObjectPtrArray::~C7ZipObjectPtrArray () {
		clear ();
	}

	void C7ZipObjectPtrArray::clear () {
		if (m_bAutoRelease) {
			for (auto it = begin (); it != end (); it++) {
				delete *it;
			}
		}

		std::vector<C7ZipObject*>::clear ();
	}

	// ==========================================================================
	class C7ZipOutStreamWrap :
		public IOutStream,
		public CMyUnknownImp {
	 public:
		explicit C7ZipOutStreamWrap (C7ZipOutStream* pOutStream)
			: m_pOutStream (pOutStream) {}

		virtual ~C7ZipOutStreamWrap () = default;

	 public:
		MY_UNKNOWN_IMP1(IOutStream)

		STDMETHOD(Seek) (Int64 offset, UInt32 seekOrigin, UInt64* newPosition) throw () {
			return m_pOutStream->Seek (offset, seekOrigin, newPosition);
		}

#if MY_VER_MAJOR > 9 || (MY_VER_MAJOR == 9 && MY_VER_MINOR >= 20)

		STDMETHOD(SetSize) (UInt64 newSize) throw ()
#else
		STDMETHOD(SetSize)(Int64 newSize)  throw()
#endif
		{
			return m_pOutStream->SetSize (newSize);
		}

		STDMETHOD(Write) (const void* data, UInt32 size, UInt32* processedSize) throw () {
			return m_pOutStream->Write (data, size, processedSize);
		}

	 private:
		C7ZipOutStream* m_pOutStream;
	};
}
//
// Created by igor on 3/31/24.
//

#ifndef VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_OBJECTS_HH_
#define VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_OBJECTS_HH_

#include <string>
#include <vector>
#include <cstdint>

//#ifndef CLASS_E_CLASSNOTAVAILABLE
//#define CLASS_E_CLASSNOTAVAILABLE (0x80040111L)
//#endif

#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2

namespace z7 {

	typedef std::vector<std::wstring> WStringArray;

	class C7ZipObject {
	 public:
		C7ZipObject () = default;

		virtual ~C7ZipObject () = default;
	};

	class C7ZipObjectPtrArray : public std::vector<C7ZipObject*> {
	 public:
		explicit C7ZipObjectPtrArray (bool auto_release = true);
		virtual ~C7ZipObjectPtrArray ();

	 public:
		void clear ();

	 private:
		bool m_bAutoRelease;
	};

	enum PropertyIndexEnum {
		PROP_INDEX_BEGIN,

		kpidPackSize = PROP_INDEX_BEGIN, //(Packed Size)
		kpidAttrib, //(Attributes)
		kpidCTime, //(Created)
		kpidATime, //(Accessed)
		kpidMTime, //(Modified)
		kpidSolid, //(Solid)
		kpidEncrypted, //(Encrypted)
		kpidUser, //(User)
		kpidGroup, //(Group)
		kpidComment, //(Comment)
		kpidPhySize, //(Physical Size)
		kpidHeadersSize, //(Headers Size)
		kpidChecksum, //(Checksum)
		kpidCharacts, //(Characteristics)
		kpidCreatorApp, //(Creator Application)
		kpidTotalSize, //(Total Size)
		kpidFreeSpace, //(Free Space)
		kpidClusterSize, //(Cluster Size)
		kpidVolumeName, //(Label)
		kpidPath, //(FullPath)
		kpidIsDir, //(IsDir)
		kpidSize, //(Uncompressed Size)

		PROP_INDEX_END
	};

	enum ErrorCodeEnum {
		LIB7ZIP_ErrorCode_Begin,

		LIB7ZIP_NO_ERROR = LIB7ZIP_ErrorCode_Begin,
		LIB7ZIP_UNKNOWN_ERROR,
		LIB7ZIP_NOT_INITIALIZE,
		LIB7ZIP_NEED_PASSWORD,
		LIB7ZIP_NOT_SUPPORTED_ARCHIVE,

		LIB7ZIP_ErrorCode_End
	};

	class C7ZipArchiveItem : public virtual C7ZipObject {
	 public:
		C7ZipArchiveItem () = default;
	 public:
		[[nodiscard]] virtual std::wstring GetFullPath () const = 0;
		[[nodiscard]] virtual uint64_t GetSize () const = 0;
		[[nodiscard]] virtual bool IsDir () const = 0;
		[[nodiscard]] virtual bool IsEncrypted () const = 0;

		[[nodiscard]] virtual unsigned int GetArchiveIndex () const = 0;

		virtual bool GetUInt64Property (PropertyIndexEnum propertyIndex, uint64_t& val) const = 0;
		virtual bool GetBoolProperty (PropertyIndexEnum propertyIndex, bool& val) const = 0;
		virtual bool GetStringProperty (PropertyIndexEnum propertyIndex, std::wstring& val) const = 0;
		virtual bool GetFileTimeProperty (PropertyIndexEnum propertyIndex, uint64_t& val) const = 0;
		[[nodiscard]] virtual std::wstring GetArchiveItemPassword () const = 0;
		virtual void SetArchiveItemPassword (const std::wstring& password) = 0;
		[[nodiscard]] virtual bool IsPasswordSet () const = 0;
	};

	class C7ZipInStream {
	 public:
		[[nodiscard]] virtual std::wstring GetExt () const = 0;
		virtual int Read (void* data, unsigned int size, unsigned int* processedSize) = 0;
		virtual int Seek (int64_t offset, unsigned int seekOrigin, uint64_t* newPosition) = 0;
		virtual int GetSize (uint64_t* size) = 0;
	};

	class C7ZipMultiVolumes {
	 public:
		virtual std::wstring GetFirstVolumeName () = 0;
		virtual bool MoveToVolume (const std::wstring& volumeName) = 0;
		virtual uint64_t GetCurrentVolumeSize () = 0;
		virtual C7ZipInStream* OpenCurrentVolumeStream () = 0;
	};

	class C7ZipOutStream {
	 public:
		virtual int Write (const void* data, unsigned int size, unsigned int* processedSize) = 0;
		virtual int Seek (int64_t offset, unsigned int seekOrigin, uint64_t* newPosition) = 0;
		virtual int SetSize (uint64_t size) = 0;
	};

	class C7ZipArchive : public virtual C7ZipObject {
	 public:
		C7ZipArchive ();
		virtual ~C7ZipArchive ();

	 public:
		virtual bool GetItemCount (unsigned int* pNumItems) = 0;
		virtual bool GetItemInfo (unsigned int index, C7ZipArchiveItem** ppArchiveItem) = 0;
		virtual bool Extract (unsigned int index, C7ZipOutStream* pOutStream) = 0;
		virtual bool Extract (unsigned int index, C7ZipOutStream* pOutStream, const std::wstring& pwd) = 0;
		virtual bool Extract (const C7ZipArchiveItem* pArchiveItem, C7ZipOutStream* pOutStream) = 0;
		[[nodiscard]] virtual std::wstring GetArchivePassword () const = 0;
		virtual void SetArchivePassword (const std::wstring& password) = 0;
		[[nodiscard]] virtual bool IsPasswordSet () const = 0;

		virtual void Close () = 0;

		virtual bool GetUInt64Property (PropertyIndexEnum propertyIndex, uint64_t& val) const = 0;
		virtual bool GetBoolProperty (PropertyIndexEnum propertyIndex,
									  bool& val) const = 0;
		virtual bool GetStringProperty (PropertyIndexEnum propertyIndex,
										std::wstring& val) const = 0;
		virtual bool GetFileTimeProperty (PropertyIndexEnum propertyIndex,
										  uint64_t& val) const = 0;
	};

	class C7ZipLibrary {
	 public:
		C7ZipLibrary ();
		~C7ZipLibrary ();

	 private:
		bool m_bInitialized;
		ErrorCodeEnum m_LastError;

		C7ZipObjectPtrArray m_InternalObjectsArray;

	 public:
		bool Initialize ();
		void Deinitialize ();

		bool GetSupportedExts (WStringArray& exts);

		bool OpenArchive (C7ZipInStream* pInStream,
						  C7ZipArchive** ppArchive,
						  bool fDetectFileTypeBySignature = false);
		bool OpenArchive (C7ZipInStream* pInStream,
						  C7ZipArchive** ppArchive,
						  const std::wstring& pwd,
						  bool fDetectFileTypeBySignature = false);
		bool OpenMultiVolumeArchive (C7ZipMultiVolumes* pVolumes,
									 C7ZipArchive** ppArchive,
									 bool fDetectFileTypeBySignature = false);
		bool OpenMultiVolumeArchive (C7ZipMultiVolumes* pVolumes,
									 C7ZipArchive** ppArchive,
									 const std::wstring& pwd,
									 bool fDetectFileTypeBySignature = false);

		ErrorCodeEnum GetLastError () const { return m_LastError; }

		const C7ZipObjectPtrArray& GetInternalObjectsArray () { return m_InternalObjectsArray; }

		bool IsInitialized () const { return m_bInitialized; }
	};

}

#endif //VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_OBJECTS_HH_

//
// Created by igor on 3/31/24.
//

#include "format_info.hh"
#include "com.hh"
#include "C/7zVersion.h"
#include "CPP/7zip/Archive/IArchive.h"

#include <bsw/strings/wchar.hh>

#if MY_VER_MAJOR >= 15
#include "CPP/Common/MyBuffer.h"
#else
#include "CPP/Common/Buffer.h"
#endif

#if MY_VER_MAJOR >= 15
#define NArchiveEnumPrefix NArchive::NHandlerPropID
#else
#define NArchiveEnumPrefix NArchive
#endif

STDAPI GetHandlerProperty2(UInt32 formatIndex, PROPID propID, PROPVARIANT *value);
STDAPI GetHandlerProperty(PROPID propID, PROPVARIANT *value);
STDAPI GetNumberOfFormats(UINT32 *numFormats);

#if MY_VER_MAJOR >= 15
static bool ParseSignatures(const Byte *data, unsigned size, CObjectVector<CByteBuffer> &signatures)
{
  signatures.Clear();
  while (size > 0)
  {
    unsigned len = *data++;
    size--;
    if (len > size)
      return false;
    signatures.AddNew().CopyFrom(data, len);
    data += len;
    size -= len;
  }
  return true;
}
#endif

namespace z7 {
	std::vector<format_info> get_formats() {
		std::vector<format_info> formats;
		UInt32 numFormats = 1;
		if (GetNumberOfFormats(&numFormats) != S_OK) {
			return {};
		}
		for(UInt32 i = 0; i < numFormats; i++)
		{
			std::wstring name;
			bool updateEnabled = false;
			bool keepName = false;
			GUID classID;
			std::wstring ext, addExt;

			if (ReadStringProp(GetHandlerProperty, GetHandlerProperty2, i, NArchiveEnumPrefix::kName, name) != S_OK) {
				continue;
			}

			NWindows::NCOM::CPropVariant prop;
			if (ReadProp(GetHandlerProperty, GetHandlerProperty2, i, NArchiveEnumPrefix::kClassID, prop) != S_OK) {
				continue;
			}
			if (prop.vt != VT_BSTR) {
				continue;
			}

			classID = *(const GUID *)prop.bstrVal;

			if (ReadStringProp(GetHandlerProperty, GetHandlerProperty2,
							   i, NArchiveEnumPrefix::kExtension, ext) != S_OK) {
				continue;
			}

			if (ReadStringProp(GetHandlerProperty, GetHandlerProperty2,
							   i, NArchiveEnumPrefix::kAddExtension, addExt) != S_OK) {
				continue;
			}

			ReadBoolProp(GetHandlerProperty, GetHandlerProperty2, i, NArchiveEnumPrefix::kUpdate, updateEnabled);

			if (updateEnabled)
			{
				ReadBoolProp(GetHandlerProperty, GetHandlerProperty2, i, NArchiveEnumPrefix::kKeepName, keepName);
			}

			std::vector<std::wstring> Exts;
			std::vector<std::wstring> AddExts;

			CByteBuffer m_StartSignature;
			CByteBuffer m_FinishSignature;
			UInt32 SignatureOffset;
			CObjectVector<CByteBuffer> Signatures;

#if MY_VER_MAJOR >= 15
			if (ReadProp(GetHandlerProperty,
						 GetHandlerProperty2, i, NArchiveEnumPrefix::kSignature, prop) == S_OK) {
#else
				if (ReadProp(GetHandlerProperty, GetHandlerProperty2, i, NArchiveEnumPrefix::kStartSignature, prop) == S_OK) {
#endif
				if (prop.vt == VT_BSTR) {
					UINT len = ::SysStringByteLen(prop.bstrVal);
#if MY_VER_MAJOR >= 15
					m_StartSignature.CopyFrom((const Byte *)prop.bstrVal, len);
#else
					m_StartSignature.SetCapacity(len);
            		memmove(m_StartSignature, prop.bstrVal, len);
#endif
#if MY_VER_MAJOR >= 15
					if (len > 0)
						Signatures.Add(m_StartSignature);
#endif
				}
			}

#if MY_VER_MAJOR >= 15
			if (ReadProp(GetHandlerProperty, GetHandlerProperty2, i, NArchiveEnumPrefix::kMultiSignature, prop) == S_OK) {
#else
				if (ReadProp(pFunctions->v.GetHandlerProperty,
                     pFunctions->v.GetHandlerProperty2, i, NArchiveEnumPrefix::kFinishSignature, prop) == S_OK) {
#endif
				if (prop.vt == VT_BSTR) {
					UINT len = ::SysStringByteLen(prop.bstrVal);
#if MY_VER_MAJOR >= 15
					m_FinishSignature.CopyFrom((const Byte *)prop.bstrVal, len);
					ParseSignatures(m_FinishSignature,
									(unsigned)m_FinishSignature.Size(),
									Signatures);
#else
					m_FinishSignature.SetCapacity(len);
            		memmove(m_FinishSignature, prop.bstrVal, len);
#endif
				}
			}

#if MY_VER_MAJOR >= 15
			if (ReadProp(GetHandlerProperty,
						 GetHandlerProperty2, i,
						 NArchiveEnumPrefix::kSignatureOffset, prop) == S_OK) {
				if (prop.vt == VT_UI4) {
					SignatureOffset = prop.ulVal;
				}
				else {
					SignatureOffset = 0;
				}
			}
			else {
				SignatureOffset = 0;
			}
#endif
			format_info fi;
			fi.name = bsw::wstring_to_utf8 (name);
			fi.keep_name = keepName;
			fi.class_id = classID;
			fi.update_enabled = updateEnabled;

			SplitString(ext, Exts);
			for (const auto& s : Exts) {
				fi.exts.push_back (bsw::wstring_to_utf8 (s));
			}
			SplitString(addExt, AddExts);
			for (const auto& s : AddExts) {
				fi.add_exts.push_back (bsw::wstring_to_utf8 (s));
			}
			fi.format_index = i;
			fi.signature_offset = SignatureOffset;
			fi.start_signature.reserve (m_StartSignature.Size());
			unsigned char* ptr = m_StartSignature;
			memcpy(fi.start_signature.data(), ptr, m_StartSignature.Size());
			fi.finish_signature.reserve (m_FinishSignature.Size());
			ptr = m_FinishSignature;
			memcpy(fi.finish_signature.data(), ptr, m_FinishSignature.Size());
			for (unsigned j=0; j<Signatures.Size(); j++) {
				auto& cb = Signatures[j];
				std::vector<char> v;
				v.resize (cb.Size());
				ptr = cb;
				memcpy(v.data(), ptr, cb.Size());
				fi.signatures.push_back (v);
			}
			formats.push_back(fi);
		}
		return formats;
	}
}
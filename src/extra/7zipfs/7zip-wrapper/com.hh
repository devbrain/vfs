//
// Created by igor on 3/31/24.
//

#ifndef VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_COM_HH_
#define VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_COM_HH_

#include "7zip-wrapper/fake_windows.hh"
#include <string>
#include <vector>



namespace z7 {
	typedef HRESULT (*GetMethodPropertyFunc)(UInt32 index, PROPID propID, PROPVARIANT *value);
	typedef HRESULT (*GetNumberOfMethodsFunc)(UInt32 *numMethods);
	typedef HRESULT (*GetNumberOfFormatsFunc)(UInt32 *numFormats);
	typedef HRESULT (*GetHandlerPropertyFunc)(PROPID propID, PROPVARIANT *value);
	typedef HRESULT (*GetHandlerPropertyFunc2)(UInt32 index, PROPID propID, PROPVARIANT *value);
	typedef HRESULT (*CreateObjectFunc)(const GUID *clsID, const GUID *iid, void **outObject);
	typedef HRESULT (*SetLargePageModeFunc)();

	HRESULT ReadProp(
		GetHandlerPropertyFunc getProp,
		GetHandlerPropertyFunc2 getProp2,
		UInt32 index, PROPID propID, NWindows::NCOM::CPropVariant &prop);


	HRESULT ReadBoolProp(
		GetHandlerPropertyFunc getProp,
		GetHandlerPropertyFunc2 getProp2,
		UInt32 index, PROPID propID, bool &res);


	HRESULT ReadStringProp(
		GetHandlerPropertyFunc getProp,
		GetHandlerPropertyFunc2 getProp2,
		UInt32 index, PROPID propID, std::wstring &res);


	void SplitString(const std::wstring &srcString, std::vector<std::wstring> &destStrings);


	HRESULT GetCoderClass(GetMethodPropertyFunc getMethodProperty, UInt32 index,
						  PROPID propId, GUID & clsId, bool &isAssigned);


	HRESULT GetMethodPropertyString(GetMethodPropertyFunc getMethodProperty, UInt32 index,
									PROPID propId, std::wstring & val);


	HRESULT GetMethodPropertyGUID(GetMethodPropertyFunc getMethodProperty, UInt32 index,
								  PROPID propId, GUID & val);
}

#endif //VFS_SRC_EXTRA_7ZIPFS_7ZIP_WRAPPER_COM_HH_

//
// Created by igor on 3/31/24.
//

#include "com.hh"

namespace z7 {
	HRESULT ReadProp(
		GetHandlerPropertyFunc getProp,
		GetHandlerPropertyFunc2 getProp2,
		UInt32 index, PROPID propID, NWindows::NCOM::CPropVariant &prop)
	{
		if (getProp2)
			return getProp2(index, propID, &prop);;
		return getProp(propID, &prop);
	}

	HRESULT ReadBoolProp(
		GetHandlerPropertyFunc getProp,
		GetHandlerPropertyFunc2 getProp2,
		UInt32 index, PROPID propID, bool &res)
	{
		NWindows::NCOM::CPropVariant prop;

		RINOK(ReadProp(getProp, getProp2, index, propID, prop));
		if (prop.vt == VT_BOOL)
			res = VARIANT_BOOLToBool(prop.boolVal);
		else if (prop.vt != VT_EMPTY)
			return E_FAIL;
		return S_OK;
	}

	HRESULT ReadStringProp(
		GetHandlerPropertyFunc getProp,
		GetHandlerPropertyFunc2 getProp2,
		UInt32 index, PROPID propID, std::wstring &res)
	{
		NWindows::NCOM::CPropVariant prop;

		RINOK(ReadProp(getProp, getProp2, index, propID, prop));
		if (prop.vt == VT_BSTR)
			res = prop.bstrVal;
		else if (prop.vt != VT_EMPTY)
			return E_FAIL;
		return S_OK;
	}

	void SplitString(const std::wstring &srcString, std::vector<std::wstring> &destStrings)
	{
		destStrings.clear();
		std::wstring s;
		size_t len = srcString.length();
		if (len == 0)
			return;
		for (size_t i = 0; i < len; i++) {
			wchar_t c = srcString[i];
			if (c == L' ') {
				if (!s.empty())	{
					destStrings.push_back(s);
					s.clear();
				}
			}
			else
				s += c;
		}
		if (!s.empty())
			destStrings.push_back(s);
	}

	HRESULT GetCoderClass(GetMethodPropertyFunc getMethodProperty, UInt32 index,
						  PROPID propId, GUID & clsId, bool &isAssigned)
	{
		NWindows::NCOM::CPropVariant prop;
		isAssigned = false;
		RINOK(getMethodProperty(index, propId, &prop));
		if (prop.vt == VT_BSTR)	{
			isAssigned = true;
			clsId = *(const GUID *)prop.bstrVal;
		}
		else if (prop.vt != VT_EMPTY)
			return E_FAIL;
		return S_OK;
	}

	HRESULT GetMethodPropertyString(GetMethodPropertyFunc getMethodProperty, UInt32 index,
									PROPID propId, std::wstring & val)
	{
		NWindows::NCOM::CPropVariant prop;
		RINOK(getMethodProperty(index, propId, &prop));
		if (prop.vt == VT_BSTR)	{
			val = prop.bstrVal;
		}
		else if (prop.vt != VT_EMPTY)
			return E_FAIL;
		return S_OK;
	}

	HRESULT GetMethodPropertyGUID(GetMethodPropertyFunc getMethodProperty, UInt32 index,
								  PROPID propId, GUID & val)
	{
		NWindows::NCOM::CPropVariant prop;
		RINOK(getMethodProperty(index, propId, &prop));
		if (prop.vt == VT_BSTR)	{
			val = *(const GUID *)prop.bstrVal;
		}
		else if (prop.vt != VT_EMPTY)
			return E_FAIL;
		return S_OK;
	}
}
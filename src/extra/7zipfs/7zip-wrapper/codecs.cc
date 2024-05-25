//
// Created by igor on 3/31/24.
//

#include "codecs.hh"
#include "com.hh"
#include "CPP/7zip/ICoder.h"
#include <bsw/strings/wchar.hh>
#include <utility>

STDAPI GetMethodProperty(UInt32 codecIndex, PROPID propID, PROPVARIANT *value);
STDAPI GetNumberOfMethods(UInt32 *numMethods);

namespace z7 {
	std::vector<codec> get_codecs () {
		std::vector<codec> out;
		UInt32 numMethods = 0;
		GetNumberOfMethods (&numMethods);
		for (UInt32 i = 0; i < numMethods; i++) {
			std::wstring name;

			if (GetMethodPropertyString (GetMethodProperty, i,
										 NMethodPropID::kName, name) != S_OK) {
				continue;
			}


			GUID encoder, decoder;
			bool encoderIsAssigned, decoderIsAssigned;

			if (GetCoderClass (GetMethodProperty, i,
							   NMethodPropID::kEncoder, encoder, encoderIsAssigned) != S_OK) {
				continue;
			}
			if (GetCoderClass (GetMethodProperty, i,
							   NMethodPropID::kDecoder, decoder, decoderIsAssigned) != S_OK) {
				continue;
			}
			out.emplace_back (bsw::wstring_to_utf8 (name),
							  encoderIsAssigned ? std::make_optional<GUID>(encoder) : std::nullopt,
							  decoderIsAssigned ? std::make_optional<GUID>(decoder) : std::nullopt,
							  i);
		}
		return out;
	}

	codec::codec (std::string name,
				  const std::optional<GUID>& encoder,
				  const std::optional<GUID>& decoder,
				  uint32_t index)
		: name (std::move(name)), encoder (encoder), decoder (decoder), index (index) {}
}
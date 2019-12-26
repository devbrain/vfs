//
// Created by igor on 22/12/2019.
//

#include "api/hasher.hh"
#include "api/detail/spooky.hh"

namespace vfs
{
	namespace core
	{
		hash_t hash(const char* data, std::size_t sz)
		{
			return hash(data, sz, {0,0});
		}
		// ---------------------------------------------------------------------------------------
		hash_t hash(const char* data, std::size_t sz, const hash_t& seed)
		{
			SpookyHash h;
			h.Init(std::get<0>(seed), std::get<1>(seed));
			h.Update(data, sz);
			std::uint64_t h1, h2;
			h.Final(&h1, &h2);

			return {h1, h2};
		}
		// ---------------------------------------------------------------------------------------
		hash_t hash(const std::string& data)
		{
			return hash(data.c_str(), data.length());
		}
		// ---------------------------------------------------------------------------------------
		hash_t hash(const std::string& data, const hash_t& seed)
		{
			return hash(data.c_str(), data.length(), seed);
		}
		// ---------------------------------------------------------------------------------------
	}
}
#include "api/detail/stats_converter.hh"

namespace vfs {
	stats convert (const core::stats& st) {
		stats res;
		res.size = st.size;
		switch (st.type) {
			case VFS_INODE_DIRECTORY: res.type = stats::eDIRECTORY;
				break;
			case VFS_INODE_REGULAR: res.type = stats::eFILE;
				break;
			default: res.type = stats::eLINK;
		}
		if (st.attr1.key[0] != 0) {
			res.attribs.insert (std::make_pair (st.attr1.key, st.attr1.value));
		}
		if (st.attr2.key[0] != 0) {
			res.attribs.insert (std::make_pair (st.attr2.key, st.attr2.value));
		}
		for (std::size_t i = 0; i < st.num_of_additional_attributes; i++) {
			res.attribs.insert (std::make_pair (st.additional_attributes[i].key, st.additional_attributes[i].value));
		}
		return res;
	}
}
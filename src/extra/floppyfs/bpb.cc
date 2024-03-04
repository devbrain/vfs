//
// Created by igor on 3/4/24.
//

#include "bpb.hh"

namespace vfs::extra {

	bsw::io::binary_reader& operator>> (bsw::io::binary_reader& is, bpb& out) {
		is.read_raw (out.jmp, sizeof (out.jmp));
		is.read_raw (out.oem, sizeof (out.oem));
		is >> out.bytes_per_sector >> out.sectors_per_cluster
		   >> out.reserved_sectors >> out.number_of_fats
		   >> out.root_dir_entries >> out.total_sectors
		   >> out.media_descriptor >> out.sectors_per_fat
		   >> out.heads >> out.number_of_hidden_sectors
		   >> out.large_sector_count;
		return is;
	}
}
//
// Created by igor on 3/12/24.
//

#include <bsw/byte_order.hh>
#include "fat.hh"


namespace vfs::extra {

	struct constants {
		explicit constants(bpb::type_t type) {
			switch (type) {
				case bpb::FAT12:
					must_not_be_used = 0xFF6;
					bad = 0xFF7;
					should_not_be_used_low = 0xFF8;
					should_not_be_used_hi  = 0xFFE;
					final_cluster = 0xFFF;
					break;
				case bpb::FAT16:
					must_not_be_used = 0xFFF6;
					bad = 0xFFF7;
					should_not_be_used_low = 0xFFF8;
					should_not_be_used_hi  = 0xFFFE;
					final_cluster = 0xFFFF;
					break;
				case bpb::FAT32:
					must_not_be_used = 0xFFFFFFF6;
					bad = 0xFFFFFFF7;
					should_not_be_used_low = 0xFFFFFFF8;
					should_not_be_used_hi  = 0xFFFFFFFE;
					final_cluster = 0xFFFFFFFF;
					break;
			}
		}
		uint32_t must_not_be_used{};
		uint32_t bad{};
		uint32_t should_not_be_used_low{};
		uint32_t should_not_be_used_hi{};
		uint32_t final_cluster{};
	};

	fat::fat (bpb::type_t type, const disk_entry& location, std::istream& is)
	: m_type(type),
	  m_location (location),
	  m_fat(location.size),
	  m_num_of_clusters{} {
		auto pos = is.tellg();
		is.seekg (location.offset, std::ios::beg);
		is.read (m_fat.data(), location.size);
		is.seekg (pos);
		switch (type) {
			case bpb::FAT12:
				m_num_of_clusters = (m_location.size * 8)/12;
				break;
			case bpb::FAT16:
				m_num_of_clusters = m_location.size / sizeof (uint16_t);
				break;
			case bpb::FAT32:
				m_num_of_clusters = m_location.size / sizeof (uint32_t);
				break;
		}
	}

	uint32_t fat::get(uint32_t cluster) const {
		const auto* image_buf = m_fat.data();
		uint32_t value;
		switch (m_type) {
			case bpb::FAT12:
			{
				std::size_t offset = (3 * (cluster/2));

				uint8_t b1, b2;
				switch(cluster % 2) {
					case 0:
						b1 = *(image_buf + offset);
						b2 = *(image_buf + offset + 1);
						value = ((0x0f & b2) << 8) | b1;
						break;
					case 1:
						b1 = *(image_buf + offset + 1);
						b2 = *(image_buf + offset + 2);
						value = b2 << 4 | ((0xf0 & b1) >> 4);
						break;
				}
				break;
			}
			case bpb::FAT16:
			{
				union {
					const char* memory;
					uint16_t    value;
				} x{};
				std::size_t offset = cluster * sizeof (x.value);
				x.memory = image_buf + offset;
				value = bsw::byte_order::from_little_endian (x.value);
			}
			break;
			case bpb::FAT32:
			{
				union {
					const char* memory;
					uint32_t    value;
				} x{};
				std::size_t offset = cluster * sizeof (x.value);
				x.memory = image_buf + offset;
				value = bsw::byte_order::from_little_endian (x.value);
			}
		}
		return value;
	}

	void fat::set(uint32_t cluster, uint32_t value) {
		auto* image_buf = (uint8_t*)m_fat.data();
		switch (m_type) {
			case bpb::FAT12:
			{
				std::size_t offset = (3 * (cluster/2));
				uint8_t* p1, *p2;
				auto v = (uint16_t )value;
				switch(cluster % 2) {
					case 0:
						p1 = image_buf + offset;
						p2 = image_buf + offset + 1;
						*p1 = (uint8_t)(0xff & v);
						*p2 = (uint8_t)((0xf0 & (*p2)) | (0x0f & (v >> 8)));
						break;
					case 1:
						p1 = image_buf + offset + 1;
						p2 = image_buf + offset + 2;
						*p1 = (uint8_t)((0x0f & (*p1)) | ((0x0f & v) << 4));
						*p2 = (uint8_t)(0xff & (v >> 4));
						break;
				}
			}
			break;
			case bpb::FAT16:
			{
				union {
					decltype (image_buf) memory;
					uint16_t    value;
				} x{};
				std::size_t offset = cluster * sizeof (x.value);
				x.memory = image_buf + offset;
				x.value = bsw::byte_order::to_little_endian ((uint16_t )value);
			}
			break;
			case bpb::FAT32:
			{
				union {
					decltype (image_buf) memory;
					uint32_t    value;
				} x{};
				std::size_t offset = cluster * sizeof (x.value);
				x.memory = image_buf + offset;
				x.value =  bsw::byte_order::to_little_endian (x.value);
			}
		}
	}

	void fat::set(uint32_t cluster, cluster_type_t t) {
		static constants c(m_type);
		switch (t) {
			case FREE:
				set(cluster, 0);
				break;
			case MUST_NOT_BE_USED:
				set(cluster, c.must_not_be_used);
				break;
			case DEFECTIVE:
				set(cluster, c.bad);
				break;
			case SHOULD_NOT_BE_USED:
				set(cluster, c.should_not_be_used_low);
				break;
			case FINAL:
				set (cluster, c.final_cluster);
				break;
		}
	}

	uint32_t fat::size () const {
		return m_num_of_clusters;
	}

	std::optional<fat::cluster_type_t> fat::get_cluster_type (uint32_t value) const {
		if (value == 0) {
			return FREE;
		}

		static constants c(m_type);
		if (value >= m_num_of_clusters && value < c.must_not_be_used) {
			return MUST_NOT_BE_USED;
		}
		if (value == c.bad) {
			return DEFECTIVE;
		}
		if (value >= c.should_not_be_used_low && value < c.should_not_be_used_hi) {
			return SHOULD_NOT_BE_USED;
		}
		if (value == c.final_cluster) {
			return FINAL;
		}
		return std::nullopt;
	}
}
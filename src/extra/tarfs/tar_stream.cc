//
// Created by igor on 3/18/24.
//

#include "tar_stream.hh"
#include <istream>

namespace {
	struct mytar : public mtar_t {
		bool is_input;
	};


	int stream_write(mtar_t *tar, const void *data, unsigned size) {
		auto* obj = reinterpret_cast<mytar*>(tar);
		if (obj->is_input) {
			return MTAR_EWRITEFAIL;
		}
		auto* stream = reinterpret_cast<std::ostream*>(obj->stream);
		size_t before = stream->tellp();
		if (!stream->write((char*)data, size)) {
			return MTAR_EWRITEFAIL;
		}
		size_t res = static_cast<std::size_t>(stream->tellp()) - before;
		return (res == size) ? MTAR_ESUCCESS : MTAR_EWRITEFAIL;
	}

	int stream_read(mtar_t *tar, void *data, unsigned size) {
		auto* obj = reinterpret_cast<mytar*>(tar);
		if (!obj->is_input) {
			return MTAR_EREADFAIL;
		}
		auto* stream = reinterpret_cast<std::istream*>(obj->stream);
		size_t before = stream->tellg();
		if (!stream->read ((char*)data, size)) {
			return MTAR_EREADFAIL;
		}
		size_t res = static_cast<std::size_t>(stream->tellg()) - before;
		return (res == size) ? MTAR_ESUCCESS : MTAR_EREADFAIL;
	}

	int stream_seek(mtar_t *tar, unsigned offset) {
		auto* obj = reinterpret_cast<mytar*>(tar);
		if (!obj->is_input) {
			auto* stream = reinterpret_cast<std::ostream*>(obj->stream);
			if (!stream->seekp (offset, std::ios::beg)) {
				return MTAR_ESEEKFAIL;
			}
		} else {
			auto* stream = reinterpret_cast<std::istream*>(obj->stream);
			if (!stream->seekg (offset, std::ios::beg)) {
				return MTAR_ESEEKFAIL;
			}
		}

		return MTAR_ESUCCESS;
	}

	int stream_close(mtar_t *tar) {
		auto* obj = reinterpret_cast<mytar*>(tar);
		if (!obj->is_input) {
			auto* stream = reinterpret_cast<std::ostream*>(obj->stream);
			stream->flush();
		}
		return MTAR_ESUCCESS;
	}
}

namespace vfs::extra {


	std::unique_ptr<mtar_t> create_mtar_from_stream (std::istream* is) {
		auto out = std::make_unique<mytar>();
		out->stream = is;
		out->is_input = true;
		out->read = stream_read;
		out->write = stream_write;
		out->close = stream_close;
		out->seek = stream_seek;
		return out;
	}

	std::unique_ptr<mtar_t> create_mtar_from_stream (std::ostream* os) {
		auto out = std::make_unique<mytar>();
		out->stream = os;
		out->is_input = false;
		out->read = stream_read;
		out->write = stream_write;
		out->close = stream_close;
		out->seek = stream_seek;
		return out;
	}
}
//
// Created by igor on 3/19/24.
//

#include "lha_archive.hh"
#include "input_stream.hh"

namespace vfs::extra {

	lha_archive::lha_archive (std::istream& is)
	: m_stream (is),
	  m_rdr_ops (create_lha_istream()),
	  m_lha_stream (lha_input_stream_new (&m_rdr_ops, &m_stream)),
	  m_reader (lha_reader_new (m_lha_stream)) {
	}

	lha_archive::~lha_archive () {
		lha_reader_free (m_reader);
		lha_input_stream_free (m_lha_stream);
	}

}
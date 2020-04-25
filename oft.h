#pragma once

#include <array>
#include "fs_config.h"

namespace filesystem::components {
	using namespace config;
	struct OFTEntry {
		OFTEntry();
		char read_write_buffer[BLOCK_SIZE];
		int fpos, fd_index;
		bool block_read, block_modified;
	};

	class OFT {
	public:
		OFTEntry* findFile(int fd_index);
		OFTEntry* getFile(int oft_index);
	private:
		std::array<OFTEntry, FD_OPENED_LIMIT> entries_buf;
	};
}
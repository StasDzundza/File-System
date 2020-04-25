#pragma once

#include <array>
#include "fs_config.h"

namespace filesystem::components {
	using namespace config;
	struct OFTEntry {
		char read_write_buffer[BLOCK_SIZE];
		int fpos = 0;
		int fd_index = 0;
	};
	class OFT {
	public:
		OFTEntry* findFile(int fd_index);
		OFTEntry* getFile(int oft_index);
	private:
		std::array<OFTEntry, FD_OPENED_LIMIT> entries_buf;
	};
	/* There is an open file table (OFT) maintained by the file system. This is a fixed length
	array (declared as part of your file system), where each entry has the following form:
	• Read/write buffer
	• Current position
	• File descriptor index */
}
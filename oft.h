#pragma once

#include <array>
#include "file_system_constants.h"

namespace filesystem::components {
	class OFT {
	public:
		struct OFTEntry {
			char read_write_buffer[constants::BLOCK_SIZE];
			int fpos = 0;
			int fd_index = 0;
		};

		OFTEntry* findFile(int fd_index);
		OFTEntry* getFile(int opened_fd_index);
	private:
		std::array<OFTEntry, constants::FD_OPENED_LIMIT> entries_buf;
	};
}
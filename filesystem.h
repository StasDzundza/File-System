#pragma once
#include "oft.h"
#include "IO_system/IOSystem.h"
#include "file_descriptor.h"
#include "directory_entry.h"
#include <bitset>
#include <memory>
#include <utility>

namespace filesystem {
	
	class FileSystem {
	private:
		void initFileSystem();
		std::pair<int, int> rawWriteToDisk(int block_idx, int shift, void* read_from, int bytes);
	public:
		FileSystem();
	private:
		char block_buf[constants::BLOCK_SIZE];
		char memory_buf[constants::MAIN_MEMORY_BUF];

		std::unique_ptr<io::IOSystem> ios;
		std::unique_ptr<components::OFT>oft;
	};
}
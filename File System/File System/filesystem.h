#pragma once
#include "oft.h"
#include "IOSystem.h"
#include "file_descriptor.h"
#include <bitset>
#include <memory>

namespace filesystem {
	class FileSystem {
	public:
		FileSystem();
		void createFile(char filename[constants::MAX_FILENAME_LENGTH]);
		void destroyFile(char filename[constants::MAX_FILENAME_LENGTH]);
	private:
		std::unique_ptr<io::IOSystem>ios;
		std::unique_ptr<components::OFT>oft;
		char main_memory[constants::MAIN_MEMORY_SIZE];

		void initSystemDiskBlocks();
		components::FileDescriptor getDirectoryDescriptor();
		int findFileInDirectory(char filename[constants::MAX_FILENAME_LENGTH]);
		int findFreeDiskBlock(const std::bitset<constants::DISC_BLOCK_SIZE>& bitset);
	};
}
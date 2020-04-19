#pragma once
#include "oft.h"
#include "IOSystem.h"
#include <bitset>
#include <memory>

namespace filesystem {
	class FileSystem {
	public:
		FileSystem();
		void createFile(char filename[constants::MAX_FILENAME_LENGTH]);
	private:
		std::unique_ptr<io::IOSystem>ios;
		std::unique_ptr<components::OFT>oft;
		char main_memory[constants::MAIN_MEMORY_SIZE];

		int findFreeDiskBlock(std::bitset<constants::DISC_BLOCK_SIZE>& bitset);
	};
}
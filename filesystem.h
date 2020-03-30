#pragma once
#include "oft.h"
#include "IOSystem.h"
#include <memory>
namespace filesystem {
	class FileSystem {
	public:
		FileSystem();
		void createFile(const char* filename);
	private:
		std::unique_ptr<io::IOSystem>ios;
		components::OFT _oft;
		char main_memory[constants::MAIN_MEMORY_SIZE];
	};
}
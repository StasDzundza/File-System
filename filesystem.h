#pragma once

#include <memory>
#include <utility>
#include <optional>

#include "oft.h"
#include "IO_system/IOSystem.h"
#include "directory_entry.h"
#include "fs_config.h"
#include "file_descriptor.h"

namespace filesystem {
	using namespace config;
	using namespace components;
	class FileSystem {
	private:
		void initFileSystem();
		std::optional<DirectoryEntry> findFileInDir(char filename[MAX_FILENAME_LENGTH]);

		FileDescriptor getDescriptorByIndex(int fd_index);
		void writeToFile(OFTEntry* entry, void* read_from, int bytes);
	public:
		FileSystem();
		int createFile(char filename[MAX_FILENAME_LENGTH]);
	private:
		io::IOSystem ios;
		OFT oft;
	};
}
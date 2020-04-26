#pragma once

#include <memory>
#include <utility>
#include <optional>

#include "oft.h"
#include "IOSystem/IOSystem.h"
#include "components/directory_entry.h"
#include "fs_config.h"
#include "components/file_descriptor.h"

namespace filesystem {
	using namespace config;
	using namespace components;
	class FileSystem {
	private:
		void _initFileSystem();

		FileDescriptor _getDescriptorByIndex(int fd_index);
		int _readFromFile(OFTEntry* entry, void* write_ptr, int bytes);
		int _writeToFile(OFTEntry* entry, void* read_ptr, int bytes);
		int _reserveBytesForFile(FileDescriptor* fd, int bytes);
		std::pair<DirectoryEntry, int> _findFileInDirectory(char filename[MAX_FILENAME_LENGTH]);
	public:
		FileSystem();
		int createFile(char filename[MAX_FILENAME_LENGTH]);
		int destroyFile(char filename[MAX_FILENAME_LENGTH]);
		int read(int fd_index, void* main_mem_ptr, int bytes);
		int write(int fd_index, void* main_mem_ptr, int bytes);
		int lseek(int fd_index, int pos);
	private:
		io::IOSystem ios;
		OFT oft;
	};
}
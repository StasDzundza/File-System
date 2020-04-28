#pragma once

#include "oft.h"
#include "IOSystem/LBASystem.h"
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
	public:
		FileSystem();

		int read(int fd_index, void* main_mem_ptr, int bytes);
		int write(int fd_index, void* main_mem_ptr, int bytes);
	private:
		io::LBASystem ios;
		OFT oft;
	};
}
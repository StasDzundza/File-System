#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "oft.h"
#include "IOSystem/LBASystem.h"
#include "components/file_descriptor.h"
#include "components/directory_entry.h"

namespace filesystem {
	using namespace config;
	using namespace components;
	class FileSystem {
	private:
		void _initFileSystem();
		void _restoreFileSystem();

		FileDescriptor _getDescriptorByIndex(int fd_index);
		int _readFromFile(OFTEntry* entry, void* write_ptr, int bytes);
		int _writeToFile(OFTEntry* entry, void* read_ptr, int bytes);
		int _reserveBytesForFile(FileDescriptor* fd, int bytes);
		std::pair<DirectoryEntry, int> _findFileInDirectory(char filename[MAX_FILENAME_LENGTH]);
		bool _fileExists(const char* filename);
	public:
		FileSystem();
		int createFile(char filename[MAX_FILENAME_LENGTH]);
		int destroyFile(char filename[MAX_FILENAME_LENGTH]);
		int read(int fd_index, void* main_mem_ptr, int bytes);
		int write(int fd_index, void* main_mem_ptr, int bytes);
		int lseek(int fd_index, int pos);
		int open(char filename[MAX_FILENAME_LENGTH]);
		int close(int fd_index);
		std::vector<char[MAX_FILENAME_LENGTH]> getAllDirectoryFiles();
		int save();
		int loadDisk(const char* filename);
	private:
		io::LBASystem ios;
		OFT oft;
	};
}
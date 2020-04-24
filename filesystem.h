#pragma once
#include "oft.h"
#include "IOSystem.h"
#include "file_descriptor.h"
#include "directory_entry.h"
#include <bitset>
#include <memory>
#include <utility>

namespace filesystem {
	class FileSystem {
	public:
		enum FilesystemAnswer {
			SUCCESS,
			ERROR,
			DISK_SPACE_OVERFLOW,
			DIR_SPACE_OVERFLOW,
			NO_FREE_DESCRIPTOR,
			BAD_FILENAME,
			BAD_INDEX,
			INVALID_POSITION,
			OPEN_FILE_ERROR
		};
		FileSystem();
		int createFile(char filename[constants::MAX_FILENAME_LENGTH]);
		int destroyFile(char filename[constants::MAX_FILENAME_LENGTH]);
		int lseek(int index, int pos);
	private:
		std::unique_ptr<io::IOSystem>ios;
		std::unique_ptr<components::OFT>oft;

		void initSystemDiskBlocks();

		components::FileDescriptor getDescriptorByIndex(int idx);
		bool setDescriptorByIndex(int idx,const components::FileDescriptor&fd);

		void readBitmapFromDisk(std::bitset<constants::BLOCKS_NUM>& bitset);
		void writeBitmapToDisk(const std::bitset<constants::BLOCKS_NUM>& bitset);

		int findFreeDiskBlock(const std::bitset<constants::BLOCKS_NUM>& bitset);

		std::pair<int,components::DirectoryEntry> findFileInDirectory(char filename[constants::MAX_FILENAME_LENGTH]);

		bool writeToSystemDiskBlocks(int disk_block, int from, char*buf);

		bool writeDataToDisk(const components::OFT::OFTEntry& open_file, const char* readFrom, int size);

		char* readDataFromDisk(components::OFT::OFTEntry& open_file, int size);
	};
}
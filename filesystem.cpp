#include "filesystem.h"
#include "file_descriptor.h"

namespace filesystem {
	FileSystem::FileSystem(){
		ios = std::make_unique<io::IOSystem>(constants::BLOCKS_NUM, constants::DISC_BLOCK_SIZE, "iosystem.conf");
	}

	void FileSystem::createFile(const char* filename){
		char* main_memory_ptr = main_memory;
		int block_idx = 1;//because 0 is bit_map
		int bytes_read = 0;
		int descriptor_idx = 1;// because 0 is directory
		bool empty_descriptor_found = false;
		//finding free descriptor;
		while (!empty_descriptor_found && block_idx < constants::SYSTEM_BLOCKS_NUM) {
			components::FileDescriptor file_descriptor;
			ios->read_block(block_idx, main_memory_ptr);
			bytes_read += constants::DISC_BLOCK_SIZE;
			while (file_descriptor.readDescriptor(main_memory_ptr, bytes_read)) {
				if (file_descriptor.getFileLength() < 0) {
					empty_descriptor_found = true;
					break;
				}
				bytes_read -= file_descriptor.size();
				main_memory_ptr += file_descriptor.size();
				++descriptor_idx;
			}
			if (!empty_descriptor_found) {
				++block_idx;
			}		
		}

		//write to ios new descriptor
		int file_length = 0;
		std::memcpy(main_memory_ptr, &file_length, sizeof(int));
		ios->write_block(block_idx, main_memory);

		//TODO
		//find free space in the directory and write descriptor index and filename entry
	}
}

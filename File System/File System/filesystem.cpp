#include "filesystem.h"
#include "file_descriptor.h"
#include "directory.h"

namespace filesystem {
	FileSystem::FileSystem(){
		ios = std::make_unique<io::IOSystem>(constants::BLOCKS_NUM, constants::DISC_BLOCK_SIZE,"file1.conf");
		oft = std::make_unique<components::OFT>();
		
		std::bitset<constants::DISC_BLOCK_SIZE> bitset;
		std::memcpy(main_memory, &bitset, sizeof(bitset));
		ios->write_block(0, main_memory);

		components::FileDescriptor directory_descriptor;
		directory_descriptor.setFileLength(1);
		ios->read_block(1, main_memory);
		std::memcpy(main_memory, &directory_descriptor, sizeof(components::FileDescriptor));
		ios->write_block(1, main_memory);//because 0 is a bit_map

		oft->addFile(0);//directory always opens with idx 0;
		//TODO
		//write empty descriptors
	}

	void FileSystem::createFile(char filename[constants::MAX_FILENAME_LENGTH]){
		int block_idx = 1;//because 0 is bit_map
		int bytes_read = 0;
		int descriptor_idx = 0;
		bool empty_descriptor_found = false;

		//finding free descriptor;
		while (!empty_descriptor_found && block_idx < constants::SYSTEM_BLOCKS_NUM) {
			ios->read_block(block_idx, main_memory);
			char* main_memory_ptr = main_memory;
			bytes_read += constants::DISC_BLOCK_SIZE;
			while (bytes_read) {
				components::FileDescriptor file_descriptor;
				std::memcpy(&file_descriptor, main_memory_ptr, sizeof(components::FileDescriptor));
				if (file_descriptor.getFileLength() < 0) {//check if empty
					empty_descriptor_found = true;
					break;
				}
				else {
					bytes_read -= file_descriptor.size();
					main_memory_ptr += file_descriptor.size();
					++descriptor_idx;
				}			
			}
			if (!empty_descriptor_found) {
				++block_idx;
			}		
		}

		if (empty_descriptor_found) {
			//rewind the directory
			components::OFT::OFTEntry directory = oft->getFile(0);
			ios->read_block(1, directory.getReadWriteBuffer());
			components::FileDescriptor directory_descriptor;
			std::memcpy(&directory_descriptor, directory.getReadWriteBuffer(), sizeof(components::FileDescriptor));
			int num_of_files_in_dir = (directory_descriptor.getFileLength() - 1) / sizeof(components::Directory::DirectoryEntry);//1 is empty
			if (num_of_files_in_dir < constants::MAX_DIRECTORY_SIZE) {
				components::Directory::DirectoryEntry new_file(filename,descriptor_idx);
				int dir_size = directory_descriptor.getFileLength() - 1;
				int num_of_occupied_blocks = constants::MAX_FILE_BLOCKS - directory_descriptor.getNumOfFreeBlocks();
				int size_of_occupied_blocks = num_of_occupied_blocks * constants::DISC_BLOCK_SIZE;
				int free_space = size_of_occupied_blocks - dir_size;
				int write_offset = 0;
				if (free_space < sizeof(components::Directory::DirectoryEntry)) {
					std::bitset<constants::DISC_BLOCK_SIZE> bitset;
					int free_disc_block = findFreeDiskBlock(bitset);
					if (free_disc_block != -1) {
						bitset[free_disc_block] = 1;
						directory_descriptor.add_block(free_disc_block);
						directory_descriptor.setFileLength(directory_descriptor.getFileLength() + sizeof(components::Directory::DirectoryEntry));
						std::memcpy(directory.getReadWriteBuffer(), &directory_descriptor, sizeof(components::FileDescriptor));
						ios->write_block(1, directory.getReadWriteBuffer());

						std::memcpy(main_memory, &bitset, sizeof(bitset));
						ios->write_block(0, main_memory);
					}
					else {
						//dir blocks overflow
					}
				}
				else {
					write_offset = constants::DISC_BLOCK_SIZE - free_space;
				}
				int last_dir_block = directory_descriptor.get_last_block();
				ios->read_block(last_dir_block, directory.getReadWriteBuffer());
				std::memcpy(directory.getReadWriteBuffer() + write_offset, &new_file, sizeof(components::Directory::DirectoryEntry));
				ios->write_block(last_dir_block, directory.getReadWriteBuffer());//write to disk new dir entry

				//write to disk new file descriptor
				components::FileDescriptor file_descriptor;
				file_descriptor.setFileLength(1);
				ios->read_block(block_idx, main_memory);
				int offset = sizeof(components::FileDescriptor) * descriptor_idx;
				while (offset > constants::DISC_BLOCK_SIZE) {
					offset -= constants::DISC_BLOCK_SIZE;
				}
				std::memcpy(main_memory + offset, &file_descriptor, sizeof(components::FileDescriptor));
				ios->write_block(block_idx, main_memory);
			}
			else {
				//dir size overflow
			}
			//TODO
			//check name repeating
		}
		
	}

	int FileSystem::findFreeDiskBlock(std::bitset<constants::DISC_BLOCK_SIZE>& bitset){
		ios->read_block(0, main_memory);
		std::memcpy(&bitset, main_memory, sizeof(bitset));
		int free_disc_block = -1;
		for (int i = constants::SYSTEM_BLOCKS_NUM; i < constants::DISC_BLOCK_SIZE; i++) {
			if (bitset[i] == 0) {
				free_disc_block = i;
				break;
			}
		}
		return free_disc_block;
	}
}

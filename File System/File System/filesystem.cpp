#include "filesystem.h"
#include "directory_entry.h"

namespace filesystem {
	FileSystem::FileSystem(){
		ios = std::make_unique<io::IOSystem>(constants::BLOCKS_NUM, constants::DISC_BLOCK_SIZE,"file1.conf");
		oft = std::make_unique<components::OFT>();
		initSystemDiskBlocks();
		oft->addFile(0);//directory always opens with idx 0;
	}

	void FileSystem::createFile(char filename[constants::MAX_FILENAME_LENGTH]){
		int block_idx = 1;//because 0 is bit_map
		int bytes_read = 0;
		int descriptor_idx = 0;
		int file_descriptor_size = sizeof(components::FileDescriptor);
		bool empty_descriptor_found = false;

		//finding free descriptor;
		while (!empty_descriptor_found && block_idx < constants::SYSTEM_BLOCKS_NUM) {
			ios->read_block(block_idx, main_memory);
			char* main_memory_ptr = main_memory;
			bytes_read += constants::DISC_BLOCK_SIZE;
			while (bytes_read) {
				if (bytes_read < file_descriptor_size) {
					break;
				}
				components::FileDescriptor file_descriptor;
				std::memcpy(&file_descriptor, main_memory_ptr, file_descriptor_size);
				if (file_descriptor.getFileLength() == -1) {//check if descriptor is empty empty
					empty_descriptor_found = true;
					break;
				}
				else {
					bytes_read -= file_descriptor_size;
					main_memory_ptr += file_descriptor_size;
					++descriptor_idx;
				}			
			}
			if (!empty_descriptor_found) {
				++block_idx;
			}		
		}

		bool filename_is_unique = (findFileInDirectory(filename) == -1);
		if (empty_descriptor_found && filename_is_unique) {
			//rewind the directory
			components::OFT::OFTEntry directory = oft->getFile(0);
			components::FileDescriptor directory_descriptor = getDirectoryDescriptor();
			int num_of_files_in_dir = (directory_descriptor.getFileLength()) / sizeof(components::DirectoryEntry);//1 is empty
			if (num_of_files_in_dir < constants::MAX_DIRECTORY_SIZE) {
				components::DirectoryEntry new_file(filename,descriptor_idx);
				int dir_size = directory_descriptor.getFileLength();
				int num_of_occupied_blocks = constants::MAX_FILE_BLOCKS - directory_descriptor.getNumOfFreeBlocks();
				int size_of_occupied_blocks = num_of_occupied_blocks * constants::DISC_BLOCK_SIZE;
				int free_space = size_of_occupied_blocks - dir_size;
				int write_offset = 0;
				if (free_space < sizeof(components::DirectoryEntry)) {
					std::bitset<constants::DISC_BLOCK_SIZE> bitset;
					ios->read_block(0, main_memory);
					std::memcpy(&bitset, main_memory, sizeof(bitset));
					int free_disc_block = findFreeDiskBlock(bitset);
					if (free_disc_block != -1) {
						bitset[free_disc_block] = 1;
						directory_descriptor.addBlock(free_disc_block);
						directory_descriptor.setFileLength(directory_descriptor.getFileLength() + sizeof(components::DirectoryEntry));
						std::memcpy(directory.getReadWriteBuffer(), &directory_descriptor, file_descriptor_size);
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
				int last_dir_block = directory_descriptor.getLastBlockNum();
				ios->read_block(last_dir_block, directory.getReadWriteBuffer());
				std::memcpy(directory.getReadWriteBuffer() + write_offset, &new_file, sizeof(components::DirectoryEntry));
				ios->write_block(last_dir_block, directory.getReadWriteBuffer());//write to disk new dir entry

				//write to disk new file descriptor
				components::FileDescriptor file_descriptor;
				file_descriptor.setFileLength(0);
				ios->read_block(block_idx, main_memory);
				int offset = file_descriptor_size * descriptor_idx;
				while (offset > constants::DISC_BLOCK_SIZE) {
					offset -= constants::DISC_BLOCK_SIZE;
				}
				std::memcpy(main_memory + offset, &file_descriptor, file_descriptor_size);
				ios->write_block(block_idx, main_memory);
			}
			else {
				//dir size overflow or file with such name already exists
			}
		}
		
	}

	void FileSystem::destroyFile(char filename[constants::MAX_FILENAME_LENGTH]) {
		int dir_entry_idx = findFileInDirectory(filename);
		if (dir_entry_idx == -1) {
			//such file doesn`t exists
			return;
		}
		else {
			components::DirectoryEntry file_dir_entry;
			int offset = dir_entry_idx * sizeof(components::DirectoryEntry);
			int block_idx = 0;
			while (offset > constants::DISC_BLOCK_SIZE) {
				offset -= constants::DISC_BLOCK_SIZE;
				block_idx++;
			}
			components::OFT::OFTEntry directory = oft->getFile(0);
			components::FileDescriptor directory_descriptor = getDirectoryDescriptor();
			const int* dir_blocks_nums = directory_descriptor.getArrBlockNums();
			ios->read_block(dir_blocks_nums[block_idx], directory.getReadWriteBuffer());
			std::memcpy(&file_dir_entry, directory.getReadWriteBuffer() + offset, sizeof(components::DirectoryEntry));

			if (oft->findFile(file_dir_entry.descriprot_idx)) {
				//cannot destroy open file
				return;
			}
			else {
				//clear file descriptor
				int dir_entry_size = sizeof(components::DirectoryEntry);
				int file_descriptor_size = sizeof(components::FileDescriptor);
				int descriptor_disk_block = 1;
				int offset = file_descriptor_size * file_dir_entry.descriprot_idx;
				while (offset > constants::DISC_BLOCK_SIZE) {
					offset -= constants::DISC_BLOCK_SIZE;
					descriptor_disk_block++;
				}
				ios->read_block(descriptor_disk_block, main_memory);
				components::FileDescriptor file_descriptor;
				std::memcpy(&file_descriptor, main_memory + offset, file_descriptor_size);
				components::FileDescriptor empty_file_descriptor;
				std::memcpy(main_memory+offset, &empty_file_descriptor, file_descriptor_size);
				ios->write_block(descriptor_disk_block, main_memory);
				//remove file from directory			
				int num_of_files_in_dir = directory_descriptor.getFileLength() / dir_entry_size;
				int num_of_files_int_prev_blocks = ((directory_descriptor.getNumOfOccupiedBlocks() - 1) * constants::DISC_BLOCK_SIZE) / 
					sizeof(components::DirectoryEntry);
				int last_file_offset = (num_of_files_in_dir - num_of_files_int_prev_blocks - 1)* dir_entry_size;
				int dir_last_block_num = directory_descriptor.getLastBlockNum();
				ios->read_block(dir_last_block_num, main_memory);
				components::DirectoryEntry last_dir_entry;
				std::memcpy(&last_dir_entry, main_memory + last_file_offset, dir_entry_size);
				std::memcpy(directory.getReadWriteBuffer() + offset ,&last_dir_entry, dir_entry_size);
				ios->write_block(dir_blocks_nums[block_idx], directory.getReadWriteBuffer());
				//update directory descriptor
				directory_descriptor.setFileLength(directory_descriptor.getFileLength() - dir_entry_size);
				ios->read_block(1, directory.getReadWriteBuffer());
				std::memcpy(directory.getReadWriteBuffer(), &directory_descriptor, file_descriptor_size);
				ios->write_block(1, directory.getReadWriteBuffer());
				//update bitmap
				std::bitset<constants::DISC_BLOCK_SIZE> bitset;
				ios->read_block(0, main_memory);
				std::memcpy(&bitset, main_memory, sizeof(bitset));
				const int* file_blocks_nums = file_descriptor.getArrBlockNums();
				for (int i = 0; i < file_descriptor.getNumOfOccupiedBlocks(); i++) {
					bitset[file_blocks_nums[i]] = 0;
				}
				std::memcpy(main_memory, &bitset, sizeof(bitset));
				ios->write_block(0, main_memory);
			}
		}
	}

	void FileSystem::initSystemDiskBlocks(){
		//write empty bitmap
		std::bitset<constants::BLOCKS_NUM> bitset;
		std::memcpy(main_memory, &bitset, sizeof(bitset));
		ios->write_block(0, main_memory);
		//write empty file descriptors
		int file_descriptor_size = sizeof(components::FileDescriptor);
		for (int i = 1; i < constants::SYSTEM_BLOCKS_NUM; i++) {
			int bytes_read = 0;
			char* main_mem_ptr = main_memory;
			ios->read_block(i, main_memory);
			bytes_read += constants::DISC_BLOCK_SIZE;
			while (bytes_read) {
				components::FileDescriptor file_descriptor;
				if (bytes_read < file_descriptor_size) {
					break;
				}
				std::memcpy(main_mem_ptr, &file_descriptor, file_descriptor_size);
				bytes_read -= file_descriptor_size;
				main_mem_ptr += file_descriptor_size;
			}
			ios->write_block(1, main_memory);
		}
		//write non empty directory descriptor
		ios->read_block(1, main_memory);
		components::FileDescriptor directory_descriptor;
		directory_descriptor.setFileLength(0);
		std::memcpy(main_memory, &directory_descriptor, file_descriptor_size);
		ios->write_block(1, main_memory);
	}

	components::FileDescriptor FileSystem::getDirectoryDescriptor()
	{
		components::OFT::OFTEntry directory = oft->getFile(0);
		ios->read_block(1, directory.getReadWriteBuffer());
		components::FileDescriptor directory_descriptor;
		std::memcpy(&directory_descriptor, directory.getReadWriteBuffer(), sizeof(components::FileDescriptor));
		return directory_descriptor;
	}

	int FileSystem::findFileInDirectory(char filename[constants::MAX_FILENAME_LENGTH]){
		components::OFT::OFTEntry directory = oft->getFile(0);
		components::FileDescriptor directory_descriptor = getDirectoryDescriptor();
		const int* dir_blocks_nums = directory_descriptor.getArrBlockNums();
		int num_of_dir_blocks = directory_descriptor.getNumOfOccupiedBlocks();
		int num_of_files_in_dir = directory_descriptor.getFileLength() / sizeof(components::DirectoryEntry);
		int dir_entry_size = sizeof(components::DirectoryEntry);
		int dir_entry_idx = 0;
		for (int i = 0; i < num_of_dir_blocks; i++) {
			ios->read_block(dir_blocks_nums[i], directory.getReadWriteBuffer());
			char* dir_buf_ptr = directory.getReadWriteBuffer();
			int bytes_read = constants::DISC_BLOCK_SIZE;
			while (bytes_read && num_of_files_in_dir){
				if (bytes_read < dir_entry_size) {
					break;
				}
				components::DirectoryEntry dir_entry;
				std::memcpy(&dir_entry, dir_buf_ptr, dir_entry_size);
				if (std::strcmp(filename, dir_entry.filename) == 0) {
					return dir_entry_idx;
				}
				num_of_files_in_dir--;
				bytes_read -= dir_entry_size;
				dir_buf_ptr += dir_entry_size;
			}
		}
		return -1;
	}

	int FileSystem::findFreeDiskBlock(const std::bitset<constants::DISC_BLOCK_SIZE>& bitset){
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

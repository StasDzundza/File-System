
#include "filesystem.h"

namespace filesystem {
	FileSystem::FileSystem(){
		ios = std::make_unique<io::IOSystem>(constants::BLOCKS_NUM, constants::DISC_BLOCK_SIZE,"file1.conf");
		oft = std::make_unique<components::OFT>();
		initSystemDiskBlocks();
		oft->addFile(0);//directory always opens with idx 0;
	}

	/*
		reserves descriptor for file,writes file directory entry and updates bitmap
		returns result code
	*/
	int FileSystem::createFile(char filename[constants::MAX_FILENAME_LENGTH]) {
		//finding free descriptor;
		int descriptor_idx = 1;//because 0 is a directory
		int file_descriptor_size = sizeof(components::FileDescriptor);
		bool empty_descriptor_found = false;
	
		int num_of_file_descriptors = (constants::SYSTEM_BLOCKS_NUM - 1) * constants::DISC_BLOCK_SIZE / file_descriptor_size;
		while (!empty_descriptor_found && descriptor_idx < num_of_file_descriptors) {
			components::FileDescriptor file_descriptor = getDescriptorByIndex(descriptor_idx);
			if (file_descriptor.getFileLength() == -1) {//check if descriptor is empty empty
				empty_descriptor_found = true;
				break;
			}
			else {
				++descriptor_idx;
			}
		}
	
		bool filename_is_unique = (findFileInDirectory(filename).first == -1);
		if (empty_descriptor_found && filename_is_unique) {
			//rewind the directory
			const int dir_entry_size = sizeof(components::DirectoryEntry);
			components::OFT::OFTEntry directory = oft->getFile(0);
			components::FileDescriptor directory_descriptor = getDescriptorByIndex(0);
			int num_of_files_in_dir = directory_descriptor.getFileLength() / sizeof(components::DirectoryEntry);
			if (num_of_files_in_dir < constants::MAX_DIRECTORY_SIZE) {
				components::DirectoryEntry new_file(filename, descriptor_idx);
				char data[dir_entry_size];
				std::memcpy(data, &new_file, dir_entry_size);
				lseek(0, directory_descriptor.getFileLength());
				if (writeDataToDisk(directory, data, dir_entry_size)) {
					//write to disk new file descriptor
					components::FileDescriptor file_descriptor;
					file_descriptor.setFileLength(0);
					setDescriptorByIndex(descriptor_idx, file_descriptor);
					return FilesystemAnswer::SUCCESS;
				}			
				else {
					lseek(0, 0);
					return FilesystemAnswer::ERROR;
				}
			}
			else {
				return FilesystemAnswer::DIR_SPACE_OVERFLOW;
			}
		}
		else {
			if (!filename_is_unique) {
				return FilesystemAnswer::BAD_FILENAME;
			}
			else {
				return FilesystemAnswer::NO_FREE_DESCRIPTOR;
			}
		}
	}

	/*
		destroys file directory entry,clear file descriptor and updates bitmap
		returns result code
	*/
	int FileSystem::destroyFile(char filename[constants::MAX_FILENAME_LENGTH]) {
		std::pair<int, components::DirectoryEntry> dir_entry_data = findFileInDirectory(filename);
		int dir_entry_idx = dir_entry_data.first;
		if (dir_entry_idx == -1) {
			return FilesystemAnswer::BAD_FILENAME;
		}
		else {
			components::DirectoryEntry file_dir_entry = dir_entry_data.second;
			if (oft->findFile(file_dir_entry.descriprot_idx) != nullptr) {
				//cannot destroy open file
				return FilesystemAnswer::OPEN_FILE_ERROR;
			}
			else {
				//clear file descriptor
				const int dir_entry_size = sizeof(components::DirectoryEntry);
				int file_descriptor_size = sizeof(components::FileDescriptor);
				components::FileDescriptor file_descriptor = getDescriptorByIndex(file_dir_entry.descriprot_idx);
				setDescriptorByIndex(file_dir_entry.descriprot_idx, components::FileDescriptor());
				//remove file from directory	
				components::OFT::OFTEntry directory = oft->getFile(0);
				components::FileDescriptor directory_descriptor = getDescriptorByIndex(0);

				int offset = directory_descriptor.getFileLength();
				lseek(0, offset);
				char* data = readDataFromDisk(directory, dir_entry_size);
				components::DirectoryEntry last_dir_entry;
				std::memcpy(&last_dir_entry, data, dir_entry_size);

				components::DirectoryEntry empty_entry;
				std::memcpy(data, &empty_entry, dir_entry_size);
				writeDataToDisk(directory, data, dir_entry_size);

				offset = dir_entry_size * dir_entry_data.first;
				lseek(0, offset);
				std::memcpy(data, &last_dir_entry, dir_entry_size);
				writeDataToDisk(directory, data, dir_entry_size);
				lseek(0, 0);

				//update directory descriptor
				directory_descriptor.setFileLength(directory_descriptor.getFileLength() - dir_entry_size);
				int free_space_in_dir = (directory_descriptor.getNumOfOccupiedBlocks * constants::DISC_BLOCK_SIZE) - 
					directory_descriptor.getFileLength();
				while (free_space_in_dir >= constants::DISC_BLOCK_SIZE) {
					free_space_in_dir -= constants::DISC_BLOCK_SIZE;
					directory_descriptor.freeLastBlock();
				}
				setDescriptorByIndex(0, directory_descriptor);
				//update bitmap
				std::bitset<constants::DISC_BLOCK_SIZE> bitset;
				readBitmapFromDisk(bitset);
				const int* file_blocks_nums = file_descriptor.getArrBlockNums();
				for (int i = 0; i < file_descriptor.getNumOfOccupiedBlocks(); i++) {
					bitset[file_blocks_nums[i]] = 0;
				}
				writeBitmapToDisk(bitset);
				return FilesystemAnswer::SUCCESS;
			}
		}
	}

	/*
		sets position in file to pos
		returnsresult code
	*/
	int FileSystem::lseek(int index, int pos)
	{
		components::OFT::OFTEntry* file_entry = oft->findFile(index);
		if (file_entry != nullptr) {
			int file_length = getDescriptorByIndex(index).getFileLength();
			if (pos < 0 || pos > file_length) {
				return FilesystemAnswer::INVALID_POSITION;
			}
			else {
				file_entry->setPosition(pos);
			}
		}
		else {
			return FilesystemAnswer::BAD_INDEX;
		}
	}

	/*
		initialize disk blocks
		write empty bitmap,directory descriptor,empty file descriptors
	*/
	void FileSystem::initSystemDiskBlocks() {
		//write empty bitmap
		std::bitset<constants::BLOCKS_NUM> bitset;
		writeBitmapToDisk(bitset);
		//find the number of descriptors
		const int file_descriptor_size = sizeof(components::FileDescriptor);
		const int num_of_file_descriptors = (constants::SYSTEM_BLOCKS_NUM - 1) * constants::DISC_BLOCK_SIZE / file_descriptor_size;
		const int BUF_SIZE = num_of_file_descriptors * file_descriptor_size;
		char descriptors_buf[BUF_SIZE];
		//write directory descriptor
		components::FileDescriptor directory_descriptor;
		directory_descriptor.setFileLength(0);
		std::memcpy(descriptors_buf, &directory_descriptor, file_descriptor_size);
		//write other empty desriptors
		int tmp = num_of_file_descriptors - 1;
		int offset = file_descriptor_size;
		while (tmp--) {
			components::FileDescriptor file_descriptor;
			std::memcpy(descriptors_buf + offset, &file_descriptor, file_descriptor_size);
			offset += file_descriptor_size;
		}
		writeToSystemDiskBlocks(1, 0, descriptors_buf);
	}

	/*
		returns file descriptor by given index
		if no such descriptor returns descriptor with file length -2
	*/
	components::FileDescriptor FileSystem::getDescriptorByIndex(int idx)
	{
		int block_idx = 1;//because 0 is bit_map
		int bytes_read = 0;
		int descriptor_idx = 0;
		int file_descriptor_size = sizeof(components::FileDescriptor);
		char tmp_buf[constants::MAIN_MEMORY_SIZE];
		char* main_mem_write_ptr = tmp_buf;
		char* main_mem_read_ptr = tmp_buf;
		while (block_idx < constants::SYSTEM_BLOCKS_NUM) {
			ios->read_block(block_idx, main_mem_read_ptr);
			bytes_read += constants::DISC_BLOCK_SIZE;
			if (bytes_read < file_descriptor_size) {
				main_mem_read_ptr += bytes_read;
			}
			while (bytes_read) {
				if (bytes_read < file_descriptor_size) {
					main_mem_read_ptr += bytes_read;
					break;
				}
				else {
					components::FileDescriptor file_descriptor;
					std::memcpy(&file_descriptor, main_mem_write_ptr, file_descriptor_size);
					if (descriptor_idx == idx) {
						return file_descriptor;
						break;
					}
					else {
						bytes_read -= file_descriptor_size;
						main_mem_write_ptr += file_descriptor_size;
						main_mem_read_ptr += file_descriptor_size;
						++descriptor_idx;
					}
				}
			}
			++block_idx;
		}
	}

	/*
		writes descriptor to disk by index
		returns true or false
	*/
	bool FileSystem::setDescriptorByIndex(int idx, const components::FileDescriptor& fd)
	{
		const int file_descriptor_size = sizeof(components::FileDescriptor);
		char buf[file_descriptor_size];
		std::memcpy(buf, &fd, file_descriptor_size);
		int offset = file_descriptor_size * idx;
		int disk_block_idx = 1;
		while (offset > constants::DISC_BLOCK_SIZE) {
			offset -= constants::DISC_BLOCK_SIZE;
			disk_block_idx++;
		}
		return writeToSystemDiskBlocks(disk_block_idx, offset, buf);
	}

	/*
		reads bitmap from disk
	*/
	void FileSystem::readBitmapFromDisk(std::bitset<constants::DISC_BLOCK_SIZE>& bitset) {
		char tmp_buf[constants::DISC_BLOCK_SIZE];
		ios->read_block(0, tmp_buf);
		std::memcpy(&bitset, tmp_buf, sizeof(bitset));
	}

	/*
		writes bitmap to disk
	*/
	void FileSystem::writeBitmapToDisk(const std::bitset<constants::DISC_BLOCK_SIZE>& bitset) {
		char tmp_buf[constants::DISC_BLOCK_SIZE];
		std::memcpy(tmp_buf, &bitset, sizeof(bitset));
		ios->write_block(0, tmp_buf);
	}

	/*
		finds file in directory by name
		returns pair<dir_entry_index,dir_entry_object>
		returns pair<-1,empty_object> if not found such name
	*/
	std::pair<int, components::DirectoryEntry> FileSystem::findFileInDirectory(char filename[constants::MAX_FILENAME_LENGTH]){
		components::OFT::OFTEntry directory = oft->getFile(0);
		components::FileDescriptor directory_descriptor = getDescriptorByIndex(0);
		const int* dir_blocks_nums = directory_descriptor.getArrBlockNums();
		int num_of_dir_blocks = directory_descriptor.getNumOfOccupiedBlocks();
		int num_of_files_in_dir = directory_descriptor.getFileLength() / sizeof(components::DirectoryEntry);
		int dir_entry_size = sizeof(components::DirectoryEntry);
		int dir_entry_idx = 0;
		int bytes_read = 0;
		char tmp_buf[constants::MAIN_MEMORY_SIZE];
		char* main_mem_write_ptr = tmp_buf;
		char* main_mem_read_ptr = tmp_buf;
		for (int i = 0; i < num_of_dir_blocks; i++) {
			ios->read_block(dir_blocks_nums[i], directory.getReadWriteBuffer());
			std::memcpy(main_mem_read_ptr, directory.getReadWriteBuffer(), constants::DISC_BLOCK_SIZE);
			bytes_read += constants::DISC_BLOCK_SIZE;
			while (bytes_read && num_of_files_in_dir){
				if (bytes_read < dir_entry_size) {
					main_mem_read_ptr += bytes_read;
					break;
				}
				components::DirectoryEntry dir_entry;
				std::memcpy(&dir_entry, main_mem_write_ptr, dir_entry_size);
				if (std::strcmp(filename, dir_entry.filename) == 0) {
					return std::make_pair(dir_entry_idx,dir_entry);
				}
				dir_entry_idx++;
				num_of_files_in_dir--;
				bytes_read -= dir_entry_size;
				main_mem_write_ptr += dir_entry_size;
				main_mem_read_ptr += dir_entry_size;
			}
		}
		return std::make_pair(-1, components::DirectoryEntry());
	}

	/*
		finds free space in disk with the help of bitset
		returns -1 if there are no free space
	*/
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

	/*
		writes given buf to given pos to first system disk blocks
		returns true if write is success,false if vice versa
	*/
	bool FileSystem::writeToSystemDiskBlocks(int disk_block, int from, char* buf) {
		char tmp_buf[constants::MAIN_MEMORY_SIZE];
		ios->read_block(disk_block, tmp_buf);
		char* main_mem_ptr = tmp_buf;
		std::memcpy(tmp_buf + from, &buf, sizeof(components::FileDescriptor));
		if (constants::DISC_BLOCK_SIZE - from < sizeof(components::FileDescriptor)) {
			int bytes_wrote = 0;
			while (bytes_wrote < sizeof(components::FileDescriptor) + constants::DISC_BLOCK_SIZE) {
				if (disk_block < constants::SYSTEM_BLOCKS_NUM) {
					ios->write_block(disk_block, main_mem_ptr);
					main_mem_ptr += constants::DISC_BLOCK_SIZE;
					bytes_wrote += constants::DISC_BLOCK_SIZE;
					disk_block++;
				}
				else {
					return false;
				}
			}
		}
		else {
			ios->write_block(disk_block, tmp_buf);
		}
		return true;
	}

	/*
		writes given buffer to file block in disk and updates bitmap and descriptors
		return true is Success,false if vice versa
	*/
	bool FileSystem::writeDataToDisk(const components::OFT::OFTEntry& open_file, const char* readFrom, int size)
	{
		return true;
	}

	/*
		reads data from file block in disk
		return buffer with data
	*/
	char* FileSystem::readDataFromDisk(components::OFT::OFTEntry& open_file, int size)
	{
		return false;
	}
}

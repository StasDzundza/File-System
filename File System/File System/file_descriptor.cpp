#include "file_descriptor.h"

#include <algorithm>

namespace filesystem::components {
	FileDescriptor::FileDescriptor(){
		std::fill(arr_block_num, arr_block_num + constants::MAX_FILE_BLOCKS, -1);
	}

	int FileDescriptor::size(){
		return sizeof(int) + sizeof(int)*constants::MAX_FILE_BLOCKS;
	}

	bool FileDescriptor::readDescriptor(char* read_from, int bytes_read){
		if (bytes_read < size()) {
			return false;
		}
		else {
			std::memcpy(&file_length, read_from, sizeof(int));
			std::memcpy(arr_block_num, read_from + sizeof(int), sizeof(arr_block_num));
			return true;
		}
	}

	int FileDescriptor::getFileLength()
	{
		return file_length;
	}

	void FileDescriptor::setFileLength(int new_file_length){
		file_length = new_file_length;
	}

	int FileDescriptor::getNumOfFreeBlocks()
	{
		int i = 0;
		for (; i < constants::MAX_FILE_BLOCKS; i++) {
			if (arr_block_num[i] == -1) {
				break;
			}
		}
		return constants::MAX_FILE_BLOCKS - i;
	}

	void FileDescriptor::add_block(int idx){
		if (getNumOfFreeBlocks() == 0) {
			//throw exception
		}
		else {
			for (int i = 0; i < constants::MAX_FILE_BLOCKS; i++) {
				if (arr_block_num[i] == -1) {
					arr_block_num[i] = idx;
					break;
				}
			}
		}
	}

	int FileDescriptor::get_last_block() const
	{
		for (int i = 0; i < constants::MAX_FILE_BLOCKS; i++) {
			if (arr_block_num[i] == -1) {
				if (i > 0) {
					return arr_block_num[i - 1];
				}
				else {
					return -1;
				}
			}
		}
	}

	const int* FileDescriptor::getArrBlockNums()
	{
		return arr_block_num;
	}

}
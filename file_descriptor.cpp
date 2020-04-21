#include "file_descriptor.h"
#include <algorithm>

namespace filesystem::components {
	FileDescriptor::FileDescriptor(){
		std::fill(arr_block_num, arr_block_num + constants::MAX_FILE_BLOCKS, -1);
	}

	int FileDescriptor::getFileLength()const{
		return file_length;
	}

	void FileDescriptor::setFileLength(int new_file_length){
		file_length = new_file_length;
	}

	int FileDescriptor::getNumOfFreeBlocks()const{
		int i = 0;
		for (; i < constants::MAX_FILE_BLOCKS; i++) {
			if (arr_block_num[i] == -1) {
				break;
			}
		}
		return constants::MAX_FILE_BLOCKS - i;
	}

	int FileDescriptor::getNumOfOccupiedBlocks() const{
		int num_of_occupied_blocks = 0;
		for (int block_num : arr_block_num) {
			if (block_num != -1) {
				num_of_occupied_blocks++;
			}
			else{
				break;
			}
		}
		return num_of_occupied_blocks;
	}

	void FileDescriptor::reset(){
		file_length = -1;
		std::fill(arr_block_num, arr_block_num + constants::MAX_FILE_BLOCKS, -1);
	}

	void FileDescriptor::addBlock(int idx){
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

	int FileDescriptor::getLastBlockNum() const{
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

	const int* FileDescriptor::getArrBlockNums()const{
		return arr_block_num;
	}

	void FileDescriptor::freeLastBlock(){
		int lastBlock = getNumOfOccupiedBlocks() - 1;
		if (lastBlock >= 0) {
			arr_block_num[lastBlock] = -1;
		}
	}

}
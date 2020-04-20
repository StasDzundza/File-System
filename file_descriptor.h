#pragma once
#include <vector>
#include "file_system_constants.h"

namespace filesystem::components {

	class FileDescriptor {
	public:
		FileDescriptor();

		int getFileLength()const;
		void setFileLength(int new_file_length);

		int getNumOfFreeBlocks()const;
		int getNumOfOccupiedBlocks()const;

		void reset();

		void addBlock(int idx);

		int getLastBlockNum()const;

		const int* getArrBlockNums()const;
	private:
		int file_length = -1;
		int arr_block_num[constants::MAX_FILE_BLOCKS];
	};
}

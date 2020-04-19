#pragma once
#include <vector>
#include "file_system_constants.h"
namespace filesystem::components {
	/*class FileDescriptor {
	public:
		int getFileLength()const noexcept;
		void increaseFileLength(const   int& length);
		void decreaseFileLength(const   int& length);
		void setFileLength(const   int& new_length);

		std::vector<int>& getDiskBlockNumbers();
		void addDiskBlockNumber(const   int& disk_block_number);

	private:
		//in bytes
		int file_length = 0;

		static const int DISK_BLOCK_NUMBERS_MAX_LENGTH = 3;
		std::vector<int> disk_block_numbers = { -1,-1,-1 };
		int disk_block_numbers_size = 0;
	};*/

	class FileDescriptor {
	public:
		FileDescriptor();
		int size();
		bool readDescriptor(char* read_from, int bytes_read);
		int getFileLength();
		void setFileLength(int new_file_length);
		int getNumOfFreeBlocks();

		void add_block(int idx);
		int get_last_block()const;
		const int* getArrBlockNums();
	private:
		int file_length = 0;
		int arr_block_num[constants::MAX_FILE_BLOCKS];
	};
}

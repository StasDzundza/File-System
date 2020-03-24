#pragma once
#include<vector>

class FileDescriptor{
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
	std::vector<int> disk_block_numbers = {-1,-1,-1};
	  int disk_block_numbers_size = 0;
};


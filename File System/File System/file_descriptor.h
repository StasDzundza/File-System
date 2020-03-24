#pragma once
class FileDescriptor{
public:
	int getFileLength()const;
	void increaseFileLength(const int& length);
	void setFileLength(const int& new_length);

	int* getDiskBlockNumbers();
	void addDickBlockNumber(const int& disk_block_number);

private:
	//in bytes
	int file_length = 0;

	static const int DISK_BLOCK_NUMBERS_MAX_LENGTH = 3;
	int disk_block_numbers[DISK_BLOCK_NUMBERS_MAX_LENGTH] = {-1,-1,-1};
	int disk_block_numbers_size = 0;
};


#include "file_descriptor.h"
#include <stdexcept>

int FileDescriptor::getFileLength()const
{
	return file_length;
}

void FileDescriptor::increaseFileLength(const int& length)
{
	file_length += length;
}

void FileDescriptor::setFileLength(const int& new_file_length)
{
	file_length = new_file_length;
}

int* FileDescriptor::getDiskBlockNumbers()
{
	return disk_block_numbers;
}

void FileDescriptor::addDickBlockNumber(const int& disk_block_number){
	if (disk_block_numbers_size < DISK_BLOCK_NUMBERS_MAX_LENGTH) {
		disk_block_numbers[disk_block_numbers_size++] = disk_block_number;
	}
	else {
		throw std::runtime_error("Disk block overflow");
	}
}

#include "file_descriptor.h"
#include <stdexcept>

int FileDescriptor::getFileLength()const noexcept
{
	return file_length;
}

void FileDescriptor::increaseFileLength(const   int& length)
{
	//TODO throw exception when overflow
	file_length += length;
}

void FileDescriptor::decreaseFileLength(const   int& length)
{
	if (file_length - length < 0) {
		throw std::logic_error("File size can`t be less than 0");
	}
	else {
		file_length -= length;
	}
}

void FileDescriptor::setFileLength(const   int& new_file_length)
{
	//TODO throw exception when overflow
	file_length = new_file_length;
}

std::vector<int>& FileDescriptor::getDiskBlockNumbers()
{
	return disk_block_numbers;
}

void FileDescriptor::addDiskBlockNumber(const   int& disk_block_number){
	if (disk_block_numbers_size < DISK_BLOCK_NUMBERS_MAX_LENGTH) {
		disk_block_numbers[disk_block_numbers_size++] = disk_block_number;
	}
	else {
		throw std::runtime_error("Disk block overflow");
	}
}

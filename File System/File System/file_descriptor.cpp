#include "file_descriptor.h"

namespace filesystem::components {

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

}
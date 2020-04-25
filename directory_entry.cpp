#define _CRT_SECURE_NO_WARNINGS

#include "directory_entry.h"
#include <string.h>

namespace filesystem::components {
	DirectoryEntry::DirectoryEntry(char filename[constants::MAX_FILENAME_LENGTH], int fd_index){
		strncpy(this->filename, filename, sizeof(filename));
		this->fd_index = fd_index;
	}
}
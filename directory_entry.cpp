#define _CRT_SECURE_NO_WARNINGS

#include "directory_entry.h"
#include <stdexcept>
#include <algorithm>

namespace filesystem::components {
	DirectoryEntry::DirectoryEntry(char filename[constants::MAX_FILENAME_LENGTH], int descriptor_index){
		std::strcpy(this->filename, filename);
		this->descriprot_idx = descriptor_index;
	}
}
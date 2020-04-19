#define _CRT_SECURE_NO_WARNINGS

#include "directory.h"
#include <stdexcept>
#include <algorithm>

namespace filesystem::components {
	Directory::Directory() {
	}

	Directory::DirectoryEntry::DirectoryEntry(char filename[constants::MAX_FILENAME_LENGTH], int descriptor_index){
		std::strcpy(this->filename, filename);
		this->descriprot_idx = descriptor_index;
	}

	void Directory::addFile(char filename[constants::MAX_FILENAME_LENGTH], int descriptor_index) {
		if (std::strlen(filename) > 0 and std::strlen(filename) <= filesystem::constants::MAX_FILENAME_LENGTH and
			files.size() < constants::MAX_DIRECTORY_SIZE) {
			DirectoryEntry entry;
			std::strcpy(entry.filename, filename);
			entry.descriprot_idx = descriptor_index;
			files.push_back(entry);
		}
		else {
			throw std::logic_error("Bad filename length");
		}
	}

	std::vector<Directory::DirectoryEntry> Directory::getFiles()
	{
		return files;
	}

	/*void Directory::removeFile(const char* filename)
	{
		auto it = std::remove_if(files.begin(), files.end(), [=](const std::pair<const char*, int> file) {
			if (std::strcmp(filename,file.first)) {
				return true;
			}
			else {
				return false;
			}
			});
		files.erase(it, files.end());
	}

	void Directory::removeFile(int descriptor_index){
		auto it = std::remove_if(files.begin(), files.end(), [=](const std::pair<const char*, int> file) {
			if (file.second == descriptor_index) {
				return true;
			}
			else {
				return false;
			}
			});
		files.erase(it, files.end());
	}*/

	int Directory::getNumberOfFiles() const noexcept{
		return files.size();
	}
}
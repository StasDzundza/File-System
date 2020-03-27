#include "directory.h"
#include <stdexcept>
#include <algorithm>
namespace filesystem::components {
	Directory::Directory() {
	}

	void Directory::addFile(const char* filename, const int& descriptor_index) {
		if (std::strlen(filename) > 0 and std::strlen(filename) <= filesystem::constants::MAX_FILENAME_LENGTH and
			files.size() < constants::MAX_DIRECTORY_SIZE) {
			files.emplace_back(filename, descriptor_index);
		}
		else {
			throw std::logic_error("Bad filename length");
		}
	}

	void Directory::removeFile(const char* filename)
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

	void Directory::removeFile(const int& descriptor_index){
		auto it = std::remove_if(files.begin(), files.end(), [=](const std::pair<const char*, int> file) {
			if (file.second == descriptor_index) {
				return true;
			}
			else {
				return false;
			}
			});
		files.erase(it, files.end());
	}

	int Directory::getNumberOfFiles() const noexcept{
		return files.size();
	}
}
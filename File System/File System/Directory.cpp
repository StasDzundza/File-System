#include "directory.h"
#include <stdexcept>
#include <algorithm>

void Directory::addFile(const std::string& filename, const int& descriptor_index){
	if (filename.size() > 0 and filename.size() <= filesystem::constants::MAX_FILENAME_LENGTH) {
		files.emplace_back(filename, descriptor_index);
	}
	else {
		throw std::logic_error("Bad filename length");
	}
}

void Directory::removeFile(const std::string& filename)
{
	auto it = std::remove_if(files.begin(), files.end(), [=](const std::pair<std::string, int> file) {
		if (file.first == filename) {
			return true;
		}
		else {
			return false;
		}
	});
	files.erase(it, files.end());
}

void Directory::removeFile(const int& descriptor_index)
{
	auto it = std::remove_if(files.begin(), files.end(), [=](const std::pair<std::string, int> file) {
		if (file.second == descriptor_index) {
			return true;
		}
		else {
			return false;
		}
	});
	files.erase(it, files.end());
}

int Directory::getNumberOfFiles() const noexcept
{
	return files.size();
}

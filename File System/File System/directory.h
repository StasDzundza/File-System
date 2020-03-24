#pragma once

#include <vector>
#include <utility>
#include <string>
#include "file_system_constants.h"

class Directory
{
public:
	void addFile(const std::string& filename, const int& descriptor_index);
	void removeFile(const std::string& filename);
	void removeFile(const int& descriptor_index);
	int getNumberOfFiles()const noexcept;
private:
	std::vector<std::pair<std::string, int>> files;
};


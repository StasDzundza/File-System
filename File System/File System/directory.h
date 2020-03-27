#pragma once

#include <vector>
#include <utility>
#include <string>
#include "file_system_constants.h"

namespace filesystem::components {
	class Directory
	{
	public:
		Directory();
		void addFile(const char* filename, const int& descriptor_index);
		void removeFile(const char* filename);
		void removeFile(const int& descriptor_index);
		int getNumberOfFiles()const noexcept;
	private:
		std::vector<std::pair<const char*, int>> files;
	};
}


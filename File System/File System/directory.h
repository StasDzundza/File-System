#pragma once

#include <vector>
#include <utility>
#include <string>
#include "file_system_constants.h"

namespace filesystem::components {
	class Directory
	{
	public:
		class DirectoryEntry {
		public:
			DirectoryEntry() = default;
			DirectoryEntry(char filename[constants::MAX_FILENAME_LENGTH], int descriptor_index);
			char filename[constants::MAX_FILENAME_LENGTH];
			int descriprot_idx = -1;
		};
		Directory();
		void addFile(char filename[constants::MAX_FILENAME_LENGTH], int descriptor_index);
		std::vector<DirectoryEntry> getFiles();
		void removeFile(const char* filename);
		void removeFile(int descriptor_index);
		int getNumberOfFiles()const noexcept;
		int getDirectorySize()const noexcept;
	private:
		std::vector<DirectoryEntry> files;
	};
}


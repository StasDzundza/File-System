#pragma once

#include <vector>
#include <utility>
#include <string>
#include "file_system_constants.h"

namespace filesystem::components {

	class DirectoryEntry {
	public:
		DirectoryEntry() = default;
		DirectoryEntry(char filename[constants::MAX_FILENAME_LENGTH], int descriptor_index);
		char filename[constants::MAX_FILENAME_LENGTH];
		int descriprot_idx = -1;
	};
}

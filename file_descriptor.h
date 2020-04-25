#pragma once
#include <vector>
#include "file_system_constants.h"

namespace filesystem::components {

	struct FileDescriptor {
		int file_length = -1;
		int arr_block_num[constants::MAX_FILE_BLOCKS];
	};
}

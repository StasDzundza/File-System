#pragma once

namespace filesystem::constants {
	static const int MAX_FILENAME_LENGTH = 4;
	static const int MAX_DIRECTORY_SIZE = 10; //10 files
	static const int MAX_NUMBER_OF_OPEN_FILES = 3;
	static const int BLOCKS_NUM = 64; // L
	static const int SYSTEM_BLOCKS_NUM = 6; // K
	static const int DISC_BLOCK_SIZE = 64;//64 bytes(recomended min num is 64)
	static const int MAX_FILE_BLOCKS = 3;
	static const int MAIN_MEMORY_SIZE = DISC_BLOCK_SIZE* SYSTEM_BLOCKS_NUM;
}
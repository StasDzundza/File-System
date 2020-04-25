#pragma once

namespace filesystem::constants {
	const int BLOCK_SIZE = 64;          // 64 bytes size of physical memory block 
	const int DISC_BLOCKS_NUM = 32;     // size of physical disk
	const int SYSTEM_BLOCKS_NUM = 8;    // blocks occupied by the fs

	const int MAIN_MEMORY_BUF = 128;

	// FD_CREATED_LIMIT*(sizeof(int)+MAX_FILENAME_LENGTH)+DISC_BLOCKS_NUM/8 < SYSTEM_BLOCKS_NUM*BLOCK_SIZE
	const int FD_CREATED_LIMIT = 10;     // 10 file descriptors
	const int FD_OPENED_LIMIT = 2*FD_CREATED_LIMIT/3;       // 4 open files
	const int MAX_FILENAME_LENGTH = 6;
	const int MAX_FILE_BLOCKS = 3;       // 3 blocks per file
}
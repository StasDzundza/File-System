#include "IOSystem.h"
// #define NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

namespace filesystem::io {
	bool IOSystem::file_exists(const char *path) const {
		struct stat buffer;
		return (stat(path, &buffer) == 0);
	}

	IOSystem::IOSystem()
		: _blocks_num(DISC_BLOCKS_NUM), _block_len(BLOCK_SIZE) {
	}

	void IOSystem::init(const char *system_state_path) {
		assert(system_state_path);
		_system_state_path = system_state_path;
		if (file_exists(_system_state_path)) {
			restore_system_state();
		}
	}

	IOSystem::~IOSystem() {
		save_system_state();
	}

	void IOSystem::read_block(int block_idx, char *copy_to_ptr) {
		assert(block_idx >= 0 && block_idx < _blocks_num && copy_to_ptr);

		memcpy(copy_to_ptr, _ldisk[block_idx], sizeof(char) * _block_len);
	}

	void IOSystem::write_block(int block_idx, char *copy_from_ptr) {
		assert(block_idx >= 0 && block_idx < _blocks_num && copy_from_ptr);

		memcpy(_ldisk[block_idx], copy_from_ptr, sizeof(char) * _block_len);
	}

	void IOSystem::save_system_state() {
		FILE *file_ptr;
		assert(file_ptr = fopen(_system_state_path, "wb"));

		fwrite(&_blocks_num, sizeof(int), 1, file_ptr);
		fwrite(&_block_len, sizeof(int), 1, file_ptr);

		for (int idx = 0; idx < _blocks_num; ++idx)
			fwrite(_ldisk[idx], sizeof(char) * _block_len, 1, file_ptr);

		fclose(file_ptr);
	}

	void IOSystem::restore_system_state() {
		FILE *file_ptr;
		assert(file_ptr = fopen(_system_state_path, "rb"));

		fread(&_blocks_num, sizeof(int), 1, file_ptr);
		fread(&_block_len, sizeof(int), 1, file_ptr);

		for (int idx = 0; idx < _blocks_num; ++idx)
			fread(_ldisk[idx], sizeof(char) * _block_len, 1, file_ptr);

		fclose(file_ptr);
	}
} // namespace filesystem::io

#define _CRT_SECURE_NO_WARNINGS
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

	char **IOSystem::reserve_disk_space(int row_size, int col_size) {
		char **disk_ptr = (char **)malloc(row_size * sizeof(char *));
		for (int idx = 0; idx < row_size; ++idx)
			disk_ptr[idx] = (char *)malloc(col_size * sizeof(char));
		return disk_ptr;
	}

	void IOSystem::free_disk_space(char **disk_ptr, int row_size, int col_size) {
		for (int idx = 0; idx < row_size; ++idx)
			free(disk_ptr[idx]);
		free(disk_ptr);
	}

	IOSystem::IOSystem(const char *system_state_path)
		: _blocks_num(0), _block_len(0), _system_state_path(system_state_path),
		_ldisk(NULL) {
		assert(system_state_path);

		restore_system_state();
	}

	IOSystem::IOSystem(int blocks_amount, int block_size, const char *system_state_path)
		: _blocks_num(blocks_amount), _block_len(block_size), _system_state_path(system_state_path),
		_ldisk(NULL) {
		assert(blocks_amount > 0 && block_size > 0 && system_state_path);

		if (file_exists(_system_state_path)) {
			restore_system_state();
		}
		else {
			_ldisk = reserve_disk_space(_blocks_num, _block_len);
		}
	}

	IOSystem::~IOSystem() {
		save_system_state();
		free_disk_space(_ldisk, _blocks_num, _block_len);
		_ldisk = NULL;
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
		_ldisk = reserve_disk_space(_blocks_num, _block_len);

		for (int idx = 0; idx < _blocks_num; ++idx)
			fread(_ldisk[idx], sizeof(char) * _block_len, 1, file_ptr);

		fclose(file_ptr);
	}
} // namespace filesystem::io
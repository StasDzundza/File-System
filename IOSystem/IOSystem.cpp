#include "IOSystem.h"
// #define NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


namespace filesystem::io {
	void IOSystemInterface::_close_fs() {
		if (!_system_state_path)
			return;
		save_system_state();
	}
	bool IOSystemInterface::_file_exists(const char *path) const {
		struct stat buffer;
		return (stat(path, &buffer) == 0);
	}
	void IOSystemInterface::_init(const char * system_state_path) {
		assert(system_state_path);
		_system_state_path = system_state_path;
		if (_file_exists(_system_state_path)) {
			restore_system_state();
		}
	}
} // namespace filesystem::io
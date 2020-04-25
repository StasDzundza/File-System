#pragma once

#include <memory>
#include <utility>
#include <optional>

#include "oft.h"
#include "IOSystem/IOSystem.h"
#include "components/directory_entry.h"
#include "fs_config.h"
#include "components/file_descriptor.h"

namespace filesystem {
	using namespace config;
	using namespace components;
	class FileSystem {
	private:
		void initFileSystem();

		FileDescriptor getDescriptorByIndex(int fd_index);
		int readFromFile(OFTEntry* entry, void* write_ptr, int bytes);
	public:
		FileSystem();
	private:
		io::IOSystem ios;
		OFT oft;
	};
}

#include "filesystem.h"

#include <string.h>
namespace filesystem {
	FileSystem::FileSystem(){
		ios = std::make_unique<io::IOSystem>(constants::DISC_BLOCKS_NUM, constants::BLOCK_SIZE,"file1.conf");
		oft = std::make_unique<components::OFT>();

		initFileSystem();
	}

	/*
		Save filesystem data to disk
		write bitmap,directory descriptor,empty file descriptors
	*/
	void FileSystem::initFileSystem() {
		std::pair<int, int> res;

		// mark first k blocks as occupied
		int range = (1 << constants::DISC_BLOCKS_NUM + 1) - 1; 
		std::bitset<constants::DISC_BLOCKS_NUM> free_blocks_set(range);
		res = rawWriteToDisk(0, 0,  &free, sizeof(free_blocks_set));

		components::FileDescriptor f_dir;
		f_dir.file_length = 0;
		res = rawWriteToDisk(res.first, res.second, &f_dir, sizeof(f_dir));
	
		// TODO: HlibPylypets/Stas Dzundza Avoid using huge buf for descriptors
		char* main_mem_ptr = memory_buf;
		for(int i = 0; i < constants::FD_CREATED_LIMIT; ++i) {
			components::FileDescriptor f_descriptor;
			memcpy(main_mem_ptr, &f_descriptor, sizeof(f_descriptor));
			main_mem_ptr += sizeof(f_descriptor);
		}
		res = rawWriteToDisk(res.first, res.second, memory_buf, constants::FD_CREATED_LIMIT*sizeof(components::FileDescriptor));

		// Init directory ??!! TODO: Stas Dzundza
		// oft->addFile(0);
	}

	std::pair<int, int> FileSystem::rawWriteToDisk(int block_idx, int shift, void* read_ptr, int bytes) {
		char *read_from = static_cast<char*>(read_ptr);
		if (shift) {
			int prefix_size = std::min(constants::BLOCK_SIZE-shift, bytes);
	
			ios->read_block(block_idx, block_buf);
			memcpy(block_buf+shift, read_from, prefix_size);
			ios->write_block(block_idx, block_buf);

			read_from += prefix_size; bytes -= prefix_size;
			if(prefix_size != bytes) {
				shift = 0; block_idx += 1;
			} else {
				shift += prefix_size;
			}
		}

		if (bytes) {
			while (bytes >= constants::BLOCK_SIZE) {
				ios->write_block(block_idx, read_from);
				block_idx += 1;
				read_from += constants::BLOCK_SIZE;
				bytes -= constants::BLOCK_SIZE;
			}
			if (bytes) {
				memcpy(block_buf, read_from, bytes);
				ios->write_block(block_idx, block_buf);
				block_idx += 1;
			}
		}
		return {block_idx, shift};
	}
}

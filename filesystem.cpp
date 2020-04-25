
#include "filesystem.h"
#include "components/file_descriptor.h"
#include "utils/disk_utils.h"
#include "fs_config.h"

#include <cmath>
#include <bitset>
using std::min;

namespace filesystem {
	FileSystem::FileSystem() {
		ios.init(SYSTEM_PATH);
		initFileSystem();
	}

	FileDescriptor FileSystem::getDescriptorByIndex(int fd_index) {
		// compute the descriptor offset
		int bytes = sizeof(std::bitset<DISC_BLOCKS_NUM>) + sizeof(components::FileDescriptor)*fd_index;
		int read_block_idx = bytes/BLOCK_SIZE, shift = bytes % BLOCK_SIZE;
		FileDescriptor fd;

		disk_utils::RawDiskReader fin(&ios, read_block_idx, shift);
		fin.read(&fd, sizeof(FileDescriptor));
		return fd;
	}

	/*
		Save filesystem data to disk: write bitmap, 
		directory descriptor,empty file descriptors.
	*/
	void FileSystem::initFileSystem() {
		// TODO: Stas Dzundza
		// Add restoring fileSystem options from IOSystem
	
		disk_utils::RawDiskWriter fout(&ios, 0, 0);

		// mark first k first bits as occupied
		int range = (1 << SYSTEM_BLOCKS_NUM + 1) - 1; 
		std::bitset<DISC_BLOCKS_NUM> free_blocks_set(range);
		fout.write(&free_blocks_set, sizeof(free_blocks_set));

		// write directory descriptor to the disk
		FileDescriptor f_dir;
		f_dir.file_length = 0;
		fout.write(&f_dir, sizeof(f_dir));

		//write remaining empty descriptors to the disk
		FileDescriptor f_descriptor;
		for(int i = 0; i < FD_CREATED_LIMIT; ++i)
			fout.write(&f_descriptor, sizeof(f_descriptor));
		fout.flush();

		// Init directory ??!! TODO: Stas Dzundza
		// oft->addFile(0);
	}

	int FileSystem::readFromFile(OFTEntry* f_entry,  void* write_ptr, int bytes) {
		// Check if file has enough bytes to read
		FileDescriptor fd = getDescriptorByIndex(f_entry->fd_index);
		if (fd.file_length - f_entry->fpos < bytes) {
			return EXIT_FAILURE;
		}

		char* write_to = static_cast<char*>(write_ptr);
		int arr_block_idx = f_entry->fpos/BLOCK_SIZE, shift = f_entry->fpos % BLOCK_SIZE;
		if (!f_entry->block_read) {
			// if block isn't read yet, read the respective block
			ios.read_block(fd.arr_block_num[arr_block_idx], f_entry->read_write_buffer);
			f_entry->block_read = true; // block isn't modified for sure
		}

		if (shift) {
            int prefix_size = min(BLOCK_SIZE-shift, bytes);
			memcpy(write_to, f_entry->read_write_buffer + shift, prefix_size);
			write_to += prefix_size; bytes -= prefix_size;

			f_entry->fpos += prefix_size;
            shift = (shift + prefix_size) % BLOCK_SIZE;
            if(shift) {
                // bytes < BLOCK_SIZE - shift
				return EXIT_SUCCESS;
			}

			if (f_entry->block_modified) {
				// write the buffer into the appropriate block on disk (if modified),
				ios.write_block(fd.arr_block_num[arr_block_idx], f_entry->read_write_buffer);
				f_entry->block_modified = false;
			}
			arr_block_idx += 1; f_entry->block_read = false;
		}

		if (bytes) {
			// Left bytes to read
			while (bytes > BLOCK_SIZE) {
				// Here we may use external call where we pass f_entry->read_write_buffer
				ios.read_block(fd.arr_block_num[arr_block_idx], f_entry->read_write_buffer);
				f_entry->fpos += BLOCK_SIZE; arr_block_idx += 1;

				memcpy(write_to, f_entry->read_write_buffer, BLOCK_SIZE);
				write_to += BLOCK_SIZE; bytes -= BLOCK_SIZE;
			}
			// read remaining portion and buffer
			ios.read_block(fd.arr_block_num[arr_block_idx], f_entry->read_write_buffer);
			memcpy(write_to, f_entry->read_write_buffer, bytes);
			f_entry->fpos += bytes; f_entry->block_read = true;
		}
		return EXIT_SUCCESS;
	}
}


#include "filesystem.h"
#include "components/file_descriptor.h"
#include "utils/disk_utils.h"
#include "fs_config.h"

#include <cmath>
#include <bitset>
#include <vector>
using std::min;
using std::vector;

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

	int FileSystem::writeToFile(OFTEntry* entry, void* read_ptr, int bytes)
	{
		FileDescriptor fd = getDescriptorByIndex(entry->fd_index);
		if(fd.file_length + bytes < BLOCK_SIZE* MAX_FILE_BLOCKS) {
			return EXIT_FAILURE;
		}
		else {
			char* read_from = static_cast<char*>(read_ptr);
			int arr_block_idx = entry->fpos / BLOCK_SIZE, offset = entry->fpos % BLOCK_SIZE;
			if (!entry->block_read) {
				// if block isn't read yet, read the respective block
				ios.read_block(fd.arr_block_num[arr_block_idx], entry->read_write_buffer);
				entry->block_read = true; // block isn't modified for sure
			}

			int pos_diff = fd.file_length - entry->fpos;
			//calculate how many new space we need
			int new_bytes_need = bytes - pos_diff;
			int num_of_occupied_blocks = ceil(fd.file_length / BLOCK_SIZE);
			int size_of_occupied_blocks = num_of_occupied_blocks * BLOCK_SIZE;
			if (fd.file_length + new_bytes_need > size_of_occupied_blocks) {
				//allocate new space in disk if we don`t have enought
				bool new_blocks_allocated = allocateNewDiskBlocks(&fd, new_bytes_need);
				if (!new_blocks_allocated) {
					return EXIT_FAILURE;
				}
			}

			//if block space isn`t enought - write block to disk and read new
			while (bytes + offset > BLOCK_SIZE) {
				memcpy(entry->read_write_buffer + offset, read_from, BLOCK_SIZE - offset);
				bytes -= BLOCK_SIZE - offset;
				entry->fpos += BLOCK_SIZE - offset;
				read_from += BLOCK_SIZE - offset;
				offset = 0;
				ios.write_block(fd.arr_block_num[arr_block_idx], entry->read_write_buffer);
				arr_block_idx++;
				ios.read_block(fd.arr_block_num[arr_block_idx], entry->read_write_buffer);
			}
			//if block space is enought - write data to buffer and dont write to disk
			memcpy(entry->read_write_buffer + offset, read_from, bytes);
			entry->fpos += bytes;
			entry->block_read = true;
			entry->block_modified = true;
			if (entry->fpos > fd.file_length) {
				fd.file_length = entry->fpos;
			}
			return EXIT_SUCCESS;
		}
	}

	int FileSystem::allocateNewDiskBlocks(FileDescriptor* fd, int bytes)
	{
		//check if we have asked space
		if (fd->file_length + bytes <= BLOCK_SIZE * MAX_FILE_BLOCKS) {
			std::bitset<DISC_BLOCKS_NUM> free_blocks_set;
			disk_utils::RawDiskReader fin(&ios, 0, 0);
			fin.read(&free_blocks_set, sizeof(free_blocks_set));
			int offset_in_last_block = fd->file_length % BLOCK_SIZE;
			bytes -= (BLOCK_SIZE - offset_in_last_block);
			int num_of_occupied_blocks = ceil(fd->file_length / BLOCK_SIZE);
			//calculate num of new blocks that we need
			int num_of_new_blocks = ceil(bytes / BLOCK_SIZE); 
			vector<int>free_blocks_idx;
			for (int i = SYSTEM_BLOCKS_NUM; i < DISC_BLOCKS_NUM; i++) {
				if (free_blocks_idx.size() == num_of_new_blocks) {
					break;
				}
				else if (free_blocks_set[i] == 0) {
					free_blocks_idx.push_back(i);
				}
			}
			//if we found all blocks that we need - reserve it
			if (free_blocks_idx.size() == num_of_new_blocks) {
				for (int i : free_blocks_idx) {
					fd->arr_block_num[num_of_occupied_blocks++] = i;
					free_blocks_set[i] = 1;
				}
				disk_utils::RawDiskWriter fout(&ios, 0, 0);
				fout.write(&free_blocks_set, sizeof(free_blocks_set));
				fout.flush();
				return EXIT_SUCCESS;
			}
			else {
				return EXIT_FAILURE;
			}
		}
		else {
			return EXIT_FAILURE;
		}
	}
}

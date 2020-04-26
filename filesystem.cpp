
#include "filesystem.h"
#include "components/file_descriptor.h"
#include "utils/disk_utils.h"
#include "fs_config.h"

#include <cmath>
#include <bitset>
#include <vector>
using std::min;
using std::max;
using std::vector;

namespace filesystem {
	FileSystem::FileSystem() {
		ios.init(SYSTEM_PATH);
		_initFileSystem();
	}

	FileDescriptor FileSystem::_getDescriptorByIndex(int fd_index) {
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
	void FileSystem::_initFileSystem() {
		// TODO: Stas Dzundza
		// Add restoring fileSystem options from IOSystem
	
		disk_utils::RawDiskWriter fout(&ios, 0, 0);

		// mark first k first bits as occupied
		int range = (1 << SYSTEM_BLOCKS_NUM) - 1;
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

		oft.addFile(0);
	}

	int FileSystem::_readFromFile(OFTEntry* f_entry,  void* write_ptr, int bytes) {
		// Check if file has enough bytes to read
		FileDescriptor fd = _getDescriptorByIndex(f_entry->fd_index);
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

		// Left bytes to read
		while (bytes >= BLOCK_SIZE) {
			// Here we may use external call where we pass f_entry->read_write_buffer
			ios.read_block(fd.arr_block_num[arr_block_idx], f_entry->read_write_buffer);
			f_entry->fpos += BLOCK_SIZE; arr_block_idx += 1;

			memcpy(write_to, f_entry->read_write_buffer, BLOCK_SIZE);
			write_to += BLOCK_SIZE; bytes -= BLOCK_SIZE;
		}

		if (bytes) {
			// read remaining portion and buffer
			ios.read_block(fd.arr_block_num[arr_block_idx], f_entry->read_write_buffer);
			memcpy(write_to, f_entry->read_write_buffer, bytes);
			f_entry->fpos += bytes; f_entry->block_read = true;
		}
		return EXIT_SUCCESS;
	}

	int FileSystem::_writeToFile(OFTEntry* entry, void* read_ptr, int bytes)
	{
		FileDescriptor fd = _getDescriptorByIndex(entry->fd_index);
		// before reading/writing blocks, we must ensure the file can store requsted bytes
		// and allocate the necessary bytes
		int bytes_to_alloc = max(0, bytes - fd.file_length + entry->fpos);
		if (!_reserveBytesForFile(&fd, bytes_to_alloc)) {
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

			//if block space is enough - write data to buffer and don't write to disk
			memcpy(entry->read_write_buffer + offset, read_from, bytes);
			if (bytes == BLOCK_SIZE) {
				ios.write_block(fd.arr_block_num[arr_block_idx], entry->read_write_buffer);
				entry->block_read = false;
			} else {
				entry->block_read = true; entry->block_modified = true;
			}

			entry->fpos += bytes;
			if (entry->fpos > fd.file_length) {
				fd.file_length = entry->fpos;
			}
			return EXIT_SUCCESS;
		}
	}

	int FileSystem::_reserveBytesForFile(FileDescriptor* fd, int bytes)
	{
		//check if file can store requsted bytes
		if (fd->file_length + bytes <= BLOCK_SIZE * MAX_FILE_BLOCKS) {
			int offset_in_last_block = fd->file_length % BLOCK_SIZE;
			bytes -= (BLOCK_SIZE - offset_in_last_block);
			int num_of_occupied_blocks = ceil((double)fd->file_length / BLOCK_SIZE);
			//calculate num of new blocks that we need
			int num_of_new_blocks = ceil((double)bytes / BLOCK_SIZE);

			if (num_of_new_blocks == 0) {
				// No need to read the bitset, add any blocks
				return EXIT_SUCCESS;
			}

			std::bitset<DISC_BLOCKS_NUM> free_blocks_set;
			disk_utils::RawDiskReader fin(&ios, 0, 0);
			fin.read(&free_blocks_set, sizeof(free_blocks_set));

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

	std::pair<DirectoryEntry, int> FileSystem::_findFileInDirectory(char filename[MAX_FILENAME_LENGTH])
	{
		OFTEntry* dir_oft_entry = oft.findFile(0);
		FileDescriptor dir_fd = _getDescriptorByIndex(0);
		lseek(0, 0);
		int num_of_files_in_dir = dir_fd.file_length / sizeof(DirectoryEntry);
		int dir_entry_idx = 0;
		for (int i = 0; i < num_of_files_in_dir; i++) {
			DirectoryEntry cur_dir_entry;
			_readFromFile(dir_oft_entry, &cur_dir_entry, sizeof(DirectoryEntry));
			if (std::strcmp(cur_dir_entry.filename, filename) == 0) {
				dir_entry_idx = i;
				return std::make_pair(cur_dir_entry, dir_entry_idx);
			}
		}
		return std::make_pair(DirectoryEntry(), -1);
	}

	int FileSystem::createFile(char filename[MAX_FILENAME_LENGTH])
	{
		FileDescriptor dir_fd = _getDescriptorByIndex(0);
		int num_files_created = dir_fd.file_length / sizeof(DirectoryEntry);

		if (num_files_created == FD_CREATED_LIMIT || _findFileInDirectory(filename).second != -1)
			return EXIT_FAILURE;

		// compute the offset before looking for empty descriptor
		int bytes = sizeof(std::bitset<DISC_BLOCKS_NUM>) + sizeof(components::FileDescriptor);
		int block_idx = bytes / BLOCK_SIZE, offset = bytes % BLOCK_SIZE;
		disk_utils::RawDiskReader fin(&ios, block_idx, offset);

		FileDescriptor free_fd;
		int free_fd_index = 0;
		for (int i = 0; i < FD_CREATED_LIMIT; ++i) {
			fin.read(&free_fd, sizeof(components::FileDescriptor));

			if (free_fd.file_length == -1) {
				// file descriptor isn't occupie
				free_fd_index = i + 1;
				free_fd.file_length = 0;
				break;
			}
		}
		if (!free_fd_index) {
			// 0 free file descriptors found
			return EXIT_FAILURE;
		}

		DirectoryEntry new_entry(free_fd_index, filename);

		OFTEntry* dir = oft.findFile(0);
		lseek(0, num_files_created * sizeof(DirectoryEntry));

		_writeToFile(dir, &new_entry, sizeof(DirectoryEntry));
		return EXIT_SUCCESS;
	}

	int FileSystem::read(int fd_index, void* main_mem_ptr, int bytes) {
		OFTEntry* oft_ptr;
		if (bytes <= 0 || !(oft_ptr = oft.findFile(fd_index))) {
			return EXIT_FAILURE;
		}

		return _readFromFile(oft_ptr, main_mem_ptr, bytes);
	}

	int FileSystem::write(int fd_index, void* main_mem_ptr, int bytes) {
		OFTEntry* oft_ptr;
		if (bytes <= 0 || !(oft_ptr = oft.findFile(fd_index))) {
			return EXIT_FAILURE;
		}

		return _writeToFile(oft_ptr, main_mem_ptr, bytes);
	}

	int FileSystem::lseek(int fd_index, int pos)
	{
		return 0;
	}

	int FileSystem::open(char filename[MAX_FILENAME_LENGTH])
	{
		std::pair<DirectoryEntry, int> file = _findFileInDirectory(filename);
		if (file.second == -1) return -1;
		DirectoryEntry dir_entry = file.first;
		if (oft.addBlock(dir_entry.fd_index) == EXIT_FAILURE) return-1;
		int curr_pos = oft.addFile(dir_entry.fd_index);
		ios.read_block(_getDescriptorByIndex(dir_entry.fd_index).arr_block_num[0], oft.getFile(dir_entry.fd_index)->read_write_buffer);
		return dir_entry.fd_index;
	}
}


#include "filesystem.h"
#include "components/file_descriptor.h"
#include "utils/disk_utils.h"
#include "utils/file_utils.h"
#include "fs_config.h"

#include <bitset>
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

	void FileSystem::writeToFile(OFTEntry* entry, void* read_from, int bytes) {
		disk_utils::RawDiskReader fin(&ios, 0, 0);

		std::bitset<DISC_BLOCKS_NUM> free_blocks_set;
		fin.read(&free_blocks_set, sizeof(free_blocks_set));

		// TODO: Hlib Pylypets
		// Come up with the idea of writing to files
	}

	int FileSystem::createFile(char filename[MAX_FILENAME_LENGTH]) {
		FileDescriptor fd_dir = getDescriptorByIndex(0);
		int num_files_created = fd_dir.file_length / sizeof(DirectoryEntry);

		if (num_files_created == FD_CREATED_LIMIT || findFileInDir(filename))
			return EXIT_FAILURE;

		// compute the offset before looking for empty descriptor
		int bytes = sizeof(std::bitset<DISC_BLOCKS_NUM>) + sizeof(components::FileDescriptor);
		int block_idx = bytes/BLOCK_SIZE, shift = bytes % BLOCK_SIZE;
		disk_utils::RawDiskReader fin(&ios, block_idx, shift);
		
		FileDescriptor free_fd;
		int free_fd_index = 0;
		for(int i = 0; i < FD_CREATED_LIMIT; ++i) {
			fin.read(&free_fd, sizeof(components::FileDescriptor));

			if (free_fd.file_length == -1) {
				// file descriptor isn't occupie
				free_fd_index = i+1;
				free_fd.file_length = 0;
				break;
			}
		}
		if (!free_fd_index) {
			// Internal error
			return EXIT_FAILURE;
		}
		
		DirectoryEntry new_entry(free_fd_index, filename);

		OFTEntry* dir = oft.getFile(0);
		dir->fpos = num_files_created*sizeof(DirectoryEntry);

		writeToFile(dir, &new_entry, sizeof(DirectoryEntry));
		return EXIT_SUCCESS;
	}

	std::optional<DirectoryEntry> FileSystem::findFileInDir(char filename[MAX_FILENAME_LENGTH]) {
		OFTEntry* directory = oft.getFile(0);
		directory->fpos = 0;
		
		FileDescriptor fd_dir = getDescriptorByIndex(0);
		int num_files_created = fd_dir.file_length / sizeof(DirectoryEntry);
		file_utils::FileReader fin(&ios, directory, fd_dir);
	
		DirectoryEntry dir_entry;
		for(int i = 0; i < num_files_created; ++i) {
			fin.read(&dir_entry, sizeof(DirectoryEntry));

			if(!strncmp(dir_entry.filename, filename, MAX_FILENAME_LENGTH))
				return std::nullopt;
		}
		return dir_entry;
	}
}

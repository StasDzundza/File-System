#include "oft.h"

#include <algorithm>

namespace filesystem::components {
	OFTEntry::OFTEntry() :
		fpos(0), fd_index(0), 
		block_read(false), block_modified(false) {
	}

	OFTEntry* OFT::findFile(int fd_index){
		if (fd_index < 0 || fd_index > FD_OPENED_LIMIT)
			return nullptr;

		auto it = std::find_if(entries_buf.begin(), entries_buf.end(), 
			[fd_index](const OFTEntry& entry) {
				return entry.fd_index == fd_index;
		});

		if (it == entries_buf.end())
			return nullptr;
		return &*it;
	}

	OFTEntry* OFT::getFile(int oft_index) {
		return &entries_buf[oft_index];
	}

	int OFT::addFile(int fd_index)
	{
		//file exists or no space in oft
		if (oft_size >= FD_OPENED_LIMIT || findFile(fd_index)!= nullptr) {
			return EXIT_FAILURE;
		}
		OFTEntry new_file_entry;
		new_file_entry.fd_index = fd_index;
		entries_buf[oft_size++] = new_file_entry;
		return EXIT_SUCCESS;
	}

	int OFT::removeFile(int fd_index)
	{
		auto it = std::remove_if(entries_buf.begin(), entries_buf.end(), [fd_index](const OFTEntry& file_entry) {
			return file_entry.fd_index == fd_index;
		});

		if (it != entries_buf.end()) {
			oft_size--;
			return EXIT_SUCCESS;
		}
		else {
			return EXIT_FAILURE;
		}
	}

	int OFT::getNumOFOpenFiles()
	{
		return oft_size;
	}
}


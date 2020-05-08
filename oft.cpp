#include "oft.h"

#include <algorithm>

namespace filesystem::components {
	OFTEntry::OFTEntry() :
		fpos(0), fd_index(-1),
		block_read(false), block_modified(false) {
	}

	int OFT::getOftIndex(int fd_index){
		if (fd_index < 0 || fd_index > FD_OPENED_LIMIT)
			return -1;

		auto it = std::find_if(entries_buf.begin(), entries_buf.end(), 
			[fd_index](const OFTEntry& entry) {
				return entry.fd_index == fd_index;
		});

		if (it == entries_buf.end())
			return -1;
		return it - entries_buf.begin();
	}

	OFTEntry* OFT::getFile(int oft_index) {
		return &entries_buf[oft_index];
	}

	int OFT::addFile(int fd_index)
	{
		//file exists or no space in oft
		if (oft_size >= FD_OPENED_LIMIT || getOftIndex(fd_index) >= 0) {
			return -1;
		}
		OFTEntry new_file_entry;
		new_file_entry.fd_index = fd_index;
		entries_buf[oft_size++] = new_file_entry;
		return oft_size-1;
	}

	void OFT::removeOftEntry(int oft_index) {
		std::swap(entries_buf[oft_index], entries_buf[oft_size - 1]);
		oft_size -= 1;
	}

	int OFT::getNumOFOpenFiles()
	{
		return oft_size;
	}
}


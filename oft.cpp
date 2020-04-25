#include "oft.h"

#include <algorithm>

namespace filesystem::components {
	OFTEntry::OFTEntry() :
		fpos(0), fd_index(0), 
		block_read(false), block_modified(false) {
	}

	OFTEntry* OFT::findFile(int fd_index){
		if (fd_index <= 0 || fd_index > FD_OPENED_LIMIT)
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
}


#include "oft.h"

#include <algorithm>

namespace filesystem::components {
	OFT::OFTEntry* OFT::findFile(int fd_index){
		auto it = std::find_if(entries_buf.begin(), entries_buf.end(), 
			[fd_index](const OFTEntry& entry) {
				return entry.fd_index == fd_index;
		});

		if (it == entries_buf.end())
			return nullptr;
		return &*it;
	}

	OFT::OFTEntry* OFT::getFile(int opened_fd_index) {
		return &entries_buf[opened_fd_index];
	}
}


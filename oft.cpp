#include "oft.h"

#include <algorithm>

namespace filesystem::components {
	OFTEntry* OFT::findFile(int fd_index){
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


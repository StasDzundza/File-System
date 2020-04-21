#pragma once
#include <vector>
#include "file_system_constants.h"

namespace filesystem::components {
	class OFT {
	public:

		class OFTEntry {
		public:
			OFTEntry() = default;

			OFTEntry(const OFTEntry& other_oft_entry);

			OFTEntry& operator=(const OFTEntry& other_oft_entry);

			char* getReadWriteBuffer();

			int getPosition() const noexcept;
			void setPosition(int new_position) noexcept;

			int getDescriptorIndex()const noexcept;
			void setDescriptorIndex(int descriptor_index) noexcept;

		private:
			char read_write_buffer[constants::DISC_BLOCK_SIZE];
			int position = 0;
			int descriptor_index = 0;
		};

		OFTEntry& getFile(int index);

		int addFile(int file_descriptor);

		OFTEntry* findFile(int descriptor_index);

	private:
		std::vector<OFTEntry> oft;
	};
}
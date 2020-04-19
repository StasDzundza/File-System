#pragma once
#include <vector>
#include "file_system_constants.h"
namespace filesystem::components {
	class OFT {
	public:

		class OFTEntry {
		public:
			OFTEntry();

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

		OFT();

		OFTEntry& operator[](int index);

		OFTEntry& getFile(int index);

		void addFile(int file_descriptor);

	private:

		std::vector<OFTEntry> oft;
	};
}
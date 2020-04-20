#include "oft.h"
#include <stdexcept>
#include <algorithm>

namespace filesystem::components {
	OFT::OFT() {
	}

	void OFT::addFile(int file_descriptor){
		if (oft.size() < constants::MAX_NUMBER_OF_OPEN_FILES + 1) {
			OFTEntry file;
			file.setDescriptorIndex(file_descriptor);
			oft.push_back(file);
		}
		else {
			throw std::logic_error("There are no space in OFT. Close some files.");
		}
	}

	bool OFT::findFile(int descriptor_index){
		auto it = std::find_if(oft.begin(), oft.end(), [descriptor_index](const OFTEntry& entry) {
			if (entry.getDescriptorIndex() == descriptor_index) {
				return true;
			}
			else {
				return false;
			}
		});
		return it != oft.end();
	}

	OFT::OFTEntry& OFT::operator[](int index)
	{
		if (index < oft.size()) {
			return oft[index];
		}
		else {
			throw std::logic_error("Index out of bound");
		}
	}

	OFT::OFTEntry& OFT::getFile(int index)
	{
		if (index < oft.size()) {
			return oft[index];
		}
		else {
			throw std::logic_error("Index out of bound");
		}
	}

	OFT::OFTEntry::OFTEntry(const OFTEntry& other_oft_entry)
	{
		for (int i = 0; i < filesystem::constants::DISC_BLOCK_SIZE; i++) {
			read_write_buffer[i] = other_oft_entry.read_write_buffer[i];
		}
		this->position = other_oft_entry.position;
		this->descriptor_index = other_oft_entry.descriptor_index;
	}

	OFT::OFTEntry& OFT::OFTEntry::operator=(const OFTEntry& other_oft_entry)
	{
		for (int i = 0; i < filesystem::constants::DISC_BLOCK_SIZE; i++) {
			read_write_buffer[i] = other_oft_entry.read_write_buffer[i];
		}
		this->position = other_oft_entry.position;
		this->descriptor_index = other_oft_entry.descriptor_index;
		return *this;
	}

	char* OFT::OFTEntry::getReadWriteBuffer()
	{
		return read_write_buffer;
	}

	int OFT::OFTEntry::getPosition() const noexcept
	{
		return position;
	}

	void OFT::OFTEntry::setPosition(int new_position) noexcept
	{
		position = new_position;
	}

	int OFT::OFTEntry::getDescriptorIndex() const noexcept
	{
		return descriptor_index;
	}

	void OFT::OFTEntry::setDescriptorIndex(int descriptor_index) noexcept {
		this->descriptor_index = descriptor_index;
	}
}


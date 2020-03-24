#include "oft.h"
#include <stdexcept>

OFT::OFT(){
	//directory always opens on start with index 0;
	OFTEntry directory;
	oft.push_back(directory);
}

OFT::OFTEntry::OFTEntry() {

}

OFT::OFTEntry& OFT::operator[](const int& index)
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

void OFT::OFTEntry::setPosition(const int& new_position) noexcept
{
	position = new_position;
}

int OFT::OFTEntry::getDescriptorIndex() const noexcept
{
	return descriptor_index;
}

void OFT::OFTEntry::setDescriptorIndex(const int& descriptor_index) noexcept{
	this->descriptor_index = descriptor_index;
}



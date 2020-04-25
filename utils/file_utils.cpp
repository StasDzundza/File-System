#include "file_utils.h"

#include <cmath>
#include <string.h>
using std::min;

#include "../IO_system/IOSystem.h"
#include "../oft.h"
#include "../components/file_descriptor.h"
#include "../fs_config.h"

namespace filesystem::file_utils {
    using namespace config;
    using namespace components;
    FileReader::FileReader(io::IOSystem* ios, OFTEntry* file_entry, FileDescriptor fd) :
        ios(ios), file_entry(file_entry), fd(fd) {
        block_read = false;
        array_block_idx = file_entry->fpos/BLOCK_SIZE;
        shift = file_entry->fpos % BLOCK_SIZE;
        block_buf = file_entry->read_write_buffer;
    }

    void FileReader::read(void* write_ptr, int bytes) {
        char* write_to = static_cast<char*>(write_ptr);
		if (shift) {
            if (!block_read) {
                ios->read_block(fd.arr_block_num[array_block_idx], block_buf);
                block_read = true;
            }

            int prefix_size = min(BLOCK_SIZE-shift, bytes);
			memcpy(write_to, block_buf + shift, prefix_size);

            file_entry->fpos += prefix_size;
            shift = (shift + prefix_size) % BLOCK_SIZE;
            if(shift) {
                // bytes < BLOCK_SIZE - shift
                return;
            }
            write_to += prefix_size; bytes -= prefix_size;
            array_block_idx += 1; block_read = false;
		}

        if (bytes) {
            while (bytes >= BLOCK_SIZE) {
                ios->read_block(fd.arr_block_num[array_block_idx], write_to);
                array_block_idx += 1;
                write_to += BLOCK_SIZE; bytes -= BLOCK_SIZE;
            }
        }

        if (bytes) {
            ios->read_block(fd.arr_block_num[array_block_idx], block_buf);
            block_read = true;
            memcpy(block_buf, write_to, bytes);
            shift = bytes;
        }
    }

    void FileReader::clear() {
        block_read = false;
    }
}
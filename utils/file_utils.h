#pragma once

namespace filesystem::io {
    class IOSystem;
}
namespace filesystem::file_utils {
    class FileReader {
    public:
        FileReader(io::IOSystem* ios, OFTEntry* file_entry, FileDescriptor fd);

        void read(void* write_ptr, int bytes);
        void clear();
    private:
        io::IOSystem* ios;
        OFTEntry* file_entry;
        FileDescriptor fd;

        bool block_read;
        int array_block_idx, shift;
        char* block_buf;
    };
} 
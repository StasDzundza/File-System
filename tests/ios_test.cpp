
#include "../IOSystem/IOSystem.h"

#include <cassert>
#include <cstring>
#include <set>
#include <iostream>
using namespace filesystem;
using namespace std;

void test1() {
    // filesystem not initialized
    io::IOSystem ios;
}

void test2() {
    // path to file don't exist
    io::IOSystem ios;
    ios.init("dummy_path/");
}

class TestIOSystem : public io::IOSystem {
public:
    int getBlocksNum() {
        return _blocks_num;
    }
    int getBlocksSize() {
        return _block_len;
    }
    auto getDisk() {
        return &_ldisk;
    }
    bool operator== (const TestIOSystem& other) {
        return _block_len == other._block_len &&_blocks_num == other._blocks_num
            && memcmp(&_ldisk, &other._ldisk, sizeof(_ldisk)) == 0;
    }
};

void test3() {
    char write_buf[config::BLOCK_SIZE] = {0};
    char read_buf[config::BLOCK_SIZE] = {0};

    char msg[] = "THis is a simple message";
    memcpy(write_buf, msg, sizeof(msg));

    char msg2[] = "20 len msg";
    memcpy(write_buf + sizeof(msg), msg2, sizeof(msg2));

    set<int> write_to_idx;
    // empty fs after writing/reading state
    {
        TestIOSystem ios;
        ios.init("data2.txt");
        int block_num = 5 + rand() % (config::DISC_BLOCKS_NUM-10)/2;
        for(int i = 0; i < block_num; ++i) {
            int block_idx = rand() % config::DISC_BLOCKS_NUM;
            ios.write_block(block_idx, write_buf);
            write_to_idx.insert(block_idx);

            ios.read_block(block_idx, read_buf);
            assert (memcmp(read_buf, write_buf, sizeof(read_buf)) == 0);
        }
    }
    TestIOSystem new_ios;
    new_ios.init("data2.txt");

    for (int index : write_to_idx) {
        new_ios.read_block(index, read_buf);
        assert (memcmp(read_buf, write_buf, sizeof(read_buf)) == 0);
    }
}

int main() {
    test3();
    return 0;
}

// 0 - success
// 1 - object too large to fit in current file
// 2 - internal error
namespace filesystem::core {
int append_to_file(file_descriptor &fobj, char const *read_from, int bytes) {
  if (bytes > fs_config.max_file_length - fobj.file_length)
    return 1;

  int fpos = fobj.file_length % fs_config.block_size,
      last_block_idx = fobj.file_length / fs_config.block_size;
  if (fpos > 0) {
    // last block is partially filled
    int disk_write_block = fobj.blocks[last_block_idx];
    char buf[fs_config.block_size]; // this is ok, since block can be allocated
                                    // in RAM, otherwise the block_size is
                                    // chosen wrong
    ios->read_block(disk_write_block, buf); // read last file block to the buf
    int portion_size = min(fs_config.block_size - fpos,
                           bytes); // compute the allowed front portion in bytes

    memcpy(buf + fpos, read_from,
           portion_size); // append portion of data to the buf
    read_from += portion_size, bytes -= portion_size;

    ios->write_block(disk_write_block, buf); // put buf to the disk
    fobj.file_length += portion_size, last_block_idx += 1;
  }

  int err = 0;
  if (bytes) {
    // here we know for sure that the file is full, and we still need to write
    // bytes to it hence we need to allocate additional memory!!!
    err = allocate_memory(fobj,
                          bytes); // this method is responsible for reporting
                                  // if such memory can't be allocated
    if (!err) {
      while (bytes >= fs_config.block_size) {
        int disk_write_block = fobj.blocks[last_block_idx];
        ios->write_block(disk_write_block, read_from);
        read_from += fs_config.block_size, bytes -= fs_config.block_size;
        fobj.file_length += fs_config.block_size, last_block_idx += 1;
      }
    } else {
      err = 2;
    }
  }

  if (bytes && !err) {
    // here we just need to write the remainding portion of bytes to the last
    // block Again, memory is already allocated
    char buf[fs_config.block_size];
    memcpy(buf, read_from, bytes);
    ios->write_block(fobj.blocks[last_block_idx], buf);
    fobj.file_length += bytes;
  }
  return err;
}
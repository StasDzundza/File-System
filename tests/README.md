# Tests
* t1: create file. check was created and is empty
* t2: create - delete. check was deleted
* t3: delete not exist file. check is error
* t4: create a lot of files. check 24 files was created, 25 - fail
* t5: open a lot of files. check 15 files was opened, 16 - fail
* t6: write(less 1 block), close, open, read. check correct writing, reading
* t7: write(more 1 block), close, open, read. check correct writing, reading
* t8: write, seek, write, seek, reed all. check correct writing, reading, seeking
* t9: write overflow. check error
* t10: open twice. check return same oft index
* t11: write(more 1 block), seek, read. check correct lazy seek when read
* t12: write(more 1 block), seek, close, open read. check correct lazy seek when close file
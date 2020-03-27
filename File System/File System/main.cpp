#include <iostream>
#include "filesystem.h"

int main() {
	filesystem::FileSystem filesystem;
	filesystem.create_file("myf2");
	filesystem.create_file("myf1");
	return 0;
}
#include <iostream>
#include "filesystem.h"
#include "directory.h"
int main() {
	filesystem::FileSystem filesystem;
	char arr[4]{ 'a','b','c','\0' };
	filesystem.createFile(arr);
	filesystem.createFile(arr);
	
	
	return 0;
}
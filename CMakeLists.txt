cmake_minimum_required(VERSION 3.10)
project(File_System)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)
include_directories(components)
include_directories(IOSystem)
include_directories(utils)

add_executable(File_System
        components/directory_entry.h
        components/file_descriptor.h
        IOSystem/CHSSystem.cpp
        IOSystem/CHSSystem.h
        IOSystem/io_config.h
        IOSystem/IOSystem.cpp
        IOSystem/IOSystem.h
        IOSystem/LBASystem.cpp
        IOSystem/LBASystem.h
        utils/disk_utils.cpp
        utils/disk_utils.h
        utils/errors.h
        filesystem.cpp
        filesystem.h
        fs_config.h
        input_reader.cpp
        input_reader.h
        main.cpp
        oft.cpp
        oft.h)

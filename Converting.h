#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


#define _int1 unsigned char
#define _int2 unsigned short int
#define _int4 unsigned int
#define _int8 unsigned long long

#define media_byte_hard_disk 0xF8
#define media_byte_removable_disk 0xF0

#define INT_1 0x0001
#define INT_2 0x0002
#define INT_4 0x0004
#define INT_8 0x0008
#define INT_16 0x0010
#define INT_32 0x0020
#define INT_64 0x0040
#define INT_128 0x0080
#define INT_512 0x0200
#define INT_1024 0x0400
#define INT_2048 0x0800
#define INT_4096 0x1000


// reflecting
_int1 reflect_int1(unsigned char byte)
{
    _int1 temp = 0;
    temp += (_int1)(byte);
    return temp;
}

_int2 reflect_int2(unsigned char byte[2])
{
    _int2 temp = 0;
    temp += (_int2)(byte[1] << 8);
    temp += (_int2)(byte[0]);
    return temp;
}

_int4 reflect_int4(unsigned char byte[4])
{
    _int4 temp = 0;
    temp += (_int4)(byte[3] << 24);
    temp += (_int4)(byte[2] << 16);
    temp += (_int4)(byte[1] << 8);
    temp += (_int4)byte[0];
    return temp;
}

void error_open_device(const char* file_path) {
    printf("\n \'%s\' : No such directory, or cannot open your device for reading. Please check Your device and try again\n", file_path);
    exit(0);
}

void error_read_device() {
    printf("\n Cannot read memory from Your device. \n");
    exit(0);
}

int open_device(const char* device_path)
{
    int current_device_descriptor = open(device_path, O_RDONLY);
    if (current_device_descriptor == -1){
        error_open_device(device_path);
    }
    return current_device_descriptor;
    return 1;
}

void seek_descriptor(int device_descriptor, _int8 offset)
{
    if (lseek(device_descriptor, offset, 0) == -1L){
        error_read_device();
    }
}

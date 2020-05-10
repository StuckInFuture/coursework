#pragma once
#include "Converting.h"
#include <stdlib.h>



class FAT {
public:
    char** FATs;
    void read_FAT(int device_descriptor, _int8 offset_first_FAT, _int2 mirror_flags, _int1 number_FAT_copies, _int8 count_bytes_FAT) {
        int current_number_FAT = number_FAT_copies;
        int current_flag = (mirror_flags & 0x0080);
        int is_simple_FAT = 0;
        int isFind = 0;
        if (current_flag) {
            current_number_FAT = 1; // 7-th bit of flag meams that active is only one FAT, if 7-th bit = zero, all FATs are active
            is_simple_FAT = 1;
            if ((mirror_flags & 0x0001) > 0) {
                isFind++;
            }
            if ((mirror_flags & 0x0002) > 0) {
                isFind++;
                offset_first_FAT += count_bytes_FAT;
            }
            if ((mirror_flags & 0x0004) > 0) {
                isFind++;
                offset_first_FAT += 2 * count_bytes_FAT;
            }
            if ((mirror_flags & 0x0008) > 0) {
                isFind++;
                offset_first_FAT += 3 * count_bytes_FAT;
            }
        }
        if (is_simple_FAT && (isFind == 0 || isFind > 1)) {
            this->error_mirror_flags();
            return;
        }

        this->FATs = (char**)malloc(current_number_FAT * sizeof(char*));

        if (!FATs) {
            this->error_memory_FAT();
        }

        for (int i = 0; i < current_number_FAT; i++) {
            this->FATs[i] = (char*)malloc(count_bytes_FAT * sizeof(char));
            if (!(this->FATs[i])) {
                this->error_memory_FAT();
                free(this->FATs);
                return;
            }
        }

        if (lseek(device_descriptor, offset_first_FAT, 0) == -1L){
            this->error_mirror_flags();
            return;
        }

        for (int i = 0; i < current_number_FAT; i++){
            if (read(device_descriptor, this->FATs[i], count_bytes_FAT) == 0){
                this->error_mirror_flags();
                return;
            }
        }
    }
    void error_mirror_flags() {
        printf("ERROR: error with finding or reading FAT\n");
        exit(0);
    }
    void error_FATs() {
        printf("ERROR: error with FATs, not equal\n");
    }
    void error_memory_FAT() {
        printf("ERROR: cannot allocate memory for FATs\n");
        exit(0);
    }
    _int1 number_of_FATs(_int2 mirror_flags, _int1 number_FAT_copies) {
        int current_number_FAT = number_FAT_copies;
        int current_flag = (mirror_flags & 0x0080);
        if (current_flag) {
            current_number_FAT = 1; // 7-th bit of flag meams that active is only one FAT, if 7-th bit = zero, all FATs are active
        }
        return current_number_FAT;
    }
    int check_equal_FAT(_int1 number_of_FATs, _int8 size_of_FAT) {
        for (int i = 0; i < number_of_FATs - 1; i++) {
            for (int j = 0; j < size_of_FAT; j++) {
                if (this->FATs[i][j] != this->FATs[i + 1][j]) {
                    this->error_FATs();
                    return 0;
                }
            }

        }
        return 1;
    }
};

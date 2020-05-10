#pragma once
#include "Converting.h"

const _int4 correct_first_signature = 0x41615252;
const _int4 correct_second_signature = 0x61417272;
const _int4 correct_third_signature = 0xAA550000;
const _int4 correct_FSInfo_cluster = 0xFFFFFFFF;

#define FSINFO_SIZE 512

class FSInfo{
public:
    unsigned char first_signature[4];
    unsigned char first_reserved[480];
    unsigned char second_signature[4];
    unsigned char free_cluster_count[4];
    unsigned char next_free_cluster[4];
    unsigned char second_reserved[12];
    unsigned char third_signature[4];

public:
    FSInfo() {};
    ~FSInfo() = default;
    void read_FSInfo(int current_device_descriptor) {
        int current_size = read(current_device_descriptor, this, sizeof(struct FSInfo));
       if (current_size == -1){
           error_read_device();
       }
    }

    void output_FSInfo() {
        printf("\n");
        printf("%d - free clusters\n", reflect_int4(this->free_cluster_count));
        printf("0x%x - next free cluster\n", reflect_int4(this->next_free_cluster));
        printf("\n");
    }

    int check_correct_FSInfo() {
        if (reflect_int4(this->first_signature) != correct_first_signature) {
            this->output_FSInfo_error();
            return 0;
        }
        if (reflect_int4(this->second_signature) != correct_second_signature) {
            this->output_FSInfo_error();
            return 0;
        }
        if (reflect_int4(this->third_signature) != correct_third_signature) {
            this->output_FSInfo_error();
            return 0;
        }
        return 1;
    }

    int check_equal_FSInfo(FSInfo second_FSInfo) {
        _int4 first_total_clusters = reflect_int4(this->free_cluster_count);
        _int4 second_total_clusters = reflect_int4(second_FSInfo.free_cluster_count);
        _int4 first_next_cluster = reflect_int4(this->next_free_cluster);
        _int4 second_next_cluster = reflect_int4(second_FSInfo.next_free_cluster);
        if (first_total_clusters != correct_FSInfo_cluster && second_total_clusters != correct_FSInfo_cluster &&
            first_total_clusters != second_total_clusters) {
            this->warning_FSInfo();
            return 0;
        }
        if (first_next_cluster != correct_FSInfo_cluster && second_next_cluster != correct_FSInfo_cluster &&
            first_next_cluster != second_next_cluster) {
            this->warning_FSInfo();
            return 0;
        }
        return 1;
    }

    void output_FSInfo_error() {
        printf("ERROR in FSInfo. Signatures is not correctly!\n");
    }

    void warning_FSInfo() {
        printf("WARNING in FSInfo. Main FSInfo does not match backup FSInfo!\n");
    }

};



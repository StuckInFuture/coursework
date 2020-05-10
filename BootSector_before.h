#ifndef BootSector_H
#define BootSector_H

#include <string.h>
#include "Converting.h"



class BootSector_before{
public:
    unsigned char jump_to_bootstrap[3];                          // 0-2
    unsigned char OEM_name[8];                                   // 3-10
    unsigned char bytes_per_sector[2];                           // 11-12
    unsigned char sectors_per_cluster;                           // 13
    unsigned char number_reserved_sectors[2];                    // 14-15
    unsigned char number_FAT_copies;                             // 16
    unsigned char number_root_entries[2];                        // 17-18
    unsigned char total_number_sectors_unused[2];                // 19-20 FOR FAT 16 and FAT 12
    unsigned char media_descriptor_type;                         // 21
    unsigned char number_of_sectors_per_FAT_unused[2];           // 22-23 0000 for FAT32, FOR FAT 16 and FAT 12
    unsigned char number_of_sectors_per_track[2];                // 24-25
    unsigned char number_of_heads[2];                            // 26-27
    unsigned char number_of_hidden_sectors[4];                   // 28-31
    unsigned char total_number_of_sectors[4];                    // 32-35
    unsigned char number_of_sectors_per_FAT[4];                  // 36-39
    unsigned char mirror_flags[2];                               // 40-41  0-3: num of active FAT(bit 7 is 1) 4-6:reserved
                                                                 // 7: 1 - single active FAT, 0 - all FATs are updated at runtime  8-15: reserved
    unsigned char filesystem_version[2];                         // 42-43
    unsigned char first_cluster_root_directory[4];               // 44-47
    unsigned char filesystem_information_sector[2];              // 48-49
    unsigned char backup_boot_sector_location[2];                // 50-51
    unsigned char reserved[12];                                  // 52-63
    unsigned char logical_drive_number;                          // 64
    unsigned char reserved_for_WindowsNT;                        // 65
    unsigned char extended_signature_0x29;                       // 66
    unsigned char serial_partition_number[4];                    // 67-70
    unsigned char volume_label[11];                              // 71-81
    unsigned char filesystem_type[8];                            // 82-89
    unsigned char bootstrap_data[420];                           // 90-509
    unsigned char signature_55AA[2];                             // 510-511
public:
    void read_boot_sector(int current_device_descriptor) {
        int current_size = read(current_device_descriptor, this, 512);
        if (current_size == -1){
            error_read_device();
        }
    }

    int check_equal_boot_sector(BootSector_before second_boot_sector) {
        second_boot_sector.reserved_for_WindowsNT = this->reserved_for_WindowsNT;
        if (memcmp(this, &second_boot_sector, 512) != 0) {
            this->warning_boot_sector();
            return 1;
        }
        return 0;
    }

    void warning_boot_sector() {
        printf("WARNING in boot sector. Main boot sector and backup boot sector are not equal!\n");
    }
};

#endif BootSector_H

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

#include "FSInfo.h"
#include "BootSector_after.h"
#include "BootSector_before.h"
#include "FAT.h"

int full_information = 0;

void output_full_information(BootSector_after current_boot_sector, FSInfo current_FSInfo)
{
    current_boot_sector.output_boot_sector();
    current_FSInfo.output_FSInfo();
}

void check_command(int argc, char const *argv[])
{
    for (int i = 1; i < argc; i++){
        if (argv[i][1] == 'f'){
            full_information = 1;
        }
    }
}

int main(int argc, char const *argv[]) {
    check_command(argc, argv);

    const char * device_path = argv[1];

    int current_device_descriptor = open_device(device_path);

    BootSector_before current_boot_sector_before;
    current_boot_sector_before.read_boot_sector(current_device_descriptor);


    printf("\n%d\n",sizeof(current_boot_sector_before));


    BootSector_after current_boot_sector_after;
    current_boot_sector_after.converting_boot_sector(current_boot_sector_before);

    FSInfo current_FSInfo;
    current_FSInfo.read_FSInfo(current_device_descriptor);

    if (full_information){
        output_full_information(current_boot_sector_after, current_FSInfo);
    }

    current_boot_sector_after.check_FAT_32();
    current_FSInfo.check_correct_FSInfo();

    seek_descriptor(current_device_descriptor, current_boot_sector_after.backup_boot_sector_location * current_boot_sector_after.bytes_per_sector);

    BootSector_before backup_boot_sector_before;
    backup_boot_sector_before.read_boot_sector(current_device_descriptor);

    FSInfo backup_FSInfo;
    backup_FSInfo.read_FSInfo(current_device_descriptor);

    current_boot_sector_after.check_boot_sector();
    current_boot_sector_before.check_equal_boot_sector(backup_boot_sector_before);
    current_FSInfo.check_equal_FSInfo(backup_FSInfo);

    FAT currentFAT;
    currentFAT.read_FAT(current_device_descriptor,
        current_boot_sector_after.number_reserved_sectors * current_boot_sector_after.bytes_per_sector,
        current_boot_sector_after.mirror_flags,
        current_boot_sector_after.number_FAT_copies,
        current_boot_sector_after.number_of_sectors_per_FAT * current_boot_sector_after.bytes_per_sector);

    currentFAT.check_equal_FAT(currentFAT.number_of_FATs(current_boot_sector_after.mirror_flags, current_boot_sector_after.number_FAT_copies),
        current_boot_sector_after.number_of_sectors_per_FAT * current_boot_sector_after.bytes_per_sector);

    return 0;
}

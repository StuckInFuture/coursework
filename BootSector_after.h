#pragma once

#include "Converting.h"
#include "BootSector_before.h"

class BootSector_after {
public:
    unsigned char OEM_name[8];                        // 3-10
    _int2 bytes_per_sector;                           // 11-12
    _int1 sectors_per_cluster;                        // 13
    _int2 number_reserved_sectors;                    // 14-15
    _int1 number_FAT_copies;                          // 16
    _int2 number_root_entries;                        // 17-18
    _int1 media_descriptor_type;                      // 21
    _int2 number_of_sectors_per_track;                // 24-25
    _int2 number_of_heads;                            // 26-27
    _int4 number_of_hidden_sectors;                   // 28-31
    _int4 total_number_of_sectors;                    // 32-35
    _int4 number_of_sectors_per_FAT;                  // 36-39
    _int2 mirror_flags;                               // 40-41
    _int4 first_cluster_root_directory;               // 44-47
    _int2 backup_boot_sector_location;                // 50-51
    _int2 signature_55AA;                             // 510-511
public:
    BootSector_after() {};
    ~BootSector_after() = default;
    void converting_boot_sector(BootSector_before boot_sector_before) {
        for (int i = 0; i < 8; i++) {
            this->OEM_name[i] = boot_sector_before.OEM_name[i];
        }
        this->bytes_per_sector = reflect_int2(boot_sector_before.bytes_per_sector);
        this->sectors_per_cluster = reflect_int1(boot_sector_before.sectors_per_cluster);
        this->number_reserved_sectors = reflect_int2(boot_sector_before.number_reserved_sectors);
        this->number_FAT_copies = reflect_int1(boot_sector_before.number_FAT_copies);
        this->number_root_entries = 32;
        this->media_descriptor_type = reflect_int1(boot_sector_before.media_descriptor_type);
        this->number_of_sectors_per_track = reflect_int2(boot_sector_before.number_of_sectors_per_track);
        this->number_of_heads = reflect_int2(boot_sector_before.number_of_heads);
        this->number_of_hidden_sectors = reflect_int4(boot_sector_before.number_of_hidden_sectors);
        this->total_number_of_sectors = reflect_int4(boot_sector_before.total_number_of_sectors);
        this->number_of_sectors_per_FAT = reflect_int4(boot_sector_before.number_of_sectors_per_FAT);
        this->mirror_flags = reflect_int2(boot_sector_before.mirror_flags);
        this->first_cluster_root_directory = reflect_int4(boot_sector_before.first_cluster_root_directory);
        this->backup_boot_sector_location = reflect_int2(boot_sector_before.backup_boot_sector_location);
        this->signature_55AA = reflect_int2(boot_sector_before.signature_55AA);
    }
    void output_boot_sector() {
        printf("System ID \"%s\"\n", this->OEM_name);
        printf("Media byte \"%x\"\n", this->media_descriptor_type);
        if (this->media_descriptor_type == media_byte_hard_disk) {
            printf("Hard disk\n");
        }
        if (this->media_descriptor_type == media_byte_removable_disk) {
            printf("Removable disk\n");
        }
        printf("%d bytes per logical sector\n", this->bytes_per_sector);
        printf("%lld bytes per cluster\n", (_int8)(this->sectors_per_cluster * this->bytes_per_sector));
        printf("%d reserved sectors\n", this->number_reserved_sectors);
        printf("First FAT starts at byte %lld (sector %d)\n",
            (_int8)(this->number_reserved_sectors * this->bytes_per_sector),
            this->number_reserved_sectors
        );
        printf("%d FATs, %d bit entries\n", this->number_FAT_copies, this->number_root_entries);
        printf("%lld bytes per FAT (%d sectors)\n",
            (_int8)(this->number_of_sectors_per_FAT * this->bytes_per_sector),
            this->number_of_sectors_per_FAT
        );
        printf("Root directory start at cluster %d\n", this->first_cluster_root_directory);
        _int8 data_area_sector = (this->number_FAT_copies * this->number_of_sectors_per_FAT) +
            this->number_reserved_sectors;
        _int8 data_clusters = (this->total_number_of_sectors - this->number_FAT_copies *
            this->number_of_sectors_per_FAT - this->number_reserved_sectors) /
            this->sectors_per_cluster;
        printf("Data area starts at byte %lld (sector %d)\n", data_area_sector * this->bytes_per_sector, data_area_sector);
        printf("%lld data clusters (%lld bytes)\n", data_clusters, (_int8)data_clusters * this->sectors_per_cluster *
            this->bytes_per_sector);
        printf("%d sectors/track, %d heads\n", this->number_of_sectors_per_track, this->number_of_heads);
        printf("%d sectors total\n", this->total_number_of_sectors);
        printf("\n");
    }
    void check_FAT_32() {
        long long root_dir_sectors = 1LL * this->number_root_entries * 32 / this->bytes_per_sector;
        long long data = this->total_number_of_sectors - (this->number_reserved_sectors +
            this->number_FAT_copies * this->number_of_sectors_per_FAT + root_dir_sectors);
        long long answer = data / this->sectors_per_cluster;
        if (answer < 4085) {
            printf("Your device is formated into FAT12 \n");
        }
        if (answer >= 4085 && answer <= 65524) {
            printf("Your device is formated into FAT16 \n");
        }
        if (answer > 65524) {
            printf("Your device is formated into FAT32 \n");
        }
    }
    int check_boot_sector() {
        int check = 0;
        _int1 MDT = this->media_descriptor_type; // MDT - media descriptor type
        if (MDT != media_byte_hard_disk && MDT != media_byte_removable_disk) {
            printf("WARNING: media descriptor type should be 0xF0(removable disk) or 0xF8(hard disk). Your device media descriptor: %x\n", MDT);
            check = 1;
        }
        _int1 NFC = this->number_FAT_copies; //NFC - number of fat copies
        if (NFC != 2) {
            printf("WARNING: number of FAT copies should be 2. Your device number of FAT copies: %d\n", NFC);
            check = 1;
        }
        if (this->signature_55AA != 0xAA55) {
            printf("WARNING: 510-511 bytes should be 0x55AA. Your device 510-511 bytes: %x\n", this->signature_55AA);
            check = 1;
        }
        _int2 BPS = this->bytes_per_sector; // BPS - bytes per sector
        if (BPS != INT_512 && BPS != INT_1024 && BPS != INT_2048 && BPS != INT_4096) {
            printf("ERROR: bytes per sector must be 512, 1024, 2048, 4096. From your device: %d\n", BPS);
            check = 2;
        }
        _int1 SPC = this->sectors_per_cluster; //SPC sectors per cluster
        if (SPC != INT_1 && SPC != INT_2 && SPC != INT_4 && SPC != INT_8 && SPC != INT_16 && SPC != INT_32 && SPC != INT_64 && SPC != INT_128) {
            printf("ERROR: sector per cluster must be 1, 2, 4, 8, 16, 32, 64, 128. From your device: %d\n", SPC);
            check = 2;
        }
        _int4 SOC = BPS * SPC; // SOC - size of clusters
        if (SOC > 65536) {
            printf("ERROR: A cluster should have at most 32768 bytes. In rare cases 65536 is OK. Your device cluster's size: %d\n", SOC);
            check = 2;
        }
        return check;
    }
    int check_equal_boot_sector(class BootSector_before first_boot_sector, class BootSector_before second_boot_sector) {
        second_boot_sector.reserved_for_WindowsNT = first_boot_sector.reserved_for_WindowsNT;
        return memcmp(&first_boot_sector, &second_boot_sector, 512);
    }
};
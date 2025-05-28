#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 4
#define MAX_DISKS 2
#define MAX_DATA_SIZE 100

#define RAID0 0
#define RAID1 1

typedef struct {
    char* blocks;
    int size;
} Disk;

typedef struct {
    Disk disks[MAX_DISKS];
    int raid_level;
    int num_disks;
    int total_blocks;
} RAIDSystem;

void init_disk(Disk* disk, int num_blocks) {
    disk->blocks = (char*)calloc(num_blocks * BLOCK_SIZE, sizeof(char));
    disk->size = num_blocks;
}

RAIDSystem* init_raid(int raid_level, int num_disks, int blocks_per_disk) {
    if (num_disks > MAX_DISKS) {
        printf("Error: Maximum number of disks is %d\n", MAX_DISKS);
        return NULL;
    }

    RAIDSystem* raid = (RAIDSystem*)malloc(sizeof(RAIDSystem));
    raid->raid_level = raid_level;
    raid->num_disks = num_disks;

    if (raid_level == RAID0) {
        raid->total_blocks = blocks_per_disk * num_disks;
    } else if (raid_level == RAID1) {
        raid->total_blocks = blocks_per_disk;
    }

    for (int i = 0; i < num_disks; i++) {
        init_disk(&raid->disks[i], blocks_per_disk);
    }

    return raid;
}

void raid_write(RAIDSystem* raid, const char* data, int size) {
    if (size > raid->total_blocks * BLOCK_SIZE) {
        printf("Error: Data size exceeds RAID capacity\n");
        return;
    }

    printf("\n%s write \"%s\"\n",
           raid->raid_level == RAID0 ? "RAID 0" : "RAID 1",
           data);

    if (raid->raid_level == RAID0) {
        int block_index = 0;
        int data_index = 0;

        while (data_index < size) {
            int disk_id = block_index % raid->num_disks;
            int disk_block = block_index / raid->num_disks;

            int bytes_to_copy = BLOCK_SIZE;
            if (size - data_index < BLOCK_SIZE) {
                bytes_to_copy = size - data_index;
            }

            memcpy(&raid->disks[disk_id].blocks[disk_block * BLOCK_SIZE],
                   &data[data_index], bytes_to_copy);

            printf("  Disk%d: B %d, ", disk_id + 1, disk_block);
            for (int i = 0; i < bytes_to_copy; i++) {
                printf("%c", data[data_index + i]);
            }
            printf("\n");

            data_index += bytes_to_copy;
            block_index++;
        }
    }
    else if (raid->raid_level == RAID1) {
        for (int disk_id = 0; disk_id < raid->num_disks; disk_id++) {
            memcpy(raid->disks[disk_id].blocks, data, size);
            printf("  Disk%d: %s\n", disk_id + 1, data);
        }
    }
}

void raid_read(RAIDSystem* raid, int offset, int size) {
    if (offset + size > raid->total_blocks * BLOCK_SIZE) {
        printf("Error: Read request exceeds RAID capacity\n");
        return;
    }

    char* result = (char*)malloc(size + 1);
    result[size] = '\0';

    printf("\n%s read at offset %d, size %d\n",
           raid->raid_level == RAID0 ? "RAID 0" : "RAID 1",
           offset, size);

    if (raid->raid_level == RAID0) {
        int data_index = 0;

        for (int block_index = offset / BLOCK_SIZE;
             data_index < size;
             block_index++) {

            int disk_id = block_index % raid->num_disks;
            int disk_block = block_index / raid->num_disks;
            int block_offset = (offset % BLOCK_SIZE) * (block_index == offset / BLOCK_SIZE);

            int block_remain = BLOCK_SIZE - block_offset;
            int bytes_to_read = (size - data_index < block_remain) ?
                                 size - data_index : block_remain;

            memcpy(&result[data_index],
                   &raid->disks[disk_id].blocks[disk_block * BLOCK_SIZE + block_offset],
                   bytes_to_read);

            printf("  Reading from Disk%d, Block %d: %.*s\n",
                   disk_id + 1, disk_block, bytes_to_read,
                   &raid->disks[disk_id].blocks[disk_block * BLOCK_SIZE + block_offset]);

            data_index += bytes_to_read;
        }
    }
    else if (raid->raid_level == RAID1) {
        memcpy(result, &raid->disks[0].blocks[offset], size);
        printf("  Reading from Disk1: %s\n", result);
    }

    printf("  Result: %s\n", result);
    free(result);
}

void free_raid(RAIDSystem* raid) {
    for (int i = 0; i < raid->num_disks; i++) {
        free(raid->disks[i].blocks);
    }
    free(raid);
}

void print_raid_state(RAIDSystem* raid) {
    printf("\nRAID %d State:\n", raid->raid_level);
    for (int i = 0; i < raid->num_disks; i++) {
        printf("Disk %d: ", i + 1);
        for (int j = 0; j < raid->disks[i].size; j++) {
            printf("[");
            for (int k = 0; k < BLOCK_SIZE; k++) {
                char c = raid->disks[i].blocks[j * BLOCK_SIZE + k];
                printf("%c", c ? c : '.');
            }
            printf("] ");
        }
        printf("\n");
    }
}

int main() {
    RAIDSystem* raid0 = init_raid(RAID0, 2, 4);
    RAIDSystem* raid1 = init_raid(RAID1, 2, 4);

    raid_write(raid0, "HELLO", 5);
    print_raid_state(raid0);

    raid_write(raid1, "HELLO", 5);
    print_raid_state(raid1);

    raid_read(raid0, 0, 5);
    raid_read(raid1, 0, 5);

    raid_write(raid0, "RAIDZEROSYSTEM", 14);
    print_raid_state(raid0);
    raid_read(raid0, 2, 10);

    raid_write(raid1, "RAIDONESYSTEM", 13);
    print_raid_state(raid1);
    raid_read(raid1, 1, 10);

    free_raid(raid0);
    free_raid(raid1);

    return 0;
}

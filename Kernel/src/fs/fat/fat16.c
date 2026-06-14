#include "fat16.h"
#include <stdint.h>
#include "disk/disk.h"
#include "disk/streamer.h"
#include "string/string.h"
#include "memory/heap/kheap.h"
#include "memory/memory.h"
#include "config.h"
#include "tool.h"
#include "status.h"

#define KERNEL_FAT16_SIGNATURE 0x29
#define KERNEL_FAT16_FAT_ENTRY_SIZE 0x02
#define KERNEL_FAT16_BAD_SECTOR 0xFF7
#define KERNEL_FAT16_UNUSED 0x00


typedef unsigned int FAT_ITEM_TYPE;
#define FAT_ITEM_TYPE_DIRECTORY 0
#define FAT_ITEM_TYPE_FILE 1

// Fat directory entry attributes bitmask
#define FAT_FILE_READ_ONLY 0x01
#define FAT_FILE_HIDDEN 0x02
#define FAT_FILE_SYSTEM 0x04
#define FAT_FILE_VOLUME_LABEL 0x08
#define FAT_FILE_SUBDIRECTORY 0x10
#define FAT_FILE_ARCHIVED 0x20
#define FAT_FILE_DEVICE 0x40
#define FAT_FILE_RESERVED 0x80


struct fat_header_extended {
	uint8_t drive_number;
	uint8_t win_nt_bit;
	uint8_t signature;
	uint32_t volume_id;
	uint8_t volume_id_string[11];
	uint8_t system_id_string[8];
} __attribute__((packed));

struct fat_header {
	uint8_t short_jmp_ins[3];
	uint8_t oem_identifier[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t fat_copies;
	uint16_t root_dir_entries;
	uint16_t number_of_sectors;
	uint8_t media_type;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t number_of_heads;
	uint32_t hidden_sectors;
	uint32_t sectors_big;
} __attribute__((packed));

struct fat_h {
	struct fat_header primary_header;
	union fat_h_e {
		struct fat_header_extended extended_header;
	} shared;
};

struct fat_directory_item
{
	uint8_t filename[8];
	uint8_t ext[3];
	uint8_t attribute;
	uint8_t reserved;
	uint8_t creation_time_tenths_of_a_sec;
	uint16_t creation_time;
	uint16_t creation_date;
	uint16_t last_access;
	uint16_t high_16_bits_first_cluster;
	uint16_t last_mod_time;
	uint16_t last_mod_date;
	uint16_t low_16_bits_first_cluster;
	uint32_t filesize;
} __attribute__((packed));

struct fat_directory
{
	struct fat_directory_item* item;
	int total;
	int sector_pos;
	int ending_sector_pos;
};

struct fat_item {
	union {
		struct fat_directory_item* item;
		struct fat_directory* directory;
	};

	FAT_ITEM_TYPE type;
};

struct fat_item_descriptor {
	struct fat_item* item;
	uint32_t pos;
};

struct fat_private {
	struct fat_h header;
	struct fat_directory root_directory;

	// Used to stream data clusters
	struct disk_stream* cluster_read_stream;
	// Used to stream the file allocation table
	struct disk_stream* fat_read_stream;

	// Used in situations where we stream the directory
	struct disk_stream* directory_stream;
};

int fat16_resolve(struct disk *disk);
void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode);

struct filesystem fat16_fs = {
        .open = fat16_open,
        .resolve = fat16_resolve,
};

struct filesystem *fat16_init()
{
        strcpy(fat16_fs.name, "FAT16");
        return &fat16_fs;
}

static inline int fat16_sector_to_absolute(struct disk *disk, int sector)
{
	return disk->sector_size * sector;
}

static int fat16_get_total_items_for_directory(struct disk *disk, int directory_start_sector)
{
	int res = 0;
	int cnt = 0;
	struct fat_directory_item item;
	struct fat_private *fat_private = disk->fs_private;
	struct disk_stream *stream = fat_private->directory_stream;

	if(diskstream_seek(stream, fat16_sector_to_absolute(disk, directory_start_sector)) != KERNEL_ALL_OK) {
		res = -EIO;
		goto out;
	}

	while (1) {
		if (diskstream_read(stream, &item, sizeof(item)) != KERNEL_ALL_OK) {
			res = -EIO;
			goto out;
		}
		if (item.filename[0] == 0x00)
			break;
		if (item.filename[0] == 0xE5)
			continue;
		cnt++;
	}
	res = cnt;
out:
	return  res;
}

static int fat16_get_first_cluster(struct fat_directory_item *item)
{
	return (item->high_16_bits_first_cluster) | (item->low_16_bits_first_cluster);
}

static int fat16_cluster_to_sector(struct fat_private *fat_private, int cluster)
{
	return fat_private->root_directory.ending_sector_pos + ((cluster - 2) * fat_private->header.primary_header.sectors_per_cluster);
}

static uint32_t fat16_get_first_fat_sector(struct fat_private *fat_private)
{
	return fat_private->header.primary_header.reserved_sectors;
}

static int fat16_get_fat_entry(struct disk *disk, int cluster)
{
	int res = -1;
	struct fat_private *fat_private = disk->fs_private;
	struct disk_stream *stream = fat_private->fat_read_stream;
	if (!stream)
		goto out;
		
	uint32_t fat_table_position = fat16_get_first_fat_sector(fat_private) * disk->sector_size;
	res = diskstream_seek(stream, fat_table_position + (cluster * KERNEL_FAT16_FAT_ENTRY_SIZE));
	if (res < 0)
		goto out;
	
	uint16_t result = 0;
	res = diskstream_read(stream, &result, sizeof(result));
	if (res < 0)
		goto out;

	res = result;

out:
	return res;
}

static int fat16_check_cluster_entry(int entry)
{
	if (entry == 0xFF8 || entry == 0xFFF)
		return -EIO;
	if (entry == KERNEL_FAT16_BAD_SECTOR)
		return -EIO;
	if (entry == 0xFF0 || entry == 0xFF6)
		return -EIO;
	if (entry == 0x00)
		return -EIO;
	return 0;
}

static int fat16_get_cluster_for_offset(struct disk *disk, int starting_cluster, int offset)
{
	int res = 0;
	struct fat_private *fat_private = disk->fs_private;
	int size_of_cluster_bytes = fat_private->header.primary_header.sectors_per_cluster * disk->sector_size;
	int cluster_to_use = starting_cluster;
	int cluster_ahead = offset / size_of_cluster_bytes;

	for (int i = 0; i < cluster_ahead; i++) {
		int entry = fat16_get_fat_entry(disk, cluster_to_use);
		if (fat16_check_cluster_entry(entry) < 0) {
			res = -EIO;
			goto out;
		}
		cluster_to_use = entry;
	}

	res = cluster_to_use;
out:
	return res;
}

static int fat16_get_next_cluster(struct disk *disk, int cluster)
{
	int entry = fat16_get_fat_entry(disk, cluster);
	if (fat16_check_cluster_entry(entry) < 0)
		return -EIO;
	return entry;
}

static int fat16_read_internal_from_stream(struct disk *disk, struct disk_stream *stream, int starting_cluster,
	int offset, int total, void *out)
{
	int res = 0;
	struct fat_private *fat_private = disk->fs_private;
	int per_cluster_size = fat_private->header.primary_header.sectors_per_cluster * disk->sector_size;
	int cluster_to_use = fat16_get_cluster_for_offset(disk, starting_cluster, offset);
	if (cluster_to_use < 0) {
		res = cluster_to_use;
		goto out;
	}

	while (total > 0) {
		int starting_sector = fat16_cluster_to_sector(fat_private, cluster_to_use);
		int offset_from_cluster = offset % per_cluster_size;
		int starting_pos = (starting_sector * disk->sector_size) + offset_from_cluster;
		int total_to_read = min_t(int, total, per_cluster_size - offset);

		res = diskstream_seek(stream, starting_pos);
		if (res != KERNEL_ALL_OK)
			goto out;

		res = diskstream_read(stream, out, total_to_read);
		if (res != KERNEL_ALL_OK)
			goto out;

		total -= total_to_read;
		offset += total_to_read;
		out += total_to_read;

		if (total > 0) {
			cluster_to_use = fat16_get_next_cluster(disk, cluster_to_use);
			if (cluster_to_use < 0) {
				res = cluster_to_use;
				goto out;
			}
		}
	}

out:
	return res;
}

static int fat16_read_internal(struct disk *disk, int starting_cluster, int offset, int total, void *out)
{
	struct fat_private *fat_private = disk->fs_private;
	struct disk_stream *stream = fat_private->cluster_read_stream;
	return fat16_read_internal_from_stream(disk, stream, starting_cluster, offset, total, out);
}

static void fat16_free_directory(struct fat_directory *directory)
{
	if (!directory)
		return;
	kfree(directory->item);
	kfree(directory);
}

static struct fat_directory *fat16_load_fat_directory(struct disk *disk, struct fat_directory_item *item)
{
	int res = 0;
	struct fat_private *fat_private = disk->fs_private;
	struct fat_directory *directory = NULL;

	if (!(item->attribute & FAT_FILE_SUBDIRECTORY)) {
		res = -EINVAGS;
		goto out;
	}

	directory = kzalloc(sizeof(struct fat_directory));
	if (!directory) {
		res = -ENOMEM;
		goto out;
	}

	int cluster = fat16_get_first_cluster(item);
	int cluster_sector = fat16_cluster_to_sector(fat_private, cluster);
	int total_items = fat16_get_total_items_for_directory(disk, cluster_sector);
	int directory_size = total_items * sizeof(struct fat_directory_item);
	
	directory->total = total_items;
	directory->item = kzalloc(directory_size);
	if (!directory->item) {
		res = -ENOMEM;
		goto out;
	}

	res = fat16_read_internal(disk, cluster, 0x00, directory_size, directory->item);
	if (res != KERNEL_ALL_OK)
		goto out;

out:
	if (res != KERNEL_ALL_OK) {
		fat16_free_directory(directory);
		directory = NULL;
	}

	return directory;
}

static struct fat_directory_item *fat16_clone_directory_item(struct fat_directory_item *item, int size)
{
	struct fat_directory_item *item_copy = NULL;
	if (size < sizeof(struct fat_directory_item))
		goto out;

	item_copy = kzalloc(size);
	if (!item_copy)
		goto out;
	
	memcpy(item_copy, item, size);

out:
	return item_copy;
}

static struct fat_item *fat16_new_fat_item_for_directory_item(struct disk *disk, struct fat_directory_item *item)
{
	struct fat_item *f_item = kzalloc(sizeof(struct fat_item));
	if (!f_item)
		return NULL;
	
	if (item->attribute & FAT_FILE_SUBDIRECTORY) {
		f_item->type = FAT_ITEM_TYPE_DIRECTORY;
		f_item->directory = fat16_load_fat_directory(disk, item);
	} else {
		f_item->type = FAT_ITEM_TYPE_FILE;
		f_item->item = fat16_clone_directory_item(item, sizeof(struct fat_directory_item));
	}
	return f_item;
}

static void fat16_to_proper_string(char **out, const char *in)
{
	while (*in != 0x00 && *in == 0x20) {
		**out = *in;
		*out += 1;
		in += 1;
	}
	**out = 0x00;
}

static void fat16_get_full_relative_filename(struct fat_directory_item *item, char *out, int max_len)
{
	memset(out, 0x00, max_len);
	char *out_tmp = out;
	fat16_to_proper_string(&out_tmp, (const char *)item->filename);
	if (item->ext[0] != 0x00 && item->ext[0] != 0x20) {
		*out_tmp = '.';
		fat16_to_proper_string(&out_tmp, (const char *)item->ext);
	}
}

static struct fat_item *fat16_find_item_in_directory(struct disk *disk, struct fat_directory *directory, const char *name)
{
	struct fat_item *f_item = NULL;
	char tmp_filename[KERNEL_MAX_PATH];
	for (int i = 0; i < directory->total; ++i) {
		fat16_get_full_relative_filename(&directory->item[i], tmp_filename, sizeof(tmp_filename));
		if (!istrncmp(tmp_filename, name, sizeof(tmp_filename))) {
			f_item = fat16_new_fat_item_for_directory_item(disk, &directory->item[i]);
		}
	}

	return f_item;
}

static void fat16_fat_item_free(struct fat_item *item)
{
	if (item->type == FAT_ITEM_TYPE_DIRECTORY)
		kfree(item->directory);
	else
		kfree(item->item);
	kfree(item);
}

static struct fat_item *fat16_get_directory_entry(struct disk *disk, struct path_part *path)
{
	struct fat_private *fat_private = disk->fs_private;
	struct fat_item *current_item = NULL;
	struct fat_item *root_item = fat16_find_item_in_directory(disk, &fat_private->root_directory, path->part);
	if (root_item)
		goto out;

	struct path_part *next_part = path->next;
	current_item = root_item;
	while (next_part != NULL) {
		if (current_item->type != FAT_ITEM_TYPE_DIRECTORY) {
			current_item = NULL;
			goto out;
		}

		struct fat_item *tmp_item = fat16_find_item_in_directory(disk, current_item->directory, next_part->part);
		if (!tmp_item) {
			current_item = NULL;
			goto out;
		}

		fat16_fat_item_free(current_item);
		current_item = tmp_item;
		next_part = next_part->next;
	}

out:
	return current_item;
}

void *fat16_open(struct disk *disk, struct path_part *path, FILE_MODE mode)
{
        struct fat_item_descriptor *descriptor = NULL;
	
	if (mode != FILE_MODE_READ)
		return ERROR(-ERDONLY);

	descriptor = kzalloc(sizeof(struct fat_item_descriptor));
	if (!descriptor)
		return ERROR(-ENOMEM);

	descriptor->item = fat16_get_directory_entry(disk, path);
	if (!descriptor->item)
		return ERROR(-EIO);

	descriptor->pos = 0;
	return descriptor;
}

static void fat16_init_private(struct disk *disk, struct fat_private *fat_private) {
	memset(fat_private, 0x00, sizeof(struct fat_private));
	fat_private->fat_read_stream = diskstream_new(disk->id);
	fat_private->cluster_read_stream = diskstream_new(disk->id);
	fat_private->directory_stream = diskstream_new(disk->id);
}

static int fat16_get_root_directory(struct disk *disk, struct fat_private *fat_private,
	struct fat_directory* fat_directory)
{
	int res = 0;
	struct fat_header *primary_header = &fat_private->header.primary_header;
	int root_dir_sector_pos = (primary_header->fat_copies * primary_header->sectors_per_fat) +
		primary_header->reserved_sectors;
	int root_dir_entries = primary_header->root_dir_entries;
	int root_dir_size = root_dir_entries * sizeof(struct fat_directory_item);
	int total_sectors = root_dir_size / disk->sector_size;
	if (root_dir_size % disk->sector_size)
		total_sectors += 1;

	int total_items = fat16_get_total_items_for_directory(disk, root_dir_sector_pos);

	struct fat_directory_item *dir = kzalloc(root_dir_size);
	if (!dir) {
		res = -ENOMEM;
		goto out;
	}

	struct disk_stream *stream = fat_private->directory_stream;
	if (diskstream_seek(stream, fat16_sector_to_absolute(disk, root_dir_sector_pos)) != KERNEL_ALL_OK) {
		res = -EIO;
		goto out;
	}

	if (diskstream_read(stream, dir, root_dir_size) != KERNEL_ALL_OK) {
		res = -EIO;
		goto out;
	}

	fat_directory->item = dir;
	fat_directory->total = total_items;
	fat_directory->sector_pos = root_dir_sector_pos;
	fat_directory->ending_sector_pos = root_dir_sector_pos + root_dir_size / disk->sector_size;

out:
	return res;
}

static void fat16_free_private(struct fat_private* private)
{
	if (!private) return;

	if (private->cluster_read_stream)
		diskstream_close(private->cluster_read_stream);
	if (private->fat_read_stream)
		diskstream_close(private->fat_read_stream);
	if (private->directory_stream)
		diskstream_close(private->directory_stream);

	kfree(private->root_directory.item);

	kfree(private);
}


int fat16_resolve(struct disk *disk)
{
	int res = 0;
	struct fat_private *fat_private = kzalloc(sizeof(struct fat_private));
	fat16_init_private(disk, fat_private);

	disk->fs_private = fat_private;
	disk->filesystem = &fat16_fs;

	struct disk_stream *stream = diskstream_new(disk->id);
	if (!stream) {
		res = -ENOMEM;
		goto out;
	}

	if (diskstream_read(stream, &fat_private->header, sizeof(fat_private->header)) != KERNEL_ALL_OK) {
		res = -EIO;
		goto out;
	}

	if (fat_private->header.shared.extended_header.signature != 0x29) {
		res = -EFSNOTUS;
		goto out;
	}

	if (fat16_get_root_directory(disk, fat_private, &fat_private->root_directory) != KERNEL_ALL_OK) {
		res = -EIO;
		goto out;
	}

out:
	if (stream) {
		diskstream_close(stream);
	}
	if (res < 0) {
		fat16_free_private(fat_private);
		disk->fs_private = NULL;
	}
	return res;
}
#include "elfloader.h"
#include "fs/file.h"
#include "status.h"
#include <stdbool.h>
#include "memory/memory.h"
#include "memory/heap/kheap.h"
#include "string/string.h"
#include "memory/paging/paging.h"
#include "kernel.h"
#include "config.h"
#include "elf.h"


const char* elf_signature[] = {0x7f, 'E', 'L', 'F'};

// 检查ELF魔数（4字节）
static bool elf_valid_signature(void* buffer)
{
	return memcmp(buffer, (void*) elf_signature, sizeof(elf_signature)) == 0;
}

// We only support 32 bit binaries.
static bool elf_valid_class(struct elf_header* header)
{
	return header->e_ident[EI_CLASS] == ELFCLASSNONE || header->e_ident[EI_CLASS] == ELFCLASS32;
}

// 字节序，我们只支持小端
static bool elf_valid_encoding(struct elf_header* header)
{
	return header->e_ident[EI_DATA] == ELFDATANONE || header->e_ident[EI_DATA] == ELFDATA2LSB;
}

static bool elf_is_executable(struct elf_header* header)
{
	return header->e_type == ET_EXEC && header->e_entry >= KERNEL_PROGRAM_VIRTUAL_ADDRESS;
}

static bool elf_has_program_header(struct elf_header* header)
{
	return header->e_phoff != 0;
}

void* elf_memory(struct elf_file* file)
{
	return file->elf_memory;
}

struct elf_header* elf_header(struct elf_file* file)
{
	return file->elf_memory;
}

struct elf32_shdr* elf_sheader(struct elf_header* header)
{
	return (struct elf32_shdr*)((int)header+header->e_shoff);
}

struct elf32_phdr* elf_pheader(struct elf_header* header)
{
	if(header->e_phoff == 0)
		return 0;

	return (struct elf32_phdr*)((int)header + header->e_phoff);
}

struct elf32_phdr* elf_program_header(struct elf_header* header, int index)
{
	return &elf_pheader(header)[index];
}

struct elf32_shdr* elf_section(struct elf_header* header, int index)
{
	return &elf_sheader(header)[index];
}

char* elf_str_table(struct elf_header* header)
{
	return (char*) header + elf_section(header, header->e_shstrndx)->sh_offset;
}

void* elf_virtual_base(struct elf_file* file)
{
	return file->virtual_base_address;
}

void* elf_virtual_end(struct elf_file* file)
{
	return file->virtual_end_address;
}

void* elf_phys_base(struct elf_file* file)
{
	return file->physical_base_address;
}

void* elf_phys_end(struct elf_file* file)
{
	return file->physical_end_address;
}

int elf_validate_loaded(struct elf_header* header)
{
	return (elf_valid_signature(header) &&
		elf_valid_class(header) &&
		elf_valid_encoding(header) &&
		elf_has_program_header(header)) ?
		KERNEL_ALL_OK : -EINVAGS;
}

int elf_process_phdr_pt_load(struct elf_file *elf_file, struct elf32_phdr *phdr)
{
	if (elf_file->virtual_base_address >= phdr->p_vaddr || elf_file->virtual_base_address == 0) {
		elf_file->virtual_base_address = phdr->p_vaddr;
		elf_file->physical_base_address = elf_file->elf_memory + phdr->p_offset;
	}

	unsigned int end_virtual_address = phdr->p_vaddr + phdr->p_filesz;
	if (elf_file->virtual_end_address <= (void *)end_virtual_address || elf_file->virtual_end_address == 0) {
		elf_file->virtual_end_address = (void *)end_virtual_address;
		elf_file->physical_end_address = elf_file->elf_memory + phdr->p_offset + phdr->p_filesz;
	}

	return 0;
}

int elf_process_pheader(struct elf_file *elf_file, struct elf32_phdr *phdr)
{
	int res = 0;
	switch (phdr->p_type) {
		case PT_LOAD:
			res = elf_process_phdr_pt_load(elf_file, phdr);
		break;
		
		default:
			res = -EINVAGS;
		break;
	}
	return res;
}

int elf_process_pheaders(struct elf_file *elf_file)
{
	int res = 0;
	int i = 0;
	struct elf_header *header = elf_header(elf_file);

	for (i = 0; i < header->e_phnum; ++i) {
		struct elf32_phdr* phdr = elf_program_header(header, i);
		res = elf_process_pheader(elf_file, phdr);
		if (res < 0)
			break;
	}

	return res;
}

int elf_process_load(struct elf_file *elf_file)
{
	int res = 0;
	struct elf_header *header = elf_header(elf_file);
	res = elf_validate_loaded(header);
	if (res < 0)
		goto out;

	res = elf_process_pheaders(elf_file);
	if (res < 0)
		goto out;

out:
	return res;
}

int elf_load(const char* filename, struct elf_file** file_out)
{
	int res = 0;
	struct elf_file *elf_file = NULL;
	int fd = 0;
	struct file_stat file_stat;
	void *elf_memory = NULL;
	
	elf_file = kzalloc(sizeof(struct elf_file));
	if (!elf_file) {
		res = -ENOMEM;
		goto out;
	}
	fd = fopen(filename, "r");
	if (fd <= 0) {
		res = -EIO;
		goto free_out;
	}

	res = fstat(fd, &file_stat);
	if (res < 0)
		goto close_out;

	elf_memory = kzalloc(file_stat.filesize);
	if (!elf_memory) {
		res = -ENOMEM;
		goto close_out;
	}

	res = fread(elf_memory, file_stat.filesize, 1, fd);
	if (res < 0)
		goto close_out;

	elf_file->elf_memory = elf_memory;
	elf_file->in_memory_size = file_stat.filesize;

	res = elf_process_load(elf_file);
	if (res < 0)
		goto close_out;

	memcpy(elf_file->filename, filename, KERNEL_MAX_PATH);
	*file_out = elf_file;
	goto out;

close_out:
	fclose(fd);
free_out:
	kfree(elf_file);
	kfree(elf_memory);
out:
	return res;
}

void elf_close(struct elf_file* file)
{
	if (!file)
		return;
	kfree(file->elf_memory);
	kfree(file);
}
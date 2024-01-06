#include <pak.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum FileAccessType {
    FileAccessTypeRead
} FileAccessType;

typedef struct PakFileEntry {
    char name[56];
    uint32_t offset;
    uint32_t size;
} PakFileEntry;

typedef struct PakArchiveInstance {
    FILE* archive_file;
    FileAccessType access_type;
    PakFileEntry* file_entries;
    uint32_t num_entries;
} PakArchiveInstance;

typedef struct PakHeader {
    uint32_t magic;
    uint32_t pak_offset;
    uint32_t pak_size;
} PakHeader;

typedef struct PakFileEntryDataInstance {
    PakArchiveInstance* pak;
    PakFileEntry pak_file_entry_info;
} PakFileEntryDataInstance;

PakArchiveHandle open_pak_archive_read(const char* pak_path) {
    const int expected_magic = 'KCAP'; // 'PACK' with corrected endianess TODO: fix big endian

    FILE* pak_file = fopen(pak_path, "rb");

    if(!pak_file) {
        return NULL;
    }

    PakHeader header = {0};
    fread(&header, sizeof(PakHeader), 1, pak_file);

    if(expected_magic != header.magic) {
        printf("Bad magic!");
        return NULL;
    }
    uint32_t num_files = header.pak_size / sizeof(PakFileEntry);

    PakFileEntry* entryBuffer = malloc(num_files * sizeof(PakFileEntry));

    fseek(pak_file, header.pak_offset, SEEK_SET);   
    fread(entryBuffer, sizeof(PakFileEntry), num_files, pak_file);

    PakArchiveInstance* return_handle = malloc(sizeof(PakArchiveInstance));

    return_handle->archive_file = pak_file;
    return_handle->access_type = FileAccessTypeRead;
    return_handle->file_entries = entryBuffer;
    return_handle->num_entries = num_files;
    return return_handle;
}

void close_pak_archive(PakArchiveHandle pak_archive) {
    fclose(pak_archive->archive_file);
    free(pak_archive);
}

// Uses linear search but this is such a basic program that I don't care at all
PakFileEntry* pak_find_file(PakFileEntry* file_entry_buffer, uint32_t count, const char* name) {
    for (uint32_t i = 0; i < count; i++) {
        if(!strcmp(file_entry_buffer[i].name, name)) 
            return file_entry_buffer + i;
    }
    return NULL;
}

PakFileEntryDataHandle open_pak_file_entry_read(PakArchiveHandle pak_archive, const char* name) {
    PakFileEntry* pak_file_entry = pak_find_file(pak_archive->file_entries, pak_archive->num_entries, name);
    
    PakFileEntryDataHandle return_handle = malloc(sizeof(PakFileEntryDataInstance));
    return_handle->pak = pak_archive;
    return_handle->pak_file_entry_info = *pak_file_entry;
    return return_handle;
}

void pak_file_entry_get_bytes(PakFileEntryDataHandle pak_file_handle, uint32_t local_offset, uint32_t ammount, void* buffer) {

    uint32_t read_ammount = ammount;
    if (local_offset + ammount > pak_file_handle->pak_file_entry_info.size) {
        read_ammount = pak_file_handle->pak_file_entry_info.size - local_offset;
    } else {
        read_ammount = ammount;
    }

    fseek(pak_file_handle->pak->archive_file, pak_file_handle->pak_file_entry_info.offset + local_offset, SEEK_SET);
    if(!fread(buffer, read_ammount, 1, pak_file_handle->pak->archive_file)) {
        printf("failed to read from %s", pak_file_handle->pak_file_entry_info.name);
    }
    
}

void close_pak_file_entry(PakFileEntryDataHandle pak_file_handle) {
    free(pak_file_handle);
}

void pak_dump_file_info(PakArchiveHandle pak_archive) {
    for (uint32_t i = 0; i < pak_archive->num_entries; i++) {
        PakFileEntry i_entry = pak_archive->file_entries[i];

        printf("File name: %s\n", i_entry.name);
        printf("  Offset: %d | ", i_entry.offset);
        printf("Size: %d \n", i_entry.size);
    }

}
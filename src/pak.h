#pragma once
#include <stdint.h>

// You can read up on the PAK spec here: https://quakewiki.org/wiki/.pak 
typedef struct PakArchiveInstance* PakArchiveHandle;

typedef struct PakFileEntryDataInstance* PakFileEntryDataHandle;


PakArchiveHandle open_pak_archive_read(const char* pak_path);

void close_pak_archive(PakArchiveHandle pak_archive);


PakFileEntryDataHandle open_pak_file_entry_read(PakArchiveHandle pak_archive, const char* name);

void pak_file_entry_get_bytes(PakFileEntryDataHandle pak_file_handle, uint32_t local_offset, uint32_t ammount, void* buffer);

void close_pak_file_entry(PakFileEntryDataHandle pak_archive);


void pak_dump_file_info(PakArchiveHandle pak_archive);
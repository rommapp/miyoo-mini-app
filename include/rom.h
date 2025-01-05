#ifndef ROMM_ROM_H
#define ROMM_ROM_H

// Structure to hold ROM information
typedef struct RomMRom {
    int id;
    int* igdb_id;              // Nullable
    void* sgdb_id;             // object type
    void* moby_id;             // object type
    int platform_id;
    char* platform_slug;
    char* platform_name;
    char* file_name;
    char* file_name_no_tags;
    char* file_name_no_ext;
    char* file_extension;
    char* file_path;
    unsigned long long file_size_bytes;
    char* name;
    char* slug;
    char* summary;
    int* first_release_date;    // Nullable
    char* path_cover_s;
    char* path_cover_l;
    bool has_cover;
    char* url_cover;
    char* revision;
    bool multi;
    void** files;              // Array of object pointers
    int files_count;
    char* full_path;
    char* created_at;          // ISO 8601 datetime string
    char* updated_at;          // ISO 8601 datetime string
} RomMRom;

// Function declarations for memory management
void free_rom(RomMRom* rom);

// Function declarations for operations
int download_rom(const char* url, const char* destination);

#endif /* ROMM_ROM_H */

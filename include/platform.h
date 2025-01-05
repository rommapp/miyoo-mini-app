#ifndef ROMM_PLATFORM_H
#define ROMM_PLATFORM_H

#include <json-c/json.h>
#include <stdbool.h>
#include "rom.h"

// Structure to hold firmware information
typedef struct RomMPlatformFirmware {
    int id;
    char* file_name;
    char* file_name_no_tags;
    char* file_name_no_ext;
    char* file_extension;
    char* file_path;
    int file_size_bytes;
    char* full_path;
    bool is_verified;
    char* crc_hash;
    char* md5_hash;
    char* sha1_hash;
    char* created_at;    // ISO 8601 datetime string
    char* updated_at;    // ISO 8601 datetime string
} RomMPlatformFirmware;

// Structure to hold platform information
typedef struct RomMPlatform {
    int id;
    char* slug;
    char* fs_slug;
    char* name;
    int igdb_id;  // Using pointer to handle nullable ulong
    int sgdb_id;  // Using pointer to handle nullable ulong
    int moby_id;  // Using pointer to handle nullable ulong
    int rom_count;
    char* logo_path;
    RomMPlatformFirmware** firmware;  // Array of pointers to firmware
    int firmware_count;          // To keep track of firmware array size
    char* created_at;           // ISO 8601 datetime string
    char* updated_at;           // ISO 8601 datetime string
} RomMPlatform;

// Function declarations for memory management
void free_platform(RomMPlatform* platform);
void free_firmware(RomMPlatformFirmware* firmware);
void free_platform_list(RomMPlatform* platforms, int count);

// Function declarations for operations
char* generate_authorization_header(const char* username, const char* password);
int fetch_platform_list(const char* server_url, const char* username, const char* password, RomMPlatform** platform_list, int* platform_count);

#endif // ROMM_PLATFORM_H

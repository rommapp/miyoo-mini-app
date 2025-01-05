#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/platform.h"

// Free memory for a single firmware
void free_firmware(RomMPlatformFirmware* firmware) {
    if (!firmware) return;

    free(firmware->file_name);
    free(firmware->file_name_no_tags);
    free(firmware->file_name_no_ext);
    free(firmware->file_extension);
    free(firmware->file_path);
    free(firmware->full_path);
    free(firmware->crc_hash);
    free(firmware->md5_hash);
    free(firmware->sha1_hash);
    free(firmware->created_at);
    free(firmware->updated_at);

    free(firmware); // Free the structure itself
}

// Free memory for a platform, including its firmware
void free_platform(RomMPlatform* platform) {
    if (!platform) return;

    free(platform->slug);
    free(platform->fs_slug);
    free(platform->name);
    free(platform->logo_path);
    free(platform->created_at);
    free(platform->updated_at);

    // Free each firmware item
    for (int i = 0; i < platform->firmware_count; i++) {
        free_firmware(platform->firmware[i]);
    }
    free(platform->firmware); // Free the firmware array

    free(platform); // Free the structure itself
}

// Free an array of platforms
void free_platform_list(RomMPlatform* platforms, int count) {
    for (int i = 0; i < count; i++) {
        free_platform(&platforms[i]);
    }
    free(platforms);
}

// Return the console path based on platform slug
const char* get_console_path(const char* platform_slug) {
    // This example assumes a fixed base path; modify as needed
    static char path[1024];
    // TODO
    snprintf(path, sizeof(path), "/Roms/%s", platform_slug);
    return path;
}

// Fetch platform list from the server
int fetch_platform_list(const char* server_url, RomMPlatform** platform_list, int* platform_count) {
    if (!server_url || !platform_list || !platform_count) return -1;

    // Simulate fetching data from a server
    printf("Fetching platform list from %s\n", server_url);

    // Allocate memory and populate with mock data for testing
    *platform_count = 3;
    *platform_list = malloc(*platform_count * sizeof(RomMPlatform));
    if (!*platform_list) return -1;

    for (int i = 0; i < *platform_count; i++) {
        (*platform_list)[i].id = i + 1;
        (*platform_list)[i].slug = strdup("example-slug");
        (*platform_list)[i].fs_slug = strdup("example-fs-slug");
        (*platform_list)[i].name = strdup("Example Platform");
        (*platform_list)[i].rom_count = 100;
        (*platform_list)[i].igdb_id = NULL;
        (*platform_list)[i].sgdb_id = NULL;
        (*platform_list)[i].moby_id = NULL;
        (*platform_list)[i].logo_path = strdup("/path/to/logo.png");
        (*platform_list)[i].firmware = NULL;
        (*platform_list)[i].firmware_count = 0;
        (*platform_list)[i].created_at = strdup("2025-01-01T12:00:00Z");
        (*platform_list)[i].updated_at = strdup("2025-01-02T12:00:00Z");
    }
    return 0;
}

// Fetch ROM list for a platform
int fetch_rom_list(const char* server_url, const char* platform_slug, RomMRom** rom_list, int* rom_count) {
    if (!server_url || !platform_slug || !rom_list || !rom_count) return -1;

    // Simulate fetching data from a server
    printf("Fetching ROM list for platform '%s' from %s\n", platform_slug, server_url);

    // Allocate memory and populate with mock data for testing
    *rom_count = 10;
    *rom_list = malloc(*rom_count * sizeof(RomMRom));
    if (!*rom_list) return -1;

    for (int i = 0; i < *rom_count; i++) {
        (*rom_list)[i].id = i + 1;
        (*rom_list)[i].igdb_id = i + 10;
        (*rom_list)[i].sgdb_id = i + 20;
        (*rom_list)[i].moby_id = i + 30;
        (*rom_list)[i].platform_id = 1;
        (*rom_list)[i].platform_slug = strdup(platform_slug);
        (*rom_list)[i].platform_name = strdup("Example Platform");
        (*rom_list)[i].file_name = strdup("example-rom.zip");
        (*rom_list)[i].file_name_no_tags = strdup("example-rom");
        (*rom_list)[i].file_name_no_ext = strdup("example-rom");
        (*rom_list)[i].file_extension = strdup("zip");
        (*rom_list)[i].file_path = strdup("/path/to/example-rom.zip");
        (*rom_list)[i].file_size_bytes = 1024;
        (*rom_list)[i].name = strdup("Example ROM");
        (*rom_list)[i].slug = strdup("example-rom");
        (*rom_list)[i].summary = strdup("This is an example ROM");
        (*rom_list)[i].first_release_date = NULL;
        (*rom_list)[i].path_cover_s = strdup("/path/to/cover-s.png");
        (*rom_list)[i].path_cover_l = strdup("/path/to/cover-l.png");
        (*rom_list)[i].has_cover = true;
        (*rom_list)[i].url_cover = strdup("https://example.com/cover.png");
        (*rom_list)[i].revision = strdup("1.0");
        (*rom_list)[i].multi = false;
        (*rom_list)[i].files_count = 1;
        (*rom_list)[i].full_path = strdup("/path/to/example-rom.zip");
        (*rom_list)[i].created_at = strdup("2025-01-01T12:00:00Z");
        (*rom_list)[i].updated_at = strdup("2025-01-02T12:00:00Z");
    }
    return 0;
}

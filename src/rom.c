#include "include/rom.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Free memory for a single ROM
void free_rom(RomMRom* rom) {
    if (!rom) return;

    free(rom->platform_slug);
    free(rom->platform_name);
    free(rom->file_name);
    free(rom->file_name_no_tags);
    free(rom->file_name_no_ext);
    free(rom->file_extension);
    free(rom->file_path);
    free(rom->name);
    free(rom->slug);
    free(rom->summary);
    free(rom->path_cover_s);
    free(rom->path_cover_l);
    free(rom->url_cover);
    free(rom->revision);
    free(rom->full_path);
    free(rom->created_at);
    free(rom->updated_at);

    free(rom); // Free the structure itself
}

// Download a ROM file from a URL to a destination path
int download_rom(const char* url, const char* destination) {
    if (!url || !destination) return -1;

    // Simulate downloading the ROM file
    printf("Downloading ROM from %s to %s\n", url, destination);

    // In a real implementation, you would use a library like libcurl to download the file
    // For the sake of this example, we'll just return a success code
    return 0;
}

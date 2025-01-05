#include <stdio.h>
#include <stdlib.h>
#include "platform.h"

int main() {
    // Define the server URL and the credentials for Basic Auth
    const char* server_url = "https";
    const char* username = "";
    const char* password = "";

    // Declare variables for storing the platform list and count
    RomMPlatform* platform_list = NULL;
    int platform_count = 0;

    // Fetch the platform list from the server
    int result = fetch_platform_list(server_url, username, password, &platform_list, &platform_count);

    if (result == 0) {
        // Successfully fetched platform list
        printf("Fetched %d platforms\n", platform_count);

        // Loop through the platforms and print some details
        for (int i = 0; i < platform_count; i++) {
            printf("Platform ID: %d\n", platform_list[i].id);
            printf("Name: %s\n", platform_list[i].name);
            printf("Slug: %s\n", platform_list[i].slug);
            printf("ROM Count: %d\n", platform_list[i].rom_count);
            printf("Created At: %s\n", platform_list[i].created_at);
            printf("Updated At: %s\n", platform_list[i].updated_at);
            printf("Logo Path: %s\n", platform_list[i].logo_path);
            printf("\n");
        }

        // Free the allocated platform list
        free_platform_list(platform_list, platform_count);
    } else {
        // Handle the error if fetching failed
        fprintf(stderr, "Failed to fetch platform list\n");
    }

    return 0;
}

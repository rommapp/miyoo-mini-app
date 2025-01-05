#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <json-c/json.h>
#include "platform.h"
#include "base64.h"
#include "response.h"

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
    return "miyoo-mini";
}

// Function to generate the Basic Authorization header from username and password
char* generate_authorization_header(const char* username, const char* password) {
    // Create a buffer large enough to hold the base64-encoded credentials
    char* auth_header = malloc(512);
    if (auth_header == NULL) {
        fprintf(stderr, "Failed to allocate memory for authorization header\n");
        return NULL;
    }

    // Format the username and password into the "username:password" format
    char credentials[512];
    snprintf(credentials, sizeof(credentials), "%s:%s", username, password);

    // Base64 encode the credentials and get the encoded string
    size_t encoded_len;
    unsigned char* encoded_credentials = base64_encode((unsigned char*)credentials, strlen(credentials), &encoded_len);

    if (encoded_credentials == NULL) {
        fprintf(stderr, "Base64 encoding failed\n");
        free(auth_header);
        return NULL;
    }

    // Format the Authorization header with "Basic <encoded_credentials>"
    snprintf(auth_header, 512, "Authorization: Basic %s", encoded_credentials);

    // Free the memory allocated by base64_encode
    free(encoded_credentials);

    return auth_header;
}

// Function to fetch the platform list from the server
int fetch_platform_list(const char* server_host, const char* username, const char* password, RomMPlatform** platform_list, int* platform_count) {
    Response* resp = response_init();
    char url[1024];
    char auth_header[1024];
    char command[2100];
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    if (!resp) {
        fprintf(stderr, "Failed to initialize response\n");
        return -1;
    }

    // Build the URL for the request
    snprintf(url, sizeof(url), "%s/api/platforms", server_host);

    // Create the Authorization header
    snprintf(auth_header, sizeof(auth_header), "%s", generate_authorization_header(username, password));

    // Build the curl command
    snprintf(command, sizeof(command), "/mnt/SDCARD/.tmp_update/bin/curl -s -H \"%s\" %s", auth_header, url);

    // Execute curl and capture the response
    fp = popen(command, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to run curl command\n");
        response_free(resp);
        return -1;
    }

    // Read the output of the curl command into a string buffer
    while ((read = getline(&line, &len, fp)) != -1) {
        response_append(resp, line);
    }

    fclose(fp);
    if (line) {
        free(line);
    }

    // Parse the JSON response
    struct json_object *parsed_json;
    parsed_json = json_tokener_parse(response_get_memory(resp));

    fprintf(stderr, "Parsed JSON: %s\n", json_object_to_json_string(parsed_json));

    if (parsed_json == NULL) {
        fprintf(stderr, "Failed to parse JSON response\n");
        response_free(resp);
        return -1;
    }

    // Get the array of platforms
    struct json_object *platform_array = parsed_json;

    *platform_count = json_object_array_length(platform_array);
    *platform_list = malloc(*platform_count * sizeof(RomMPlatform));  // Allocate memory for platforms

    // Loop through each platform and populate the platform_list
    for (int i = 0; i < *platform_count; i++) {
        struct json_object *platform_obj = json_object_array_get_idx(platform_array, i);

        (*platform_list)[i].id = json_object_get_int(json_object_object_get(platform_obj, "id"));
        (*platform_list)[i].slug = strdup(json_object_get_string(json_object_object_get(platform_obj, "slug")));
        (*platform_list)[i].fs_slug = strdup(json_object_get_string(json_object_object_get(platform_obj, "fs_slug")));
        (*platform_list)[i].name = strdup(json_object_get_string(json_object_object_get(platform_obj, "name")));
        (*platform_list)[i].rom_count = json_object_get_int(json_object_object_get(platform_obj, "rom_count"));
        (*platform_list)[i].logo_path = strdup(json_object_get_string(json_object_object_get(platform_obj, "logo_path")));
        (*platform_list)[i].created_at = strdup(json_object_get_string(json_object_object_get(platform_obj, "created_at")));
        (*platform_list)[i].updated_at = strdup(json_object_get_string(json_object_object_get(platform_obj, "updated_at")));

        // Handle nullable fields with NULL checks
        struct json_object *igdb_id_obj = json_object_object_get(platform_obj, "igdb_id");
        (*platform_list)[i].igdb_id = igdb_id_obj == NULL ? -1 : json_object_get_int(igdb_id_obj);
        struct json_object *sgdb_id_obj = json_object_object_get(platform_obj, "sgdb_id");
        (*platform_list)[i].sgdb_id = sgdb_id_obj == NULL ? -1 : json_object_get_int(sgdb_id_obj);
        struct json_object *moby_id_obj = json_object_object_get(platform_obj, "moby_id");
        (*platform_list)[i].moby_id = moby_id_obj == NULL ? -1 : json_object_get_int(moby_id_obj);
    }

    // Clean up
    response_free(resp);
    json_object_put(parsed_json);

    return 0;
}

// Function to fetch the ROM list from the server
int fetch_rom_list(const char* server_host, const char* platform_slug, RomMRom** rom_list, int* rom_count) {
    return 0;
}

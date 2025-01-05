#include <curl/curl.h>
#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "base64.h"

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

// This is the callback function to write the response into a string
size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data) {
    size_t total_size = size * nmemb;
    strncat(data, ptr, total_size);  // Append the data to the string
    return total_size;
}

// Return the console path based on platform slug
const char* get_console_path(const char* platform_slug) {
    // This example assumes a fixed base path; modify as needed
    static char path[1024];
    // TODO
    snprintf(path, sizeof(path), "/Roms/%s", platform_slug);
    return path;
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
    CURL *curl;
    CURLcode res;
    char response[10000] = "";  // Response buffer to store the JSON data
    char url[1024];

    // Build the URL for the request
    snprintf(url, sizeof(url), "%s/api/platforms", server_host);

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (!curl) {
        fprintf(stderr, "Failed to initialize curl\n");
        curl_global_cleanup();
        return -1;
    }

    // Set the URL and the callback function
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);

    // Set the Authorization header
    char* auth_header = generate_authorization_header(username, password);
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, auth_header);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Perform the request
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return -1;
    }

    // Parse the JSON response
    struct json_object *parsed_json;
    parsed_json = json_tokener_parse(response);

    // Get the array of platforms
    struct json_object *platform_array;
    json_object_object_get_ex(parsed_json, "platforms", &platform_array);

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
    json_object_put(parsed_json);  // Free the JSON object
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}

// Function to fetch the ROM list from the server
int fetch_rom_list(const char* server_host, const char* platform_slug, RomMRom** rom_list, int* rom_count) {
    return 0;
}

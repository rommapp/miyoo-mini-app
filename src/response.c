#include "response.h"
#include <string.h>
#include <stdio.h>

#define RESPONSE_INITIAL_SIZE 4096  // Start with 4KB

struct Response {
    char* memory;     // Response buffer
    size_t size;      // Current size of data
    size_t capacity;  // Total allocated size
};

Response* response_init(void) {
    Response* resp = malloc(sizeof(struct Response));
    if (!resp) {
        return NULL;
    }
    
    resp->memory = malloc(RESPONSE_INITIAL_SIZE);
    if (!resp->memory) {
        free(resp);
        return NULL;
    }
    
    resp->size = 0;
    resp->capacity = RESPONSE_INITIAL_SIZE;
    return resp;
}

void response_free(Response* resp) {
    if (resp) {
        free(resp->memory);
        free(resp);
    }
}

static int ensure_capacity(Response* resp, size_t additional_size) {
    if (resp->size + additional_size + 1 > resp->capacity) {
        size_t new_capacity = resp->capacity;
        while (new_capacity < resp->size + additional_size + 1) {
            new_capacity *= 2;
        }
        
        char* new_memory = realloc(resp->memory, new_capacity);
        if (!new_memory) {
            return -1;
        }
        
        resp->memory = new_memory;
        resp->capacity = new_capacity;
    }
    return 0;
}

size_t response_write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    Response* resp = (Response*)userp;
    
    if (ensure_capacity(resp, realsize) != 0) {
        fprintf(stderr, "Failed to allocate memory (realloc returned NULL)\n");
        return 0;
    }
    
    memcpy(resp->memory + resp->size, contents, realsize);
    resp->size += realsize;
    resp->memory[resp->size] = '\0';
    
    return realsize;
}

const char* response_get_memory(const Response* resp) {
    return resp ? resp->memory : NULL;
}

size_t response_get_size(const Response* resp) {
    return resp ? resp->size : 0;
}

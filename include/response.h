#ifndef ROMM_RESPONSE_H
#define ROMM_RESPONSE_H

#include <stdlib.h>

// Opaque pointer to hide implementation details
typedef struct Response Response;

// Public interface
Response* response_init(void);
void response_free(Response* resp);
size_t response_write_callback(void* contents, size_t size, size_t nmemb, void* userp);
void response_append(Response* resp, const char* data);

// Getters since the structure is opaque
const char* response_get_memory(const Response* resp);
size_t response_get_size(const Response* resp);

#endif // ROMM_RESPONSE_H

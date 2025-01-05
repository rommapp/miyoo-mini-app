#ifndef ROMM_CLIENT_H
#define ROMM_CLIENT_H

#include "rom.h"
#include "platform.h"

// Structure for memory buffer
struct MemoryStruct {
    char* memory;
    size_t size;
};

// Memory handling callback
static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp);

#endif /* ROMM_CLIENT_H */

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#include "platform.h"

typedef struct {
    int display_width;
    int display_height;
    SDL_Surface* screen;
    SDL_Surface* renderer;
    TTF_Font* font;
    RomMPlatform* platforms;
    int platform_count;
    int selected_index;
    int scroll_offset;
    int last_tick_count;
    int cur_tick_count;
    char* server_url;
    char* username;
    char* password;
} MenuState;

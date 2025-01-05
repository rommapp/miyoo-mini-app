#include <stdio.h>
#include <stdlib.h>
#include "platform.h"

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define ITEM_HEIGHT 40
#define MAX_VISIBLE_ITEMS 10
#define FRAME_RATE 60.0f

typedef struct {
    SDL_Surface* screen;     // SDL1 uses Surface instead of Window+Renderer
    TTF_Font* font;
    RomMPlatform* platforms;
    int platform_count;
    int selected_index;
    int scroll_offset;
    int last_tick_count;
    int cur_tick_count;
} MenuState;

void cleanup_menu(MenuState* state) {
    if (state->font) TTF_CloseFont(state->font);
    if (state->screen) SDL_FreeSurface(state->screen);
    TTF_Quit();
    SDL_Quit();
}

int init_menu(MenuState* state) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    // SDL1 video mode initialization
    state->screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 16, 
                                   SDL_HWSURFACE | SDL_DOUBLEBUF);
    
    if (!state->screen) {
        fprintf(stderr, "Video mode could not be set! SDL_Error: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_WM_SetCaption("ROMM", NULL);  // Set window title in SDL1

    // Load font
    state->font = TTF_OpenFont("./DejaVuSans.ttf", 16);
    if (!state->font) {
        fprintf(stderr, "Failed to load font! TTF_Error: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    state->selected_index = 0;
    state->scroll_offset = 0;

    return 0;
}

void render_platform_list(MenuState* state) {
    // Fill background with black
    SDL_FillRect(state->screen, NULL, SDL_MapRGB(state->screen->format, 0, 0, 0));

    SDL_Color text_color = {255, 255, 255, 0};
    SDL_Color selected_color = {255, 255, 0, 0};

    for (int i = 0; i < state->platform_count && i < MAX_VISIBLE_ITEMS; i++) {
        int actual_index = i + state->scroll_offset;
        if (actual_index >= state->platform_count) break;

        SDL_Color current_color = (actual_index == state->selected_index) ? 
                                selected_color : text_color;

        SDL_Surface* text_surface = TTF_RenderText_Solid(state->font, 
            state->platforms[actual_index].name, 
            current_color);

        if (text_surface) {
            SDL_Rect dest_rect = {
                20,                    // x position
                i * ITEM_HEIGHT + 10,  // y position
                text_surface->w,       // width
                text_surface->h        // height
            };

            // SDL1 uses SDL_BlitSurface instead of SDL_RenderCopy
            SDL_BlitSurface(text_surface, NULL, state->screen, &dest_rect);
            SDL_FreeSurface(text_surface);
        }
    }

    // Update the screen
    SDL_Flip(state->screen);
}

void handle_input(MenuState* state, SDL_Event* event, bool* quit, bool* selected) {
    if (event->type == SDL_KEYDOWN) {
        SDLKey key = event->key.keysym.sym;
        switch (key) {
            case SDLK_UP:    // D-pad up
                if (state->selected_index > 0) {
                    state->selected_index--;
                    if (state->selected_index < state->scroll_offset) {
                        state->scroll_offset--;
                    }
                }
                break;

            case SDLK_DOWN:  // D-pad down
                if (state->selected_index < state->platform_count - 1) {
                    state->selected_index++;
                    if (state->selected_index >= state->scroll_offset + MAX_VISIBLE_ITEMS) {
                        state->scroll_offset++;
                    }
                }
                break;

            case SDLK_SPACE: // A button
                *selected = true;
                break;

            case SDLK_ESCAPE: // Menu button
                *quit = true;
                break;

            default:
                break;
        }
    }
}

int main() {
    MenuState state = {0};
    
    if (init_menu(&state) < 0) {
        return -1;
    }

    // Define the server URL and the credentials for Basic Auth
    const char* server_url = "";
    const char* username = "";
    const char* password = "";

    if (fetch_platform_list(server_url, username, password, &state.platforms, &state.platform_count) < 0) {
        cleanup_menu(&state);
        return -1;
    }

    bool quit = false;
    bool selected = false;
    SDL_Event event;

    state.last_tick_count = SDL_GetTicks();
    state.cur_tick_count = state.last_tick_count;

    float fps = 0.0f;
    int frame = state.cur_tick_count;
    int frameCount = 0;

    while (!quit && !selected) {
        while (SDL_PollEvent(&event)) {
            handle_input(&state, &event, &quit, &selected);
        }

        if (state.cur_tick_count - state.last_tick_count < 1000 / FRAME_RATE) {
            continue;
        }

        ++frameCount;
        if (state.cur_tick_count - frame >= 1000) {
            fps = frameCount / ((state.cur_tick_count - frame) / 1000.f);
            frameCount = 0;
            frame = state.cur_tick_count;
        }

        render_platform_list(&state);
    }

    if (selected) {
        // User selected a platform - fetch its games
        RomMPlatform* selected_platform = &state.platforms[state.selected_index];
        printf("Selected platform: %s\n", selected_platform->name);
        // TODO: Implement fetch_rom_list and show games menu
    }

    cleanup_menu(&state);
    return 0;
}

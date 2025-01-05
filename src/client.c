#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "platform.h"
#include "menu_state.h"

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

#define ITEM_HEIGHT 40
#define MAX_VISIBLE_ITEMS 10
#define FRAME_RATE 60.0f

void cleanup_menu(MenuState* state) {
    if (state->server_url) free(state->server_url);
    if (state->username) free(state->username);
    if (state->password) free(state->password);
    if (state->platforms) free_platform_list(state->platforms, state->platform_count);
    if (state->font) TTF_CloseFont(state->font);
    if (state->screen) SDL_FreeSurface(state->screen);
    if (state->renderer) SDL_FreeSurface(state->renderer);
    TTF_Quit();
    SDL_Quit();
}

int init_menu(MenuState* state) {
    // Initialize state
    memset(state, 0, sizeof(MenuState));

    // Initialize SDL with specific subsystems
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {  // Removed SDL_INIT_TIMER as it's not needed
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Initialize TTF
    if (TTF_Init() < 0) {
        fprintf(stderr, "TTF could not initialize! TTF_Error: %s\n", TTF_GetError());
        SDL_Quit();
        return -1;
    }

    // Get the current display width and height
    const SDL_VideoInfo *info = SDL_GetVideoInfo();
    state->display_width = info->current_w;
    state->display_height = info->current_h;

    // Load font - adjust path as needed for the Miyoo Mini
    state->font = TTF_OpenFont("/mnt/SDCARD/App/RomM/fonts/DejaVuSans.ttf", 16);
    if (!state->font) {
        fprintf(stderr, "Failed to load font! TTF_Error: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // Set the video mode
    state->screen = SDL_SetVideoMode(state->display_width, state->display_height, 32, SDL_HWSURFACE);
    if (!state->screen) {
        fprintf(stderr, "Video mode could not be set! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Create a renderer surface
    state->renderer = SDL_CreateRGBSurface(SDL_HWSURFACE, state->display_width, state->display_height, 32, 0, 0, 0, 0);
    if (!state->renderer) {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Clear the screen to black immediately after initialization
    SDL_FillRect(state->screen, NULL, SDL_MapRGB(state->screen->format, 0, 0, 0));
    SDL_Flip(state->screen);

    state->selected_index = 0;
    state->scroll_offset = 0;
    state->last_tick_count = SDL_GetTicks();
    state->cur_tick_count = state->last_tick_count;

    state->server_url = malloc(256);
    state->username = malloc(256);
    state->password = malloc(256);

    return 0;
}

void render_platform_list(MenuState* state) {
    if (!state->renderer || !state->font) return;  // Add safety check

    // Clear screen with black using the surface's format
    Uint32 black = SDL_MapRGB(state->renderer->format, 0, 0, 0);
    SDL_FillRect(state->renderer, NULL, black);

    SDL_Color text_color = {255, 255, 255, 0};
    SDL_Color selected_color = {255, 255, 0, 0};

    for (int i = 0; i < state->platform_count && i < MAX_VISIBLE_ITEMS; i++) {
        int actual_index = i + state->scroll_offset;
        if (actual_index >= state->platform_count) break;

        SDL_Color current_color = (actual_index == state->selected_index) ? 
                                selected_color : text_color;

        if (state->platforms && state->platforms[actual_index].name) {  // Add safety check
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

                SDL_BlitSurface(text_surface, NULL, state->renderer, &dest_rect);
                SDL_FreeSurface(text_surface);
            }
        }
    }

    // Blit the renderer to the screen
    SDL_BlitSurface(state->renderer, NULL, state->screen, NULL);
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

            case SDLK_RETURN: // Start button
                *quit = true;
                break;

            default:
                break;
        }
    }
}

int read_config(MenuState* state, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open config file: %s\n", filename);
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;

        if (strncmp(line, "server_url=", 11) == 0) {
            snprintf(state->server_url, 256, "%s", line + 11);
        } else if (strncmp(line, "username=", 9) == 0) {
            snprintf(state->username, 256, "%s", line + 9);
        } else if (strncmp(line, "password=", 9) == 0) {
            snprintf(state->password, 256, "%s", line + 9);
        }
    }

    fclose(file);
    return 0;
}

int main() {
    MenuState state = {0};

    if (init_menu(&state) < 0) {
        fprintf(stderr, "Failed to initialize menu\n");
        return -1;
    }

    if (read_config(&state, "/mnt/SDCARD/App/RomM/config.txt") < 0) {
        fprintf(stderr, "Failed to read configuration\n");
        return -1;
    }

    if (fetch_platform_list(state.server_url, state.username, state.password, &state.platforms, &state.platform_count) < 0) {
        fprintf(stderr, "Failed to fetch platform list\n");
        cleanup_menu(&state);
        return -1;
    }

    bool quit = false;
    bool selected = false;
    SDL_Event event;

    while (!quit && !selected) {
        state.cur_tick_count = SDL_GetTicks();
        
        while (SDL_PollEvent(&event)) {
            handle_input(&state, &event, &quit, &selected);
        }

        // Only render if enough time has passed
        if (state.cur_tick_count - state.last_tick_count >= (1000 / FRAME_RATE)) {
            render_platform_list(&state);
            state.last_tick_count = state.cur_tick_count;
        } else {
            SDL_Delay(1);  // Give up some CPU time
        }
    }

    if (selected && state.selected_index < state.platform_count) {
        RomMPlatform* selected_platform = &state.platforms[state.selected_index];
        printf("Selected platform: %s\n", selected_platform->name);
    }

    cleanup_menu(&state);
    return 0;
}

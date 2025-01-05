#ifndef ROMM_CLIENT_H
#define ROMM_CLIENT_H

#include "rom.h"
#include "menu_state.h"

#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"

int init_menu(MenuState* state);
void cleanup_menu(MenuState* state);
void render_platform_list(MenuState* state);
void handle_input(MenuState* state, SDL_Event* event, bool* quit, bool* selected);
int read_config(MenuState* state, const char* config_file);

#endif /* ROMM_CLIENT_H */

#ifndef SDL_keycode_h_
#define SDL_keycode_h_

#define SDL_Keycode int

enum
{
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_a,
    SDLK_c,
    SDLK_d,
    SDLK_e,
    SDLK_s,
    SDLK_z,
    SDLK_UP,
    SDLK_DOWN,
    SDLK_LEFT,
    SDLK_RIGHT,
    SDLK_r,
    SDLK_TAB,
    SDLK_LSHIFT,
    SDLK_ESCAPE,
    SDLK_F5,
    SDLK_F6,
    SDLK_F7,
    SDLK_F8,
    SDLK_F9,
    SDLK_F10,
    SDLK_F11,
    SDLK_F12
};

enum
{
    SDL_BUTTON_LEFT = 1,
    SDL_BUTTON_MIDDLE,
    SDL_BUTTON_RIGHT,
};

static inline const char* SDL_GetKeyName(SDL_Keycode key)
{
    return "N";
}

#endif
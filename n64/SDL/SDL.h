#ifndef SDL_h_
#define SDL_h_

#include <stdlib.h>
#include "SDL_keycode.h"

#define SDL_Surface void
#define Uint32 uint32_t

static inline int SDL_GetNumVideoDisplays()
{
    return 1;
}

static inline const char * SDL_GetDisplayName(int displayIndex)
{
    return "Libdragon";
}

static inline void* SDL_GL_GetProcAddress(const char *proc)
{
    return NULL;
}

#endif
#ifndef SDL_ttf_h_
#define SDL_ttf_h_

#define TTF_Font void

static inline int TTF_Init(void)
{
    return 0;
}

static inline void TTF_Quit(void)
{

}

static inline int TTF_SizeUTF8(TTF_Font *font, const char *text, int *w, int *h)
{
    *w = 0;
    *h = 0;
    return 0;
}


static inline void TTF_CloseFont(TTF_Font *font)
{
    return;
}



#endif
#include <libdragon.h>
#include "version.h"
#include "glext.h"
#include "config.h"
#include "video.h"
#include "image.h"
#include "audio.h"
#include "progress.h"
#include "gui.h"
#include "set.h"
#include "tilt.h"
#include "hmd.h"
#include "fs.h"
#include "common.h"
#include "text.h"
#include "mtrl.h"
#include "geom.h"
#include "joy.h"
#include "fetch.h"
#include "package.h"

#include "st_conf.h"
#include "st_title.h"
#include "st_demo.h"
#include "st_level.h"
#include "st_pause.h"

typedef struct sram_files_t
{
    const char *name;
    uint32_t size;
    uint32_t offset; //Track position of the file cursor
} sram_files_t;
int sramfs_init(sram_files_t *files, int num_files);

#define MAX_SRAM_FILES 1
//FIXME: Add scores probably - not worried about replays, screenshots or downloads
//need to fix config_read and config_load functions if we want configs? probably not
static sram_files_t sram_files[MAX_SRAM_FILES] = {
    {"neverballrc", 4096, 0},
};

struct main_loop
{
    uint32_t now;
    unsigned int done:1;
};
static void step(void *data);

static inline uint32_t ticks()
{
    return timer_ticks() / (TICKS_PER_SECOND / 1000);
}

int main(void)
{
    struct main_loop mainloop = { 0 };

    dfs_init(DFS_DEFAULT_LOCATION);
    debug_init(DEBUG_FEATURE_LOG_ISVIEWER);
    timer_init();

    fs_init("rom:/neverball/data");
    config_paths("rom:/neverball/data");

    joy_init();
    config_init();
    config_load();
    lang_init();
    nb_audio_init();
    video_init();
    mtrl_init();
    init_state(&st_null);
    goto_state(&st_title);
    mainloop.now = ticks();
    mainloop.done = 0;
    while (!mainloop.done)
    {
        step(&mainloop);
    }
        

    return 0;
}

static int loop(void)
{
    joy_button(0, 0, 0);
    joy_axis(0, 0, 0);
    return 1;
}

static void step(void *data)
{
    struct main_loop *mainloop = (struct main_loop *) data;

    int running = loop();

    if (running)
    {
        uint32_t now = ticks();
        uint32_t dt = (now - mainloop->now);

        if (0 < dt && dt < 1000)
        {
            /* Step the game state. */
            st_timer(0.001f * dt);
            /* Render. */
            hmd_step();
            st_paint(0.001f * now);
            video_swap();
        }

        mainloop->now = now;
    }

    mainloop->done = !running;
}

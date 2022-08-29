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

    controller_init();
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
    struct controller_data keys_down, keys_up, keys;
    float x, y;
    controller_scan();

    keys_down = get_keys_down();
    keys_up = get_keys_up();
    keys = get_keys_pressed();

    if (keys_down.c[0].A) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_A), 1);
    if (keys_down.c[0].B) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_B), 1);
    if (keys_down.c[0].start) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_START), 1);

    if (keys_up.c[0].A) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_A), 0);
    if (keys_up.c[0].B) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_B), 0);
    if (keys_up.c[0].start) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_START), 0);

    x = (float)(keys.c[0].x) / 128.0f;
    y = (float)(keys.c[0].y) / 128.0f;
    st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_X0), x);
    st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_Y0), y);

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

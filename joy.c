#include <libdragon.h>
#include "share/config.h"

void joy_init()
{
    controller_init();
}

int joy_button(int instance, int b, int d)
{
    (void) instance;
    (void) b;
    (void) d;

    controller_scan();
    struct controller_data keys_down = get_keys_down();
    struct controller_data keys_up = get_keys_up();

    if (keys_down.c[0].A) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_A), 1);
    if (keys_down.c[0].B) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_B), 1);
    if (keys_down.c[0].start) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_START), 1);

    if (keys_up.c[0].A) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_A), 0);
    if (keys_up.c[0].B) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_B), 0);
    if (keys_up.c[0].start) st_buttn(config_get_d(CONFIG_JOYSTICK_BUTTON_START), 0);

    return 1;
}

void joy_axis(int instance, int a, float v)
{
    (void) instance;
    (void) a;
    (void) v;

    struct controller_data keys_down = get_keys_pressed();
    float x = (float)(keys_down.c[0].x) / 128.0f;
    float y = (float)(keys_down.c[0].y) / 128.0f;
    st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_X0), x);
    st_stick(config_get_d(CONFIG_JOYSTICK_AXIS_Y0), y);
}

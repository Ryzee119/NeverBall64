#include <libdragon.h>
void joy_init()
{
}

int joy_button(int instance, int b, int d)
{
    (void) instance;
    (void) b;
    (void) d;
    return 0;
    //Work out what buttons have changed this last call and set the new button state for each change..
    //return st_buttn(button, state);
    //button = config_get_d(CONFIG_JOYSTICK_BUTTON_A) \
               config_get_d(CONFIG_JOYSTICK_BUTTON_B_) \
               config_get_d(CONFIG_JOYSTICK_BUTTON_START
    //state = 0 or 1
}

void joy_axis(int instance, int a, float v)
{
    (void) instance;
    (void) a;
    (void) v;
    //Set new stick state.
    //st_stick(dir, value); //0-1.0f
    //dir = config_get_d(CONFIG_JOYSTICK_AXIS_X0) \
            config_get_d(CONFIG_JOYSTICK_AXIS_Y0)
}

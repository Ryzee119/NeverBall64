#include <libdragon.h>
#include <GL/gl.h>
#include <GL/gl_integration.h>

#include "share/common.h"
#include "share/image.h"
#include "share/vec3.h"
#include "share/glext.h"
#include "share/config.h"
#include "share/gui.h"
#include "share/hmd.h"

struct video video;

/*---------------------------------------------------------------------------*/

/* Normally...... show the system cursor and hide the virtual cursor.        */
/* In HMD mode... show the virtual cursor and hide the system cursor.        */

void video_show_cursor()
{
}

/* When the cursor is to be hidden, make sure neither the virtual cursor     */
/* nor the system cursor is visible.                                         */

void video_hide_cursor()
{
}

void video_snap(const char *path)
{
}

/*
 * Enter/exit fullscreen mode.
 */
int video_fullscreen(int f)
{
    return 1;
}

/*
 * Handle a window resize event.
 */
void video_resize(int window_w, int window_h)
{
    video.window_w = window_w;
    video.window_h = window_h;
    video.device_w = video.window_w;
    video.device_h = video.window_h;
    video.device_scale = (float) video.device_h / (float) video.window_h;
    glViewport(0, 0, video.device_w, video.device_h);    
}

void video_set_window_size(int w, int h)
{
}

int video_display(void)
{
    return 0;
}

int video_init(void)
{
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 1, GAMMA_NONE, ANTIALIAS_RESAMPLE_FETCH_ALWAYS);
    gl_init();

    video_mode(config_get_d(CONFIG_FULLSCREEN),
               config_get_d(CONFIG_WIDTH),
               config_get_d(CONFIG_HEIGHT));
    return 1;
}

void video_quit(void)
{

}

int video_mode(int f, int w, int h)
{
    config_set_d(CONFIG_DISPLAY,    video_display());
    config_set_d(CONFIG_FULLSCREEN, f);
    glext_init();

    video_resize(w, h);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float aspect_ratio = (float)w/(float)h;
    glFrustum(-1*aspect_ratio, 1*aspect_ratio, -1, 1, 1, 100);

    #warning Upstream has GL_LEQUAL which is not supported here

    glDepthFunc(GL_LESS);
    return 0;
}

/*---------------------------------------------------------------------------*/

static float ms     = 0;
static int   fps    = 0;
static int   last   = 0;
static int   ticks  = 0;
static int   frames = 0;

int  video_perf(void)
{
    return fps;
}

void video_swap(void)
{
    int dt;
    gl_swap_buffers();

    /* Accumulate time passed and frames rendered. */

    dt = (int) timer_ticks() / (TICKS_PER_SECOND / 1000);

    frames +=  1;
    ticks  += dt;
    last   += dt;

    /* Average over 250ms. */

    if (ticks > 1000)
    {
        /* Round the frames-per-second value to the nearest integer. */

        double k = 1000.0 * frames / ticks;
        double f = floor(k);
        double c = ceil (k);

        /* Compute frame time and frames-per-second stats. */

        fps = (int) ((c - k < k - f) ? c : f);
        ms  = (float) ticks / (float) frames;

        /* Reset the counters for the next update. */

        frames = 0;
        ticks  = 0;

        //fprintf(stderr, "%4d %8.4f\n", fps, (double) ms);
            
    }
}

/*---------------------------------------------------------------------------*/

void video_set_grab(int w)
{
}

void video_clr_grab(void)
{
}

int  video_get_grab(void)
{
    return 1;
}

/*---------------------------------------------------------------------------*/

void video_calc_view(float *M, const float *c,
                               const float *p,
                               const float *u)
{
    float x[3];
    float y[3];
    float z[3];

    v_sub(z, p, c);
    v_nrm(z, z);
    v_crs(x, u, z);
    v_nrm(x, x);
    v_crs(y, z, x);

    m_basis(M, x, y, z);
}

/*---------------------------------------------------------------------------*/

void video_push_persp(float fov, float n, float f)
{
    if (hmd_stat())
        hmd_persp(n, f);
    else
    {
        GLfloat m[4][4];

        GLfloat r = fov / 2 * V_PI / 180;
        GLfloat s = fsinf(r);
        GLfloat c = fcosf(r) / s;

        GLfloat a = ((GLfloat) video.device_w /
                     (GLfloat) video.device_h);

        glMatrixMode(GL_PROJECTION);
        {
            glLoadIdentity();

            m[0][0] = c / a;
            m[0][1] =  0.0f;
            m[0][2] =  0.0f;
            m[0][3] =  0.0f;
            m[1][0] =  0.0f;
            m[1][1] =     c;
            m[1][2] =  0.0f;
            m[1][3] =  0.0f;
            m[2][0] =  0.0f;
            m[2][1] =  0.0f;
            m[2][2] = -(f + n) / (f - n);
            m[2][3] = -1.0f;
            m[3][0] =  0.0f;
            m[3][1] =  0.0f;
            m[3][2] = -2.0f * n * f / (f - n);
            m[3][3] =  0.0f;

            glMultMatrixf(&m[0][0]);
        }
        glMatrixMode(GL_MODELVIEW);
        {
            glLoadIdentity();
        }
    }
}

void video_push_ortho(void)
{
    if (hmd_stat())
        hmd_ortho();
    else
    {
        GLfloat w = (GLfloat) video.device_w;
        GLfloat h = (GLfloat) video.device_h;

        glMatrixMode(GL_PROJECTION);
        {
            glLoadIdentity();
            glOrtho_(0.0, w, 0.0, h, -1.0, +1.0);
        }
        glMatrixMode(GL_MODELVIEW);
        {
            glLoadIdentity();
        }
    }
}

void video_pop_matrix(void)
{
}

void video_clear(void)
{
    if (config_get_d(CONFIG_REFLECTION))
        glClear(GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);
    else
        glClear(GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT);
}

/*---------------------------------------------------------------------------*/

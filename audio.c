#include <libdragon.h>

//Had to rename from audio_init due to conflict with libdragon's audio_init
void nb_audio_init(void)
{
}

void audio_free()
{
}

void audio_play(const char *filename, float a)
{
}

void audio_music_queue(const char *filename, float t)
{
}

void audio_music_play(const char *filename)
{
}

void audio_music_stop()
{
}

void audio_music_fade_to(float t, const char *filename)
{
}

void audio_music_fade_in(float t)
{
}
void audio_music_fade_out(float t)
{
}

void audio_volume(int s, int m)
{
}
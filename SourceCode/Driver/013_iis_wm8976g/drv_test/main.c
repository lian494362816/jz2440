#if 0
#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>

snd_pcm_t *open_sound_dev(snd_pcm_stream_t type)
{
    int err;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *hw_params;
    unsigned int rate = 44100;

    if ((err = snd_pcm_open (&handle, "default", type, 0)) < 0) {
        return NULL;
    }
       
    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
        fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n",
             snd_strerror (err));
        return NULL;
    }
             
    if ((err = snd_pcm_hw_params_any (handle, hw_params)) < 0) {
        fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_access (handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf (stderr, "cannot set access type (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_format (handle, hw_params, SND_PCM_FORMAT_S16_LE)) < 0) {
        fprintf (stderr, "cannot set sample format (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_rate_near (handle, hw_params, &rate, 0)) < 0) {
        fprintf (stderr, "cannot set sample rate (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_channels (handle, hw_params, 2)) < 0) {
        fprintf (stderr, "cannot set channel count (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    if ((err = snd_pcm_hw_params (handle, hw_params)) < 0) {
        fprintf (stderr, "cannot set parameters (%s)\n",
             snd_strerror (err));
        return NULL;
    }

    snd_pcm_hw_params_free (hw_params);

    return handle;
}

void close_sound_dev(snd_pcm_t *handle)
{
    snd_pcm_close (handle);
}

snd_pcm_t *open_playback(void)
{
    return open_sound_dev(SND_PCM_STREAM_PLAYBACK);
}

snd_pcm_t *open_capture(void)
{
    return open_sound_dev(SND_PCM_STREAM_CAPTURE);
}


int main (int argc, char *argv[])
{
    int err;
    char buf[512];
    snd_pcm_t *playback_handle;
    snd_pcm_t *capture_handle;


    playback_handle = open_playback();
    if (!playback_handle)
    {
        fprintf (stderr, "cannot open for playback\n");
        return -1;
    }


    capture_handle = open_capture();
    if (!capture_handle)
    {
        fprintf (stderr, "cannot open for capture\n");
        return -1;
    }

    if ((err = snd_pcm_prepare (playback_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
        return -1;
    }

    if ((err = snd_pcm_prepare (capture_handle)) < 0) {
        fprintf (stderr, "cannot prepare audio interface for use (%s)\n",
             snd_strerror (err));
        return -1;
    }

    while (1) {
        if ((err = snd_pcm_readi (capture_handle, buf, 128)) != 128) {
            fprintf (stderr, "read from audio interface failed (%s)\n",
                 snd_strerror (err));
            return -1;
        }

        if ((err = snd_pcm_writei (playback_handle, buf, 128)) != 128) {
            fprintf (stderr, "write to audio interface failed (%s)\n",
                 snd_strerror (err));
            return -1;
        }
    }

    snd_pcm_close (playback_handle);
    snd_pcm_close (capture_handle);
    return 0;
}
#endif

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define WAV_FILE   "/music.wav"
#define AUDIO_FILE "/dev/wm8976"


int main(int argc, char *argv)
{
    int wav_fd = 0;
    int play_fd = 0;
    int tmp_buffer[1024];
    int ret = 0;
    int i = 0;

    play_fd = open(AUDIO_FILE, O_WRONLY);
    if (play_fd < 0)
    {
        printf("open %s fail \n", AUDIO_FILE);
        return -1;
    }

    wav_fd = open(WAV_FILE, O_RDWR);
    if (wav_fd < 0)
    {
        close(play_fd);
        printf("open %s fail \n", WAV_FILE);
        return -1;
    }
    printf("wav_fd:%d\n", wav_fd);
    printf("play_fd:%d \n", play_fd);

    while(1)
    {
        ret = read(wav_fd, tmp_buffer, 1024);
        if (ret > 0)
        {
            write(play_fd, tmp_buffer, ret); 
            /* printf("%d \n", ++i); */
        }
        else
        {
            printf("%s \n", strerror(errno));

            break;
        }
    }
            
    close(play_fd);
    close(wav_fd);

    return 0;
}



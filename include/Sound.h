/*
 * Sound.h
 *
 *  Created on: Aug 20, 2015
 *      Author: messier
 */

#ifndef SOUND_H_
#define SOUND_H_
#include <alsa/asoundlib.h>
#include <stdio.h>
#define BUFFER_SIZE 	16*1024
namespace iHearTech {

class Sound {
public:
	Sound();
	virtual ~Sound();
	static int play(FILE *wav_file);
	static int init();
	static int set_hwparams(snd_pcm_t *handle, snd_pcm_hw_params_t *params, snd_pcm_access_t access);
	static int set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams);
private:
	static char *device;					/* playback device */
	static snd_pcm_format_t format;			/* sample format */
	static unsigned int rate;				/* stream rate */
	static unsigned int channels;			/* count of channels */
	static unsigned int buffer_time;		/* ring buffer length in us */
	static unsigned int period_time;		/* period time in us */
	static int verbose;						/* verbose flag */
	static int resample;					/* enable alsa-lib resampling */
	static int period_event;				/* produce poll event after each period */

	static snd_pcm_sframes_t buffer_size;
	static snd_pcm_sframes_t period_size;
	static snd_output_t *output;


};

} /* namespace iHearTech */

#endif /* SOUND_H_ */

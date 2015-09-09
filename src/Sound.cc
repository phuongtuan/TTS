/*
 * Sound.cc
 *
 *  Created on: Aug 20, 2015
 *      Author: messier
 */

#include <Sound.h>
#include "debug.h"

namespace iHearTech {

char *Sound::device = (char*)"default";					/* playback device */
snd_pcm_format_t Sound::format = SND_PCM_FORMAT_S16;	/* sample format */
unsigned int Sound::rate = 16000;						/* stream rate */
unsigned int Sound::channels = 1;						/* count of channels */
unsigned int Sound::buffer_time = 500000;				/* ring buffer length in us */
unsigned int Sound::period_time = 100000;				/* period time in us */
int Sound::verbose = 0;									/* verbose flag */
int Sound::resample = 1;								/* enable alsa-lib resampling */
int Sound::period_event = 0;							/* produce poll event after each period */
snd_pcm_sframes_t Sound::buffer_size;
snd_pcm_sframes_t Sound::period_size;

Sound::Sound() {
	// TODO Auto-generated constructor stub
}

Sound::~Sound() {
	// TODO Auto-generated destructor stub
}

int Sound::play(FILE *wav_file){
	if(wav_file == NULL) return -1;
	return Sound::play_file(wav_file);
}

int Sound::play(std::string file_path){
	FILE *wav_file;
	int ret;
	if((wav_file = fopen(file_path.c_str(), "r+b")) == NULL){
		return -1;
	}
	ret = Sound::play_file(wav_file);
	fclose(wav_file);
	return ret;
}

int Sound::play(const char* file_path){
	FILE *wav_file;
	int ret;
	if((wav_file = fopen(file_path, "r+b")) == NULL){
		return -1;
	}
	ret = Sound::play_file(wav_file);
	fclose(wav_file);
	return ret;
}

int Sound::play_file(FILE *wav_file){
	DEBUG_INFO("Playing wav file");
	snd_pcm_t *handle;
	int err;
	snd_pcm_hw_params_t *hwparams;
	snd_pcm_sw_params_t *swparams;
	signed short *samples;
	snd_pcm_hw_params_alloca(&hwparams);
	snd_pcm_sw_params_alloca(&swparams);
	if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		DEBUG_ERROR("Playback open error: %s\n", snd_strerror(err));
		return 0;
	}
	if ((err = set_hwparams(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		DEBUG_ERROR("Setting of hwparams failed: %s\n", snd_strerror(err));
		return 0;
	}
	if ((err = set_swparams(handle, swparams)) < 0) {
		DEBUG_ERROR("Setting of swparams failed: %s\n", snd_strerror(err));
		return 0;
	}
	int samples_size = (period_size * channels * snd_pcm_format_physical_width(format)) / 8;
	samples = (signed short *)malloc(samples_size);
	if (samples == NULL) {
		DEBUG_ERROR("No enough memory\n");
		return -1;
	}
	int byte_read;
	while(feof(wav_file) == 0){
		byte_read = fread(samples, sizeof(char), samples_size, wav_file);
		signed short *ptr = samples;
		int cptr;
		if(byte_read < samples_size){
			cptr = byte_read / ((channels * snd_pcm_format_physical_width(format))/8);
		}else cptr = period_size;
		while(cptr > 0){
			err = snd_pcm_writei(handle, ptr, cptr);
			if (err < 0) {
				err = snd_pcm_recover(handle, err, 0);
			}
			if(err < 0){
				DEBUG_ERROR("snd_pcm_writei failed: %s\n", snd_strerror(err));
				break;
			}
			ptr += err * channels;
			cptr -= err;
		}
	}
	memset(samples, 0, samples_size);
	for(int i = 10; i > 0; i--)snd_pcm_writei(handle, samples, period_size);
	snd_pcm_close(handle);
	free(samples);
	return 0;
}

/*
 * Set master volume
 * TODO: This method not work on ARM!
 */
void Sound::setMasterVolume(int volume)
{
	long min, max;
	snd_mixer_t *handle;
	snd_mixer_selem_id_t *sid;
	const char *card = "default";
	const char *selem_name = "Master";

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, card);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, selem_name);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

	snd_mixer_close(handle);
}

int Sound::set_hwparams(snd_pcm_t *handle,
		snd_pcm_hw_params_t *params,
		snd_pcm_access_t access)
{
	unsigned int rrate;
	snd_pcm_uframes_t size;
	int err, dir;

	/* choose all parameters */
	err = snd_pcm_hw_params_any(handle, params);
	if (err < 0) {
		DEBUG_ERROR("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
		return err;
	}
	/* set hardware resampling */
	err = snd_pcm_hw_params_set_rate_resample(handle, params, Sound::resample);
	if (err < 0) {
		DEBUG_ERROR("Resampling setup failed for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* set the interleaved read/write format */
	err = snd_pcm_hw_params_set_access(handle, params, access);
	if (err < 0) {
		DEBUG_ERROR("Access type not available for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* set the sample format */
	err = snd_pcm_hw_params_set_format(handle, params, format);
	if (err < 0) {
		DEBUG_ERROR("Sample format not available for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* set the count of channels */
	err = snd_pcm_hw_params_set_channels(handle, params, channels);
	if (err < 0) {
		DEBUG_ERROR("Channels count (%i) not available for playbacks: %s\n", channels, snd_strerror(err));
		return err;
	}
	/* set the stream rate */
	rrate = rate;
	err = snd_pcm_hw_params_set_rate_near(handle, params, &rrate, 0);
	if (err < 0) {
		DEBUG_ERROR("Rate %iHz not available for playback: %s\n", rate, snd_strerror(err));
		return err;
	}
	if (rrate != rate) {
		DEBUG_ERROR("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
		return -EINVAL;
	}
	/* set the buffer time */
	err = snd_pcm_hw_params_set_buffer_time_near(handle, params, &buffer_time, &dir);
	if (err < 0) {
		DEBUG_ERROR("Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
		return err;
	}
	err = snd_pcm_hw_params_get_buffer_size(params, &size);
	if (err < 0) {
		DEBUG_ERROR("Unable to get buffer size for playback: %s\n", snd_strerror(err));
		return err;
	}
	buffer_size = size;
	/* set the period time */
	err = snd_pcm_hw_params_set_period_time_near(handle, params, &period_time, &dir);
	if (err < 0) {
		DEBUG_ERROR("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
		return err;
	}
	err = snd_pcm_hw_params_get_period_size(params, &size, &dir);
	if (err < 0) {
		DEBUG_ERROR("Unable to get period size for playback: %s\n", snd_strerror(err));
		return err;
	}
	period_size = size;
	/* write the parameters to device */
	err = snd_pcm_hw_params(handle, params);
	if (err < 0) {
		DEBUG_ERROR("Unable to set hw params for playback: %s\n", snd_strerror(err));
		return err;
	}
	return 0;
}

int Sound::set_swparams(snd_pcm_t *handle, snd_pcm_sw_params_t *swparams)
{
	int err;
	/* get the current swparams */
	err = snd_pcm_sw_params_current(handle, swparams);
	if (err < 0) {
		DEBUG_ERROR("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* start the transfer when the buffer is almost full: */
	/* (buffer_size / avail_min) * avail_min */
	err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);
	if (err < 0) {
		DEBUG_ERROR("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* allow the transfer when at least period_size samples can be processed */
	/* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
	err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_event ? buffer_size : period_size);
	if (err < 0) {
		DEBUG_ERROR("Unable to set avail min for playback: %s\n", snd_strerror(err));
		return err;
	}
	/* enable period events when requested */
	if (period_event) {
		err = snd_pcm_sw_params_set_period_event(handle, swparams, 1);
		if (err < 0) {
			DEBUG_ERROR("Unable to set period event: %s\n", snd_strerror(err));
			return err;
		}
	}
	/* write the parameters to the playback device */
	err = snd_pcm_sw_params(handle, swparams);
	if (err < 0) {
		DEBUG_ERROR("Unable to set sw params for playback: %s\n", snd_strerror(err));
		return err;
	}
	return 0;
}


} /* namespace iHearTech */

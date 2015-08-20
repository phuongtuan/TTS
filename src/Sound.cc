/*
 * Sound.cc
 *
 *  Created on: Aug 20, 2015
 *      Author: messier
 */

#include <Sound.h>
#include "debug.h"

namespace iHearTech {

Sound::Sound() {
	// TODO Auto-generated constructor stub
}

Sound::~Sound() {
	// TODO Auto-generated destructor stub
}

int Sound::play(FILE *wav_file){
	DEBUG_INFO("Playing wav file");
	int err;
	snd_pcm_t *handle;
	snd_pcm_sframes_t frames;
	if(wav_file == NULL){
		DEBUG_ERROR("wav_file == NULL, return now");
		return -1;
	}
	if ((err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		DEBUG_ERROR("Playback open error: %s\n", snd_strerror(err));
		return -1;
	}
	if ((err = snd_pcm_set_params(handle,
			SND_PCM_FORMAT_S16_LE,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			1,
			16000,
			1,
			500000)) < 0) {	/* 0.5sec */
		DEBUG_ERROR("Playback open error: %s\n", snd_strerror(err));
		return -1;
	}
	unsigned char *buffer = new unsigned char [BUFFER_SIZE];
	if(buffer == NULL){
		DEBUG_ERROR("Cannot allocate buffer memory");
		return -1;
	}
	fseek(wav_file, 44, SEEK_SET);
	while(feof(wav_file) == 0){
		memset(buffer, 0, BUFFER_SIZE);
		fread((void*)buffer, sizeof(char), BUFFER_SIZE, wav_file);
		frames = snd_pcm_writei(handle, buffer, BUFFER_SIZE);
		if (frames < 0)
			frames = snd_pcm_recover(handle, frames, 0);
		if (frames < 0) {
			DEBUG_ERROR("snd_pcm_writei failed: %s\n", snd_strerror(err));
			break;
		}
	}
	snd_pcm_close(handle);
	delete[] buffer;
	return 0;
}

} /* namespace iHearTech */

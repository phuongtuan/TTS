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
private:
	snd_output_t *output = NULL;
};

} /* namespace iHearTech */

#endif /* SOUND_H_ */

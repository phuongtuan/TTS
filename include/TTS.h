/*
 * TTS.h
 *
 *  Created on: Jul 1, 2015
 *      Author: messier
 */

#ifndef TTS_H_
#define TTS_H_
#include <string>
#include <stdio.h>
#include "TextObjectTTS.h"
#include "debug.h"
#include "TextObjectTTS.h"
#include "UnitSelector.h"

namespace iHearTech {

class TTS {
public:
	TTS();
	virtual ~TTS();
	void sayText(char* text);
	void sayUrl(char* url);
	void outputWav(char* file_path);
private:

};

} /* namespace iHearTech */

#endif /* TTS_H_ */

/*
 * TTS.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: messier
 */
#include "TTS.h"
#include "TextObjectTTS.h"
#include "UnitSelector.h"
#include "Sound.h"
#include "debug.h"

namespace iHearTech {

TTS::TTS() {
	// TODO Auto-generated constructor stub
	this->text_obj = new TextObjectTTS();
	this->unit_sel = new UnitSelector();
	unit_sel->restoreMaps();
	this->TTS_SYS_ROOT = std::string(getenv("TTS_SYS_ROOT"));
}

TTS::~TTS() {
	// TODO Auto-generated destructor stub
	delete this->text_obj;
	delete this->unit_sel;
}

void TTS::sayText(const char* text){
	if(text == NULL){
		return;
	}
	text_obj->setInputStr(std::string(text));
	text_obj->normalize();
	unit_sel->createIdList(text_obj->getOutputStr());
	unit_sel->createWavFile(TTS_SYS_ROOT + "/tts_wav.wav");
	Sound::play(TTS_SYS_ROOT + "/tts_wav.wav");
}

void TTS::sayUrl(const char* url){
	if(url == NULL){
		return;
	}
	text_obj->getTextFromUrl(std::string(url));
	text_obj->normalize();
	unit_sel->createIdList(text_obj->getOutputStr());
	unit_sel->createWavFile(TTS_SYS_ROOT + "/tts_wav.wav");
	Sound::play(TTS_SYS_ROOT + "/tts_wav.wav");
}

void TTS::sayUrl(const std::string url){
	if(url.size() == 0) return;
	text_obj->getTextFromUrl(url);
	text_obj->normalize();
	unit_sel->createIdList(text_obj->getOutputStr());
	unit_sel->createWavFile(TTS_SYS_ROOT + "/tts_wav.wav");
	Sound::play(TTS_SYS_ROOT + "/tts_wav.wav");
}

void TTS::sayFile(FILE* pFile){
	if(pFile == NULL) return;
	int size;
	fseek(pFile, 0, SEEK_END);
	size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	char *buffer = (char*)calloc(size, sizeof(char));
	fread(buffer, sizeof(char), size, pFile);
	TTS::sayText(buffer);
}

void TTS::sayFile(const char *file_path){
	if(file_path == NULL) return;
	FILE *pFile;
	if((pFile = fopen(file_path, "r+b")) == NULL) return;
	TTS::sayFile(pFile);
}

void TTS::sayFile(const std::string file_path){
	if(file_path.size() == 0) return;
	TTS::sayFile(file_path.c_str());
}
} /* namespace iHearTech */



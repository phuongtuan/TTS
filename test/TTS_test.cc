/*
 * TTS_test.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: messier
 */
#include "TTS.h"
#include "debug.h"
#include "TTS.h"

using namespace iHearTech;
using namespace std;

void initializeComponent(){
	DEBUG_INIT();
}

int main(int argc, char* argv[]){
	initializeComponent();
	TTS tts;
	tts.sayUrl("http://tuoitre.vn/tin/thu-phi-xe-may-khong-hieu-qua/955743.html");

	return 0;
}

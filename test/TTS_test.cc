/*
 * TTS_test.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: messier
 */
#define TTS_VERSION_MAJOR		1
#define TTS_VERSION_MINOR		0
#define TTS_VERSION_SUBMINOR	1

#include "TTS.h"
#include "debug.h"
#include "TTS.h"
#include <getopt.h>
#include <alsa/asoundlib.h>
#include <stdio.h>

using namespace iHearTech;
using namespace std;

void initializeComponent(){
	char *current_path = (char*)calloc(512, sizeof(char));
	getcwd(current_path, 512);
	strcat(current_path, "/../");
	setenv("TTS_SYS_ROOT", current_path, 0);
	DEBUG_INIT();
}

void print_help(){
	printf("Usage: TTS [OPTION]... [FILE]...\n"
			"-h, --help          help\n"
			"-v, --version       print current version\n"
			"-u, --url           say text from URL\n"
			"-t, --text          say text file\n"
			"-o, --output        set output .wav file path\n"
			"\n");
}

void print_version(){
	printf("iHearTech Text-to-Speech Application v%d.%d.%d\n\n", TTS_VERSION_MAJOR,
				TTS_VERSION_MINOR, TTS_VERSION_SUBMINOR);
}

int main(int argc, char* argv[]){
	initializeComponent();
	struct option long_option[] =
	{
		{"help", 0, NULL, 'h'},
		{"version", 1, NULL, 'v'},
		{"url", 1, NULL, 'u'},
		{"text", 1, NULL, 't'},
		{"output", 1, NULL, 'o'},
		{NULL, 0, NULL, 0},
	};
	int more_help = 0;
	int more_version = 0;
	std::string url;
	std::string text_path;
	std::string output_path = std::string(getenv("TTS_SYS_ROOT")) + "/tts_out.wav";
	while(1){
		int c;
		if((c = getopt_long(argc, argv, "hvu:t:o:", long_option, NULL)) < 0){
			break;
		}
		switch(c){
		case 'h':
			more_help++;
			break;
		case 'v':
			more_version++;
			break;
		case 'u':
			if(optarg != NULL) url = std::string(optarg);
			break;
		case 't':
			if(optarg != NULL) text_path = std::string(optarg);
			break;
		case 'o':
			if(optarg != NULL) output_path = std::string(optarg);
			break;
		}
	}
	if(more_help > 0){
		print_help();
		return 0;
	}
	if(more_version > 0){
		print_version();
		return 0;
	}
	TTS tts;
	if(url.length() != 0){
		tts.sayUrl(url);
		return 0;
	}else if(text_path.length() != 0){
		tts.sayFile(text_path);
		return 0;
	}
	return 0;
}

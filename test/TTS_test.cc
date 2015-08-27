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
#include <stdio.h>
#include <unistd.h>

using namespace iHearTech;
using namespace std;

void initializeComponent(){
	char *current_path = (char*)calloc(512, sizeof(char));
	getcwd(current_path, 512);
	strcat(current_path, "/../");
	setenv("TTS_SYS_ROOT", current_path, 0);
	DEBUG_INIT();
	free(current_path);
}

void print_help(){
	printf("Usage: TTS [OPTION]... [FILE|URL]...\n"
			"-h, --help              help\n"
			"-v, --version           print current version\n"
			"-u, --url               say text from URL\n"
			"-t, --text              say text file\n"
			"-o, --output            set output .wav file path\n"
			"-d, --debug             1: Enable debug (default)\n"
			"                        0: Disable debug\n"
			"-r, --unresolved        output unresolved word list to file\n"
			"-p, --play-enable       1: Play .wav file after synthesized (default)\n"
			"                        0: Don't play .wav file after synthesized\n"
			"\n");
}

void print_version(){
	printf("iHearTech Vietnamese Text-to-Speech v%d.%d.%d\n\n", TTS_VERSION_MAJOR,
				TTS_VERSION_MINOR, TTS_VERSION_SUBMINOR);
}

int main(int argc, char* argv[]){
	initializeComponent();
	TTS tts;
	struct option long_option[] =
	{
		{"help", 0, NULL, 'h'},
		{"version", 1, NULL, 'v'},
		{"url", 1, NULL, 'u'},
		{"text", 1, NULL, 't'},
		{"output", 1, NULL, 'o'},
		{"debug", 1, NULL, 'd'},
		{"unresolved", 1, NULL, 'r'},
		{"play-enable", 1, NULL, 'p'},
		{NULL, 0, NULL, 0},
	};
	int more_help = 0;
	int more_version = 0;

	std::string unresolved_output_path;
	std::string url;
	std::string text_path;
	std::string output_path = std::string(getenv("TTS_SYS_ROOT")) + "/tts_out.wav";

	while(1){
		int c;
		if((c = getopt_long(argc, argv, "hvu:t:o:d:r:p:", long_option, NULL)) < 0){
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
		case 'd':
			if(*optarg == '0') DEBUG_ENABLE = false;
			else DEBUG_ENABLE = true;
			break;
		case 'r':
			if(optarg != NULL){
				tts.enableUnresolvedList(true);
				unresolved_output_path = std::string(optarg);
			}
			break;
		case 'p':
			if(*optarg == '0') tts.play_enable = false;
			else tts.play_enable = true;
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
	if(url.length() != 0){
		printf("Synthesize speech from URL: %s\n", url.c_str());
		tts.sayUrl(url);
	}
	else if(text_path.length() != 0){
		printf("Synthesize speech from file: %s\n", text_path.c_str());
		tts.sayFile(text_path);
	}
	if(unresolved_output_path.length() != 0){
		printf("Output unresolved words list to file: %s\n", unresolved_output_path.c_str());
		tts.outputUnresolvedList(unresolved_output_path);
	}
	return 0;
}


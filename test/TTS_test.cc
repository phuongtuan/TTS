/*
 * TTS_test.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: messier
 */
#define TTS_VERSION_MAJOR		1
#define TTS_VERSION_MINOR		0
#define TTS_VERSION_SUBMINOR	4

#include "TTS.h"
#include "debug.h"
#include "TTS.h"
#include "Sound.h"
#include "NewsReader.h"
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

using namespace iHearTech;
using namespace std;

void initializeComponent(){
	/*
	 * This only work when the application is launched from
	 * its current directory.
	 * TODO: Search application PID and find its location in
	 * /proc directory may be more accurate
	 */
	char *current_path = (char*)calloc(512, sizeof(char));
	getcwd(current_path, 512);
	strcat(current_path, "/../");
	setenv("TTS_SYS_ROOT", current_path, 0);
	free(current_path);
	/*
	 * Initialize debug interface
	 */
	DEBUG_INIT();
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
			"-i, --direct            synthesize from directly text input\n"
			"-l, --volume            set output volume\n"
			"-n, --news-reader       run news reader program\n"
			"-c, --voice-command     enable voice command\n"
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
			{"version", 0, NULL, 'v'},
			{"url", 1, NULL, 'u'},
			{"text", 1, NULL, 't'},
			{"output", 1, NULL, 'o'},
			{"debug", 1, NULL, 'd'},
			{"unresolved", 1, NULL, 'r'},
			{"play-enable", 1, NULL, 'p'},
			{"direct", 1, NULL, 'i'},
			{"volume", 1, NULL, 'l'},
			{"news-reader", 0, NULL, 'n'},
			{"voice-command", 0, NULL, 'c'},
			{NULL, 0, NULL, 0},
	};
	int more_help = 0;
	int more_version = 0;
	if(argv[1] == NULL) more_help++;
	std::string unresolved_output_path;
	std::string url;
	std::string text_path;
	std::string output_path = std::string(getenv("TTS_SYS_ROOT")) + "/tts_out.wav";
	std::string direct_input_text;
	bool run_news_reader = false;
	int volume;
	while(1){
		int c;
		if((c = getopt_long(argc, argv, "hvu:t:o:d:r:p:i:l:cn", long_option, NULL)) < 0){
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
		case 'i':
			if(optarg != NULL) direct_input_text = std::string(optarg);
			break;
		case 'l':
			volume = atoi(optarg);
			if((volume >= 0)&&(volume <= 100)) Sound::setMasterVolume(volume);
			else printf("ERROR: Cannot set volume to %d\n\n",volume);
			break;
		case 'n':
			run_news_reader = true;
			break;
		case 'c':
			NewsReader::enable_voice_cmd = true;
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
	if(run_news_reader){
		if(NewsReader::enable_voice_cmd) printf("Run news reader with voice command input\n");
		else printf("Run news reader with terminal input\n");
		NewsReader::indexing();
		NewsReader::run(&tts);
		return 0;
	}
	/*
	 * Start synthesizing base on user option
	 */
	if(!url.empty()){
		printf("Synthesize speech from URL: %s\n", url.c_str());
		tts.sayUrl(url);
	}
	else if(!text_path.empty()){
		printf("Synthesize speech from file: %s\n", text_path.c_str());
		tts.sayFile(text_path);
	}else if(!direct_input_text.empty()){
		printf("Synthesize from direct input text\n");
		tts.sayText(direct_input_text.c_str());
	}
	/*
	 * Do other stuff below
	 */
	if(!unresolved_output_path.empty()){
		printf("Output unresolved words list to file: %s\n", unresolved_output_path.c_str());
		tts.outputUnresolvedList(unresolved_output_path);
	}
	return 0;
}


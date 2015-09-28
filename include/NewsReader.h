/*
 * NewsReader.h
 *
 *  Created on: Sep 9, 2015
 *      Author: messier
 */

#ifndef NEWSREADER_H_
#define NEWSREADER_H_

#define __VOICE_COMMAND_INPUT__
#define NUMBER_OF_CMD	10
#include <string>
#include <vector>
#include "TTS.h"
#include "HtmlParser.h"
using namespace std;
namespace iHearTech {

struct index_t {
	string name;
	string path;
	string keyword;
	vector<html_t>list;
};

struct cmd_t {
	string keyword;
	unsigned int index;
};

class NewsReader {
public:
	NewsReader();
	virtual ~NewsReader();
	static void index_local();
	static void index_online();
	static vector<index_t> categories;
	static vector<cmd_t> cmdList;

	static void run_local(TTS *tts);
	static void run_online(TTS *tts);
	static unsigned int getVoiceCmd(int option);
	static bool enable_voice_cmd;
private:
	static void splitString(std::vector<std::string> *tokens, const std::string *s, char delim);

};

} /* namespace iHearTech */

#endif /* NEWSREADER_H_ */

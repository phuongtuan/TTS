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
	static void indexing();
	static void online_index();
	static vector<index_t> categories;
	static vector<cmd_t> cmdList;
	static void run(TTS *tts);
	static unsigned int getVoiceCmd(void);
	static bool enable_voice_cmd;

};

} /* namespace iHearTech */

#endif /* NEWSREADER_H_ */

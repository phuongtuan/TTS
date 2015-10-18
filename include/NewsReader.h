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
#define MAX_NUMBER_OF_NEWS	5

#include <string>
#include <vector>
#include "TTS.h"
#include "HtmlParser.h"
using namespace std;
namespace iHearTech {

// Struture contains news list and related information for a news category
struct index_t {
	string name;						// Name of category (which is spoken during category selection)
	string path;						// Local path on the machine
	string keyword;						// Keyword to search in the index.html page
	vector<html_t> list;				// Vector contains news list (html format)
};

struct news_t {
	string name;
	string path;
};

struct category_t {
	string name;
	string path;
	vector<category_t> subcategory;
	vector<news_t> news_list;
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
	static void index_local(char *root_file_path);
	static void index_online();
	static vector<index_t> categories;
	static category_t root_index;
	static vector<cmd_t> cmdList;
	static void run_local(TTS *tts);
	static void run_online(TTS *tts);
	static unsigned int getVoiceCmd(int option);
	static bool enable_voice_cmd;
private:
	static void splitString(std::vector<std::string> *tokens, const std::string *s, char delim);
	static int fillCategory(category_t *cat);
};

} /* namespace iHearTech */

#endif /* NEWSREADER_H_ */

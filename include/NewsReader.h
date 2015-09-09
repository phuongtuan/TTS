/*
 * NewsReader.h
 *
 *  Created on: Sep 9, 2015
 *      Author: messier
 */

#ifndef NEWSREADER_H_
#define NEWSREADER_H_
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

class NewsReader {
public:
	NewsReader();
	virtual ~NewsReader();
	static void indexing();
	static vector<index_t> categories;
	static void run(TTS *tts);
};

} /* namespace iHearTech */

#endif /* NEWSREADER_H_ */

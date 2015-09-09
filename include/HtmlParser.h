/*
 * HtmlParser.h
 *
 *  Created on: Sep 8, 2015
 *      Author: messier
 */

#ifndef HTMLPARSER_H_
#define HTMLPARSER_H_
#include <string>
#include <vector>
#include <dirent.h>
using namespace std;
namespace iHearTech {

struct html_t {
	string title;
	string body;
	string header;
};



class HtmlParser {
public:
	HtmlParser();
	virtual ~HtmlParser();
	static html_t *process(html_t *html, const char *file_path);
};

} /* namespace iHearTech */

#endif /* HTMLPARSER_H_ */

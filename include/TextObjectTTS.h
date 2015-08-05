/*
 * TextObjectTTS.h
 *
 *  Created on: Jul 5, 2015
 *      Author: messier
 */

#ifndef TEXTOBJECTTTS_H_
#define TEXTOBJECTTTS_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <regex.h>
#include <fstream>
#include "debug.h"
#include <glib.h>
#include <vector>
#include "config.h"

#define REGMATCH_MAX_SIZE	10

namespace iHearTech {

typedef enum text_style{
	NORMAL,
	WEBPAGE
}text_style_t;

typedef struct _pattern_t {
	char *pattern;
	std::string *(*handler)(std::string *src, regmatch_t *pmatch);
} pattern_t;

class TextObjectTTS {
public:
	TextObjectTTS();
	virtual ~TextObjectTTS();
	void setTextType();
	void normalize();
	void setInputStr(std::string str);
	bool getTextFromUrl(std::string url);
	std::string getOutputStr();

#ifndef __UNITTEST__		// Expose all methods if __UNITTEST__ is defined
private:
#endif
	std::string inputStr;
	std::string outputStr;
	// Regex handle function
	static std::string* normalize_date_1(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_date_2(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_date_3(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_date_4(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_date_5(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_time(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_snum(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_remove(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_size(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_numcomma(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_numdot(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_punctuation(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_cidis(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_addsp(std::string *src, regmatch_t *pmatch);
	static std::string* normalize_rmduplicate(std::string *src, regmatch_t *pmatch);

	// Util method
	static std::string* normalize_uppercase(std::string *dst, std::string *src);
	static std::string* normalize_3digits(std::string *dst, short num, short hnum);
	static std::string* normalize_number(std::string *dst, unsigned int num);
	static regmatch_t* search_pattern(char * pattern, regex_t *regex, std::string* src, regmatch_t* pmatch);

	// Regex pattern declaration
	static std::vector<pattern_t> regexPattern;
	// Some constant string
	const static char *num_text[];
	const static char *kh[];
};
} /* namespace iHearTech */

#endif /* TEXTOBJECTTTS_H_ */

/*
 * HtmlParser.cc
 *
 *  Created on: Sep 8, 2015
 *      Author: messier
 */

#include "HtmlParser.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
namespace iHearTech {

HtmlParser::HtmlParser() {
	// TODO Auto-generated constructor stub

}

HtmlParser::~HtmlParser() {
	// TODO Auto-generated destructor stub
}

html_t *HtmlParser::process(html_t* html, const char *file_path){
	FILE *pFile = fopen(file_path, "r");
	if(pFile == NULL) return NULL;
	regex_t reg_title;
	//regex_t reg_paragraph;
	regex_t reg_p_start;
	regex_t reg_p_end;

	regmatch_t pmatch[3];
	int res;

	res = regcomp(&reg_title, "<title>(.+)</title>", REG_EXTENDED);
	//res = regcomp(&reg_paragraph, "<p>([[:alpha:]]| )+</p>", REG_EXTENDED);
	res = regcomp(&reg_p_start, "<p>", REG_EXTENDED);
	res = regcomp(&reg_p_end, "</p>", REG_EXTENDED);

	/*
	 * Read all file content
	 */
	int size;
	fseek(pFile, 0, SEEK_END);
	size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char *buffer = (char*)calloc(size, sizeof(char));
	fread(buffer, sizeof(char), size, pFile);

	/*
	 * Parsing into html structure
	 */
	html->title.clear();
	html->header.clear();
	html->body.clear();

	res = regexec(&reg_title, buffer, 3, pmatch, REG_NOTBOL);
	if(res == 0){
		html->title.append(buffer + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
	}
	res = 0;
	char *cptr = buffer;
	int start, end;
	while((res == 0) && (cptr <= (buffer + size))){
		res = regexec(&reg_p_start, cptr, 3, pmatch, REG_NOTBOL);
		start = pmatch[0].rm_eo;
		if(res == 0){
			res = regexec(&reg_p_end, cptr, 3, pmatch, REG_NOTBOL);
			end = pmatch[0].rm_so;
			html->body.append(cptr + start, end - start);
			html->body += " ";
			cptr += pmatch[0].rm_eo;
		}
	}
	return html;
}


} /* namespace iHearTech */

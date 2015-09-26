/*
 * NewsReader.cc
 *
 *  Created on: Sep 9, 2015
 *      Author: messier
 */

#include "NewsReader.h"
#include <string.h>
#include <iostream>
#include <istream>
#include "dirent.h"
#include "stdlib.h"

namespace iHearTech {

/*
 * Add new categories using this structure
 * {<name of categories>, <path to category folder>, }
 */
vector<index_t> NewsReader::categories = {
		{"Chính trị xã hội", "/var/www/html/chinh-tri-xa-hoi/", },
		{"Pháp luật", "/var/www/html/phap-luat/", }
};

vector<cmd_t> NewsReader::cmdList = {
		{"mojt", 1},
		{"hai", 2},
		{"ba", 3},
		{"bon", 4},
		{"nam", 5},
		{"chisnh trij xax hooji", 6},
		{"phasp luaajt", 7},
		{"vawn hoas dari tris", 8},
		{"kinh tees", 9},
		{"thees dowsi", 10}
};

NewsReader::NewsReader() {
	// TODO Auto-generated constructor stub

}

NewsReader::~NewsReader() {
	// TODO Auto-generated destructor stub
}

/*
 * Indexing all html file
 */
void NewsReader::indexing(void){
	DIR *pDir;
	struct dirent *entry;
	vector<index_t>::iterator it_category;
	string file_path;
	html_t html;
	for(it_category = categories.begin(); it_category != categories.end(); ++it_category){
		it_category->list.clear();
		pDir = opendir(it_category->path.c_str());
		if(pDir != NULL){
			while((entry = readdir(pDir)) != NULL){
				if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
					HtmlParser::process(&html, (it_category->path + string(entry->d_name)).c_str());
					it_category->list.push_back(html);
				}
			}
		}
	}
}

unsigned int NewsReader::getVoiceCmd(void){
	system("cd /home/ubuntu/hoang/vnsr_direction-build-desktop;./vnsr_doc_bao.sh");
	std::ifstream ifs("/media/SD/iheartech-tts/vcmd_result");
	std::string cmd_str((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
	vector<cmd_t>::iterator it;
	std::size_t found;
	for(it = cmdList.begin(); it != cmdList.end(); it++){
		found = cmd_str.find(it->keyword, 0);
		if (found!=std::string::npos){
			return it->index;
		}
	}
	return 0;
}

/*
 * Run news reader program
 */
void NewsReader::run(TTS *tts){
	//Read categories list;
	int i = 1;
	unsigned int choice_category;
	unsigned int choice_news;
	vector<index_t>::iterator it;
	tts->sayText("Chào bạn, đây là chương trình đọc báo điện tử. Các chuyên mục hiện có gồm");
	cout << "Các chuyên mục hiện có: " << endl;
	for(it = categories.begin(); it != categories.end(); it++){
		cout << i << ". " << it->name << endl;
		tts->sayText((it->name).c_str());
		i++;
	}
	tts->sayText("Bạn muốn chọn chuyên mục nào");
	cout << "Chọn chuyên mục số: " << endl;
	/*
	 * Select voice command input or terminal input
	 */
#ifndef __VOICE_COMMAND_INPUT__
	do{
		scanf("%d",&choice_category);
	}while(choice_category > categories.size());
#else
	choice_category = NewsReader::getVoiceCmd() - 6;
#endif
	cout << "Bạn đã chọn chuyên mục số: " << choice_category << endl;
	choice_category --;
	tts->sayText(("Bạn đã chọn chuyên mục " + categories[choice_category].name).c_str());
	tts->sayText(("Chuyên mục " + categories[choice_category].name + " gồm có các tin sau").c_str());
	cout << "Danh mục tin tức:" << endl;
	vector<html_t>::iterator it_html;
	i = 1;
	for(it_html = categories[choice_category].list.begin(); it_html != categories[choice_category].list.end(); it_html++){
		cout << i << ". " << it_html->title << endl;
		tts->sayText(it_html->title.c_str());
		i++;
	}
	cout << "Bạn chọn tin số: " << endl;
#ifndef __VOICE_COMMAND_INPUT__
	scanf("%d", &choice_news);
	cout << "Bạn đã chọn tin số: " << choice_news << endl;
	choice_news --;
#else
	choice_news = NewsReader::getVoiceCmd();
#endif
	tts->sayText(("Bạn đã chọn tin " + categories[choice_category].list[choice_news].title).c_str());
	tts->sayText(categories[choice_category].list[choice_news].body.c_str());
}

} /* namespace iHearTech */

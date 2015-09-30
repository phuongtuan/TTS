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
#include "get_html_link.h"
#include <fstream>
#include <sstream>
#include "Sound.h"

namespace iHearTech {

bool NewsReader::enable_voice_cmd = false;
/*
 * Add new categories using this structure
 * {<name of categories>, <path to category folder>, }
 */
vector<index_t> NewsReader::categories = {
		{"Chính trị xã hội", "/var/www/html/chinh-tri-xa-hoi/", "http://tuoitre.vn/tin/chinh-tri-xa-hoi",},
		{"Pháp luật", "/var/www/html/phap-luat/", "http://tuoitre.vn/tin/phap-luat", },
		{"Văn hóa giải trí", "/var/www/html/van-hoa-giai-tri/", "http://tuoitre.vn/tin/van-hoa-giai-tri", },
		{"Kinh tế", "/var/www/html/kinh-te/", "http://tuoitre.vn/tin/kinh-te", },
		{"Thế giới", "/var/www/html/the-gioi/", "http://tuoitre.vn/tin/the-gioi", },
};
/*
 * Commands list and associated indexes
 */
vector<cmd_t> NewsReader::cmdList = {
		{"moojt", 1},
		{"hai", 2},
		{"ba", 3},
		{"boosn", 4},
		{"nawm", 5},
		{"chisnh", 6},
		{"phasp", 7},
		{"vawn", 8},
		{"kinh", 9},
		{"thees", 10}
};

NewsReader::NewsReader() {
	// TODO Auto-generated constructor stub

}

NewsReader::~NewsReader() {
	// TODO Auto-generated destructor stub
}

void NewsReader::splitString(std::vector<std::string> *tokens, const std::string *s, char delim){
	stringstream ss(*s);
	string item;
	tokens->clear();		// Clear before using
	while (getline(ss, item, delim)){
		tokens->push_back(item);
	}
}

/*
 * Indexing all html file
 */
void NewsReader::index_local(void){
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
					if(it_category->list.size() < MAX_NUMBER_OF_NEWS)it_category->list.push_back(html);
				}
			}
		}
	}
}

void NewsReader::index_online(void){
	html_t html;
	update_link();
	std::ifstream ifsIndex("./index.txt");
	if(ifsIndex.is_open() == false){
		printf("Cannot find index.txt\n");
		return;
	}
	std::string strline;
	std::vector<string> tokens;
	do{
		strline.clear();
		getline(ifsIndex, strline);
		vector<index_t>::iterator it;
		for(it = NewsReader::categories.begin(); it != NewsReader::categories.end(); ++it){
			if(strline.find(it->keyword) != std::string::npos){
				splitString(&tokens, &strline, '\t');
				if(tokens.size() < 2) break;
				else{
					html.url = tokens[0];
					html.title = tokens[1];
					if(it->list.size() < MAX_NUMBER_OF_NEWS) it->list.push_back(html);
				}
				break;
			}
		}
	}while(!ifsIndex.eof());
}

unsigned int NewsReader::getVoiceCmd(int option){
	Sound::play("/media/SD/iheartech-tts/database/beep_start.wav");
	if(option == 1) {
		system("cd /home/ubuntu/hoang/vnsr_direction-build-desktop;./vnsr_doc_bao.sh;cd -");
	}else if (option == 2){
		system("cd /home/ubuntu/hoang/vnsr_direction-build-desktop;./vnsr_doc_bao_lan2.sh;cd -");
	}
	std::ifstream ifs("/media/SD/iheartech-tts/vcmd_result");
	std::string cmd_str((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
	vector<cmd_t>::iterator it;
	std::size_t found;
	printf("Command: %s\n", cmd_str.c_str());
	for(it = cmdList.begin(); it != cmdList.end(); it++){
		found = cmd_str.find(it->keyword, 0);
		if (found != std::string::npos){
			printf("Command index: %d\n", it->index);
			return it->index;
		}
	}
	return 0;
}

/*
 * Run news reader program
 */
void NewsReader::run_local(TTS *tts){
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
	if(enable_voice_cmd){
		do{
			choice_category = NewsReader::getVoiceCmd(1) - 5;
		}while(choice_category > categories.size());
	} else{
		do{
			scanf("%d",&choice_category);
		}while(choice_category > categories.size());
	}
	cout << "Bạn đã chọn chuyên mục số: " << choice_category << endl;
	choice_category --;
	tts->sayText(("Bạn đã chọn chuyên mục " + categories[choice_category].name).c_str());
	tts->sayText(("Chuyên mục " + categories[choice_category].name + " gồm có các tin sau").c_str());
	cout << "Danh mục tin tức:" << endl;
	vector<html_t>::iterator it_html;
	i = 1;
	char numstr[32];
	for(it_html = categories[choice_category].list.begin(); it_html != categories[choice_category].list.end(); it_html++){
		cout << i << ". " << it_html->title << endl;
		snprintf(numstr, 32, "Tin số %d. ", i);
		tts->sayText((std::string(numstr) + it_html->title).c_str());
		i++;
	}
	tts->sayText("Bạn muốn chọn tin số mấy");
	cout << "Bạn chọn tin số: " << endl;
	if(enable_voice_cmd){
		do{
			choice_news = NewsReader::getVoiceCmd(2);
		}while(choice_news > categories[choice_category].list.size());
	} else{
		do{
			scanf("%d", &choice_news);
			cout << "Bạn đã chọn tin số: " << choice_news << endl;
		}while(choice_news > categories[choice_category].list.size());
	}
	choice_news --;
	tts->sayText(("Bạn đã chọn tin. " + categories[choice_category].list[choice_news].title).c_str());
	tts->sayText(categories[choice_category].list[choice_news].body.c_str());
}

void NewsReader::run_online(TTS *tts){
	DEBUG_INFO("Running news reader in online mode");
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
	if(enable_voice_cmd){
		do{
			choice_category = NewsReader::getVoiceCmd(1) - 5;
		}while(choice_category > categories.size());
	} else{
		do{
			scanf("%d",&choice_category);
		}while(choice_category > categories.size());
	}
	cout << "Bạn đã chọn chuyên mục số: " << choice_category << endl;
	choice_category --;
	tts->sayText(("Bạn đã chọn chuyên mục " + categories[choice_category].name).c_str());
	tts->sayText(("Chuyên mục " + categories[choice_category].name + " gồm có các tin sau").c_str());
	cout << "Danh mục tin tức:" << endl;
	vector<html_t>::iterator it_html;
	i = 1;
	char numstr[32];
	for(it_html = categories[choice_category].list.begin(); it_html != categories[choice_category].list.end(); it_html++){
		cout << i << ". " << it_html->title << endl;
		snprintf(numstr, 32, "Tin số %d. ", i);
		tts->sayText((std::string(numstr) + it_html->title).c_str());
		i++;
	}
	tts->sayText("Bạn muốn chọn tin số mấy");
	cout << "Bạn chọn tin số: " << endl;
	if(enable_voice_cmd){
		do{
			choice_news = NewsReader::getVoiceCmd(2);
		}while(choice_news > categories[choice_category].list.size());
	} else{
		do{
			scanf("%d", &choice_news);
			cout << "Bạn đã chọn tin số: " << choice_news << endl;
		}while(choice_news > categories[choice_category].list.size());
	}
	choice_news --;
	tts->sayText(("Bạn đã chọn tin " + categories[choice_category].list[choice_news].title).c_str());
	tts->sayUrl(categories[choice_category].list[choice_news].url);
}

} /* namespace iHearTech */

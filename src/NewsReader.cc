/*
 * NewsReader.cc
 *
 *  Created on: Sep 9, 2015
 *      Author: messier
 */

#include "NewsReader.h"
#include <string.h>
#include <iostream>
#include "dirent.h"

namespace iHearTech {

/*
 * Add new categories using this structure
 */
vector<index_t> NewsReader::categories = {
		{"Chính trị xã hội", "/var/www/html/chinh-tri-xa-hoi/", },
		{"Pháp luật", "/var/www/html/phap-luat/", }
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

/*
 * Run news reader program
 */
void NewsReader::run(TTS *tts){
	//Read categories list;
	int i = 1;
	int choice_category;
	int choice_news;
	vector<index_t>::iterator it;
	cout << "Các chuyên mục hiện có: " << endl;
	for(it = categories.begin(); it != categories.end(); it++){
		cout << i << ". " << it->name << endl;
		i++;
	}
	cout << "Chọn chuyên mục số: " << endl;
	scanf("%d",&choice_category);
	cout << "Bạn đã chọn chuyên mục số: " << choice_category << endl;
	choice_category --;
	cout << "Danh mục tin tức:" << endl;
	vector<html_t>::iterator it_html;
	i = 1;
	for(it_html = categories[choice_category].list.begin(); it_html != categories[choice_category].list.end(); it_html++){
		cout << i << ". " << it_html->title << endl;
		i++;
	}
	cout << "Bạn chọn tin số: " << endl;
	scanf("%d", &choice_news);
	cout << "Bạn đã chọn tin số: " << choice_news << endl;
	choice_news --;
	tts->sayText(categories[choice_category].list[choice_news].body.c_str());
}

} /* namespace iHearTech */

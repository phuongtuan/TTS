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
#include "rapidxml.hpp"			// xml parser library

using namespace std;
using namespace rapidxml;

namespace iHearTech {

bool NewsReader::enable_voice_cmd = false;

// Add new categories using this structure
// {<name of categories>, <path to category folder>, }
vector<index_t> NewsReader::categories = {
		{"Chính trị xã hội", "/var/www/html/chinh-tri-xa-hoi/", "http://tuoitre.vn/tin/chinh-tri-xa-hoi", "chisnh +trij +xax +hooji"},
		{"Pháp luật", "/var/www/html/phap-luat/", "http://tuoitre.vn/tin/phap-luat", "phasp +luaajt"},
		{"Văn hóa giải trí", "/var/www/html/van-hoa-giai-tri/", "http://tuoitre.vn/tin/van-hoa-giai-tri", "vawn +hoas +dari +tris"},
		{"Kinh tế", "/var/www/html/kinh-te/", "http://tuoitre.vn/tin/kinh-te", "kinh +tees"},
		{"Thế giới", "/var/www/html/the-gioi/", "http://tuoitre.vn/tin/the-gioi", "thees +dowsi"},
		//{"Giáo dục", "/var/www/html/giao-duc/", "http://tuoitre.vn/tin/giao-duc", "giaso +dujc"},
};

category_t NewsReader::root_index;

vector<cmd_t> NewsReader::cmdList_online = {
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

// Commands list and associated indexes
vector<cmd_t> NewsReader::cmdNumber = {
		{"moojt", 1},
		{"hai", 2},
		{"ba", 3},
		{"boosn", 4},
		{"nawm", 5},
		{"sasu", 6},
		{"bary", 7},
		{"tasm", 8},
		{"chisn", 9},
		{"tieesp", -1},
		{"duwfng", -2}
};

NewsReader::NewsReader() {
	// TODO Auto-generated constructor stub
}

NewsReader::~NewsReader() {
	// TODO Auto-generated destructor stub
}

// Split string into tokens. Each tokens separate by character delim
void NewsReader::splitString(std::vector<std::string> *tokens, const std::string *s, char delim){
	stringstream ss(*s);
	string item;
	tokens->clear();		// Clear before using
	while (getline(ss, item, delim)){
		tokens->push_back(item);
	}
}

// Recursion function to fill category struct
int NewsReader::fillCategory(category_t *cat){
	// Open index.xml file
	ifstream ifs(cat->path + "/index.xml");
	if(ifs.is_open() == false){
		DEBUG_ERROR("Cannot open index.xml in %s", cat->path.c_str());
		return -1;
	}
	vector<char>buffer((istreambuf_iterator<char>(ifs)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parsing into xml document structure
	xml_document<> xml;
	xml_node<> *root_node;
	xml.parse<0>(&buffer[0]);
	root_node = xml.first_node("category");
	// Iterate over sub-category
	for(xml_node<> *sub_category_node = root_node->first_node("category");
			sub_category_node; sub_category_node = sub_category_node->next_sibling()){
		category_t sub_category;
		sub_category.name = string(sub_category_node->first_attribute("name")->value());
		sub_category.path = string(sub_category_node->first_attribute("path")->value());
		sub_category.vnsr_keyword = string(sub_category_node->first_attribute("vnsr_keyword")->value());
		// Recursion call
		fillCategory(&sub_category);
		cat->subcategory.push_back(sub_category);
	}
	// Iterate over news
	for(xml_node<> *news_node = root_node->first_node("news");
			news_node; news_node = news_node->next_sibling()){
		news_t news;
		news.name = string(news_node->first_attribute("name")->value());
		news.path = string(news_node->first_attribute("path")->value());
		cat->news_list.push_back(news);
	}
	return 0;
}

// Indexing all categories and news in local storage
void NewsReader::index_local(char *root_file_path){
	// Initialize root index
	NewsReader::root_index.name = "Mục lục chính";
	NewsReader::root_index.path = string(root_file_path);
	fillCategory(&root_index);
}

// Indexing news from tuoitre.vn
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

unsigned int NewsReader::getVoiceCmd_online(int option){
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
	for(it = cmdList_online.begin(); it != cmdList_online.end(); it++){
		found = cmd_str.find(it->keyword, 0);
		if (found != std::string::npos){
			printf("Command index: %d\n", it->index);
			return it->index;
		}
	}
	return 0;
}

// Run vnsr script and read result
unsigned int NewsReader::getVoiceCmd(int option, category_t *cat){
	if(option == 1) {
		std::ifstream ifs("../vnsr_result.txt");
		std::string cmd_str((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));
		if(cmd_str.find("bor") != string::npos) return -1;
		if(cat != NULL){
			vector<category_t>::iterator it;
			regex_t regex;
			regmatch_t pmatch[2];
			int res;
			for(it = cat->subcategory.begin(); it != cat->subcategory.end(); it++){
				if(regcomp(&regex, it->vnsr_keyword.c_str(), REG_EXTENDED) == 0){
					res = regexec(&regex, cmd_str.c_str(), 2, pmatch, REG_NOTBOL);
					if(res == 0){
						regfree(&regex);
						return (it - cat->subcategory.begin());
					}
				}
			}
		}
	}else if (option == 2){
		std::ifstream ifs("../vnsr_result.txt");
		std::string cmd_str((std::istreambuf_iterator<char>(ifs)),
				(std::istreambuf_iterator<char>()));
		vector<cmd_t>::iterator it;
		std::size_t found;
		printf("Command: %s\n", cmd_str.c_str());
		for(it = cmdNumber.begin(); it != cmdNumber.end(); it++){
			found = cmd_str.find(it->keyword, 0);
			if (found != std::string::npos){
				printf("Command index: %d\n", it->index);
				return it->index;
			}
		}
	}
	return -10;
}

// Run news reader program in offline mode
void NewsReader::run_local(TTS *tts){
	// Read categories list;
	tts->sayText("Chào bạn, đây là chương trình đọc báo điện tử.");
	bool cont = false;
	do {
		int choice_category;
		int choice_news;
		vector<category_t>::iterator it_head;
		category_t *head = &NewsReader::root_index;
		int head_count = 0;
		while(head->subcategory.size() > 0){
			tts->sayText((string(head->name) + " gồm có các chuyên mục sau").c_str());
			cout << head->name <<" gồm các chuyên mục:" << endl;
			int i = 1;
			for(it_head = head->subcategory.begin();
					it_head != head->subcategory.end(); it_head++){
				cout << i++ << ". " << it_head->name << endl;
				tts->sayText(it_head->name.c_str());
			}
			tts->sayText("Bạn muốn chọn chuyên mục nào");
			cout << "Chọn chuyên mục số: " << endl;
			// Select voice command input or terminal input
			if(enable_voice_cmd){
				do{
					if(head_count == 0) system("cd /home/ubuntu/hoang/vnsr_direction-build-desktop;./vnsr_doc_bao.sh;cd -");
					else if(head_count == 1) system("cd /home/ubuntu/hoang/vnsr_direction-build-desktop;./vnsr_doc_bao_lan3.sh;cd -");
					choice_category = NewsReader::getVoiceCmd(1, head);
					// Command == 'bor qua' -> return
					if(choice_category == -1){
						tts->sayText("Chương trình kết thúc");
						return;
					}
					if(choice_category == -10) tts->sayText("Xin lỗi, bạn vui lòng đọc lại");
				}while((choice_category > (signed)head->subcategory.size())||
						(choice_category < 0));
			} else{
				do{
					scanf("%d",&choice_category);
				}while(choice_category > (signed)head->subcategory.size());
				choice_category --;
			}
			cout << "Bạn đã chọn chuyên mục " << head->subcategory[choice_category].name << endl;
			tts->sayText(("Bạn đã chọn chuyên mục " + head->subcategory[choice_category].name).c_str());
			head = &(head->subcategory[choice_category]);
			head_count++;
		}
		tts->sayText(("Chuyên mục " + head->name + " gồm có các tin sau").c_str());
		cout << "Danh mục tin tức:" << endl;
		vector<news_t>::iterator it_html;
		int i = 1;
		char numstr[32];
		for(it_html = head->news_list.begin(); it_html != head->news_list.end(); it_html++){
			cout << i << ". " << it_html->name << endl;
			snprintf(numstr, 32, "Tin số %d. ", i);
			tts->sayText((std::string(numstr) + it_html->name).c_str());
			i++;
		}
		tts->sayText("Bạn muốn chọn tin số mấy");
		cout << "Bạn chọn tin số: " << endl;
		if(enable_voice_cmd){
			do{
				system("cd /home/ubuntu/hoang/vnsr_direction-build-desktop;./vnsr_doc_bao_lan2.sh;cd -");
				choice_news = NewsReader::getVoiceCmd(2);
				// "tooi muoosn duwfng
				if(choice_news == -2){
					tts->sayText("Bạn đã chọn dừng. Chương trình kết thúc");
					return;
				}
				if(choice_news == -10) tts->sayText("Xin lỗi, bạn vui lòng đọc lại");
			}while((choice_news > (signed)head->news_list.size())||
					(choice_news < 0));
		} else{
			do{
				scanf("%d", &choice_news);
				cout << "Bạn đã chọn tin số: " << choice_news << endl;
			}while(choice_news > (signed)head->news_list.size());
		}
		choice_news --;
		tts->sayText(("Bạn đã chọn tin. " + head->news_list[choice_news].name).c_str());
		html_t html_file;
		HtmlParser::process(&html_file, head->news_list[choice_news].path.c_str());
		tts->sayText(html_file.body.c_str());
		cout << "Bạn muốn tiếp tục hay dừng? 1: tiếp tục, 0: dừng" << endl;
		tts->sayText("Bạn muốn tiếp tục hay dừng?");
		int choice;
		if(enable_voice_cmd){
			do{
				system("cd /home/ubuntu/hoang/vnsr_direction-build-desktop;./vnsr_doc_bao_lan4.sh;cd -");
				choice = NewsReader::getVoiceCmd(2);
				if(choice == -1){
					tts->sayText("Bạn đã chọn tiếp tục");
					cont = true;
				}
				else if(choice == -2){
					tts->sayText("Bạn đã chọn dừng");
					cont = false;
				}
			}while(choice >= 0);
		} else{
			scanf("%d",&choice);
			if(choice == 1) {
				cout << "Bạn đã chọn tiếp tục" << endl;
				cont = true;
			}
			else{
				tts->sayText("Bạn đã chọn dừng chương trình");
				cont = false;
			}
		}
	}while(cont);
}

// Run news reader in online mode
void NewsReader::run_online(TTS *tts){
	DEBUG_INFO("Running news reader in online mode");
	// Read categories list;
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

	// Select voice command input or terminal input
	if(enable_voice_cmd){
		do{
			choice_category = NewsReader::getVoiceCmd_online(1) - 5;
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
			choice_news = NewsReader::getVoiceCmd_online(2);
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

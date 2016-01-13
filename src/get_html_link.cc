#include "rapidxml.hpp"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace rapidxml;
using namespace std;

void get_link_file(char *filepath, FILE *index){
	xml_document<> doc;
	xml_node<> * root_node;
	// Read the xml file into a vector
	ifstream theFile (filepath);
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]);
	// Find our root node
	root_node = doc.first_node("rss");
	xml_node<> * channel_node = root_node->first_node("channel");

	for (xml_node<> * item_node = channel_node->first_node("item"); item_node; item_node = item_node->next_sibling())
	{
		xml_node<> * link_node = item_node->first_node("link");
		xml_node<> * title_node = item_node->first_node("title");
		fprintf(index, "%s	%s\n",link_node->value(), title_node->value());
	}

}

int get_link(void){
	FILE *index = fopen("./index.txt", "w");
	// chinh-tri-xa-hoi
	system("wget -O ./news.html http://tuoitre.vn/rss/tt-chinh-tri-xa-hoi.rss 2> /dev/null");
	get_link_file((char*)"./news.html", index);

	// van-hoa-giai-tri
	system("wget -O ./news.html http://tuoitre.vn/rss/tt-van-hoa-giai-tri.rss 2> /dev/null");
	get_link_file((char*)"./news.html", index);

	// phap-luat
	system("wget -O ./news.html http://tuoitre.vn/rss/tt-phap-luat.rss 2> /dev/null");
	get_link_file((char*)"./news.html", index);

	// phap-luat
	system("wget -O ./news.html http://tuoitre.vn/rss/tt-phap-luat.rss 2> /dev/null");
	get_link_file((char*)"./news.html", index);

	// phap-luat
	system("wget -O ./news.html http://tuoitre.vn/rss/tt-kinh-te.rss 2> /dev/null");
	get_link_file((char*)"./news.html", index);

	// the gioi
	system("wget -O ./news.html http://tuoitre.vn/rss/tt-the-gioi.rss 2> /dev/null");
	get_link_file((char*)"./news.html", index);
	fclose(index);
	return 0;
}

void update_link(){
	get_link();
}

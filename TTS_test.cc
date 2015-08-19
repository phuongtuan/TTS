/*
 * TTS_test.cpp
 *
 *  Created on: Jul 1, 2015
 *      Author: messier
 */
#include "TTS.h"
#include <stdio.h>
#include "debug.h"

#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>
#include "TextObjectTTS.h"
#include "UnitSelector.h"

#include "unittest/testUnitSelector.h"

using namespace iHearTech;
using namespace std;

void initializeComponent(){
	DEBUG_INIT();
}

vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

int main(int argc, char* argv[]){
	initializeComponent();

//	std::ifstream ifs("/home/messier/news");
//	std::string str((std::istreambuf_iterator<char>(ifs)),
//				(std::istreambuf_iterator<char>()));
	std::ofstream ofs("/home/messier/news.nml");
	TextObjectTTS a;
	//if(!a.getTextFromUrl("http://tuoitre.vn/tin/kinh-te/20150724/can-song-phang-voi-doanh-nghiep/782136.html")){
	//if(!a.getTextFromUrl("http://tuoitre.vn/tin/phap-luat/20150811/bat-giam-4-can-bo-sai-pham-den-bu-giai-phong-mat-bang/947259.html")){
	if(!a.getTextFromUrl("http://tuoitre.vn/tin/phap-luat/20150819/bi-phat-tien-neu-ket-hon-giua-anh-em-ba-doi/954756.html")){
		return 1;
	}
	a.normalize();
	ofs << a.getOutputStr();
	UnitSelector b;
	b.restoreMaps();
	//b.initMaps();
	//b.storeMaps();
	b.createIdList(a.getOutputStr());
	//b.outputIdListToFile("/home/messier/idlist");
	b.createWavFile("/home/messier/tts_out.wav");
	//b.outputUnresolvedListToFile("/home/messier/unresolve_list.txt");
	return 0;
}



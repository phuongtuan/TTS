/*
 * test_hash_map.cc
 * Test unitMap and unitIdMap
 *  Created on: Jul 30, 2015
 *      Author: messier
 */
#include <fstream>
#include "debug.h"
#include "UnitSelector.h"
#include "config.h"

using namespace std;
using namespace iHearTech;
namespace unittest{

bool testHashMaps(iHearTech::UnitSelector *t){
#ifdef __UNITTEST__
	std::string TTS_SYS_ROOT(getenv("TTS_SYS_ROOT"));
	std::ifstream ifsUnitId(TTS_SYS_ROOT + TTS_UNIT_ID_PATH);
	std::ifstream ifsUnitSel(TTS_SYS_ROOT + TTS_UNIT_SELECTOR_PATH);
	std::ifstream ifsSpecDict(TTS_SYS_ROOT + TTS_SPEC_PATH);
	std::string strline;
	vector<string> tokens;
	std::string phrase;
	int id;
	if(t->unitIdMap.empty() || t->unitMap.empty()){
		DEBUG_ERROR("Unittest failed: unitIdMap or unitMap uninitialized!");
		return false;
	}
	while(!ifsUnitId.eof()){
		getline(ifsUnitId, strline);
		tokens = iHearTech::UnitSelector::splitString(strline, '\t');
		if(tokens.size() < 2) break;
		phrase = tokens[0];
		id = atoi(tokens[1].c_str());
		if(!(t->unitIdMap[phrase] == id)){
			DEBUG_ERROR("Cannot search phrase %s in unit ID map", phrase.c_str());
			return false;
		}
	}
	unit_key_t key;
	wav_segment_t segment;
	while(!ifsUnitSel.eof()){
		getline(ifsUnitSel, strline);
		tokens = iHearTech::UnitSelector::splitString(strline, '\t');
		if(tokens.size() < 6) break;
		key.id = atoi(tokens[0].c_str());
		key.id_left = atoi(tokens[1].c_str());
		key.id_right = atoi(tokens[2].c_str());
		strcpy(segment.filename,tokens[3].c_str());
		segment.begin = atoi(tokens[4].c_str());
		segment.end = atoi(tokens[5].c_str());
		if(!(t->unitMap[key] == segment)){
			DEBUG_ERROR("Cannot search key{%d,%d,%d} in unit ID map",
					key.id, key.id_left, key.id_right);
			return false;
		}
	}
	while(!ifsSpecDict.eof()){
		getline(ifsSpecDict, strline);
		tokens = iHearTech::UnitSelector::splitString(strline, '\t');
		if(tokens.size() < 2) break;
		if(t->specMap[tokens[0]].compare(tokens[1]) != 0){
			DEBUG_ERROR("Cannot search string %s in specMap", tokens[0].c_str());
			return false;
		}
	}
#endif
	return true;
}

bool testHashMapSerialize(void){
#ifdef __UNITTEST__
	iHearTech::UnitSelector t, t1;
	t1.initMaps();
	t1.storeMaps();
	t.restoreMaps();
	if(!(unittest::testHashMaps(&t)))
		return false;
#endif
	return true;
}
}

/*
 * UnitSelector.cpp
 *
 *  Created on: Jul 16, 2015
 *      Author: messier
 */

#include "UnitSelector.h"

using namespace std;
namespace iHearTech {
const std::string UnitSelector::alpha_text[] = {"A", "BÊ", "XÊ", "ĐI", "E", "ÉP", "GI", "HẮC", "Y", "DI", "KA", "LỜ",
		"MỜ", "NỜ", "O", "BI", "QUI", "RỜ", "SỜ", "TÊ", "U", "VÊ", "ĐÚP LỜ VÊ", "ÍT", "Y", "DÉT"};

UnitSelector::UnitSelector(){
	// TODO Auto-generated constructor stub
}

UnitSelector::~UnitSelector(){
	// TODO Auto-generated destructor stub
	//	vector<string>().swap(this->vectorUnitId);
	//	vector<unit_key_t>().swap(this->vectorUnitKey);
}

void UnitSelector::initMaps(){
	DEBUG_INFO("Initializing hash maps...");
	std::string TTS_SYS_ROOT(getenv("TTS_SYS_ROOT"));
	std::ifstream ifsUnitId(TTS_SYS_ROOT + TTS_UNIT_ID_PATH);
	std::ifstream ifsUnitSel(TTS_SYS_ROOT + TTS_UNIT_SELECTOR_PATH);
	std::ifstream ifsSpecial(TTS_SYS_ROOT + TTS_SPEC_PATH);
	if(!(ifsUnitId.is_open()&&ifsUnitSel.is_open()&&ifsSpecial.is_open())){
		DEBUG_ERROR("Cannot open TTS dictionary, return now");
		return;
	}
	DEBUG_INFO("Initializing unit ID map...");
	std::string strline;
	vector<string> tokens;
	unitIdMap.set_empty_key(std::string());
	do{
		getline(ifsUnitId, strline);
		tokens = splitString(strline, '\t');
		if(tokens.size() < 2) break;
		this->unitIdMap[tokens[0]] = atoi(tokens[1].c_str());;
	}while(!ifsUnitId.eof());
	DEBUG_INFO("Initializing unit selector map...");
	unit_key_t key;
	wav_segment_t segment;
	this->unitMap.set_empty_key(this->default_key);
	do{
		getline(ifsUnitSel, strline);
		tokens = splitString(strline, '\t');
		if(tokens.size() < 6) break;
		key.id = atoi(tokens[0].c_str());
		key.id_left = atoi(tokens[1].c_str());
		key.id_right = atoi(tokens[2].c_str());
		strcpy(segment.filename,tokens[3].c_str());
		segment.begin = atoi(tokens[4].c_str());
		segment.end = atoi(tokens[5].c_str());
		this->unitMap[key] = segment;
	}while(!ifsUnitSel.eof());
	DEBUG_INFO("Initializing special map");
	this->specMap.set_empty_key(string());
	do{
		getline(ifsSpecial, strline);
		tokens = splitString(strline, '\t');
		if(tokens.size() < 2) break;
		this->specMap[tokens[0]] = tokens[1];
	}while(!ifsSpecial.eof());
	DEBUG_INFO("Finish initializing hash maps");
}

void UnitSelector::storeMaps(void){
	std::string TTS_SYS_ROOT(getenv("TTS_SYS_ROOT"));
	FILE *fpUnitId = fopen((TTS_SYS_ROOT + TTS_UNIT_ID_DAT).c_str(),"w");
	// Serialize unit id map
	if(!fpUnitId){
		DEBUG_ERROR("Cannot load unit ID map data");
		return;
	}
	this->unitIdMap.serialize(unitIdMapSerializer(),fpUnitId);
	fclose(fpUnitId);
	// Serialize Unit Selector map
	FILE *fpUnitSel = fopen((TTS_SYS_ROOT + TTS_UNIT_SELECTOR_DAT).c_str(),"w");
	if(!fpUnitSel){
		DEBUG_ERROR("Cannot load unit map data");
		return;
	}
	if(this->unitMap.empty()){
		DEBUG_ERROR("Unit map uninitialized, run initMaps first!");
		return;
	}
	this->unitMap.serialize(unitMapSerializer(), fpUnitSel);
	fclose(fpUnitSel);
	// Serialize specMap
	FILE *fpSpecDict  = fopen((TTS_SYS_ROOT + TTS_SPEC_DAT).c_str(), "w");
	if(!fpSpecDict){
		DEBUG_ERROR("Cannot load specMap.dat");
		return;
	}
	if(this->specMap.empty()){
		DEBUG_ERROR("specMap uninitialized, run initMaps first!");
		return;
	}
	this->specMap.serialize(specMapSerializer(), fpSpecDict);
	fclose(fpSpecDict);
}

void UnitSelector::restoreMaps(void){
	DEBUG_INFO("Restoring hash maps");
	std::string TTS_SYS_ROOT(getenv("TTS_SYS_ROOT"));
	FILE *fpUnitId = fopen((TTS_SYS_ROOT + TTS_UNIT_ID_DAT).c_str(),"r");
	// De-serialize unit id map
	if(!fpUnitId){
		DEBUG_ERROR("Cannot load unit ID map data");
		return;
	}
	this->unitIdMap.set_empty_key(std::string());
	this->unitIdMap.unserialize(unitIdMapSerializer(), fpUnitId);
	fclose(fpUnitId);

	// De-serialize unit map
	FILE *fpUnitSel = fopen((TTS_SYS_ROOT + TTS_UNIT_SELECTOR_DAT).c_str(), "r");
	if(!fpUnitSel){
		DEBUG_ERROR("Cannot load unit map data");
		return;
	}
	this->unitMap.set_empty_key(this->default_key);
	this->unitMap.unserialize(unitMapSerializer(), fpUnitSel);
	fclose(fpUnitSel);
	DEBUG_INFO("Finished restoring hash maps");

	// De-serialize specMap
	FILE *fpSpecDict  = fopen((TTS_SYS_ROOT + TTS_SPEC_DAT).c_str(), "r");
	if(!fpSpecDict){
		DEBUG_ERROR("Cannot load specMap.dat");
		return;
	}
	this->specMap.set_empty_key(std::string());
	this->specMap.unserialize(specMapSerializer(), fpSpecDict);
	fclose(fpSpecDict);
}

// Create ID list from an input string
// String is tokenize into sentences (mark by '$' in the end of every
// sentence) and then then tokenized into words. The longest word combination
// that exist in unitIdMap is selected.
void UnitSelector::createIdList(std::string str){
	vector<string>::iterator its;
	vector<string> words;
	string phrase;
	unit_t unit;
	unsigned int id, num_words;
	// Split input string into sentences
	vector<string> sentence;
	//TODO: Indicate end-of-sentence symbol for tokenizing, should be '\n' ??
	this->splitString(&sentence, &str, '\n');
	// Start searching each sentence
	this->idList.clear();
	for(its = sentence.begin(); its != sentence.end(); ++its){
		// Split phrase into words
		this->splitString(&words, &(*its), ' ');
		// Search for longest phrase contents combination from "words"
		phrase.clear();
		num_words = 0;
		for(vector<string>::iterator itw = words.begin(); itw != words.end(); ++itw){
			phrase += *itw;
			if(phrase.empty())	// Blank space at string beginning cause an empty word
				continue;		// just simply ignore and pass it
			num_words++;
			if((id = this->unitIdMap[phrase]) == 0){
				//TODO: Bug detected !!!!!!
				if(num_words == 1){
					// Lets search in abbreviation dictionary
					this->resolveAbbreWord(phrase);
				}else{
					idList.push_back(unit);
					itw--;
				}
				phrase.clear();
				num_words = 0;
			}else{
				unit.key.id = id;
				phrase += " ";
			}
		}
	}
	// All phrase-id have been found, wave segment of each unit is select
	// from the unitMap base on left and right unit
	this->fillNeighborId();
	std::vector<unit_t>::iterator itu;
	wav_segment_t segment;
	unit_key_t key;
	for(itu = this->idList.begin();	itu != this->idList.end(); ++itu){
		DEBUG_INFO("Search unitMap with key = {%d,%d,%d}",itu->key.id,
				itu->key.id_left, itu->key.id_right);
		if(itu->key.id == 0){
			DEBUG_ERROR("key.id = 0, ignore this unit");
			continue;
		}
		key = itu->key;
		segment = this->unitMap[key];
		// Select the most identical unit from unitMap
		// Search process:
		// {id, id_left, id_right} --[not exist?]-->
		// {id, 0, id_right} --[not exist?]-->
		// {id, id_left, 0} --[not exist?]-->
		// {id, 0, 0}. This always match in unitMap
		if(strlen(segment.filename) == 0){
			DEBUG_INFO("Key not found, try {%d,%d,%d}", itu->key.id, 0, itu->key.id_right);
			key.id_left = 0;
			segment = this->unitMap[key];
			if(strlen(segment.filename) == 0){
				DEBUG_INFO("Key not found, try {%d,%d,%d}", itu->key.id, itu->key.id_left, 0);
				key.id_left = itu->key.id_left;
				key.id_right = 0;
				segment = this->unitMap[key];
				if(strlen(segment.filename) == 0){
					DEBUG_INFO("Key not found, try {%d,%d,%d}", itu->key.id, 0, 0);
					key.id_left = 0;
					segment = this->unitMap[key];
				}
			}
		}
		itu->segment = segment;
	}
	DEBUG_INFO("ID list created!");
}

void UnitSelector::outputIdListToFile(std::string path){
	vector<unit_t>::iterator it;
	std::ofstream ofs(path);
	//char *buff = (char*)malloc(512);
	for(it = this->idList.begin(); it != this->idList.end(); ++it){
		ofs << it->segment.filename << ".wav\t"
				<< it->segment.begin << "\t" << it->segment.end << "\n";
	}
}

std::vector<unit_t> UnitSelector::getIdList(){
	return this->idList;
}

void UnitSelector::resolveAbbreWord(string word){
	string phrase = this->specMap[word];
	if(phrase.empty()){
		// Spell every characters, send word to unresolved list for update
		// TODO: Implement a smarter approach to spell words that haven't been indexed
		this->spellWord(word);
	}else{
		this->searchPhrase(phrase);
	}
}

void UnitSelector::spellWord(std::string word){
	string phrase;
#ifdef __LIST_UNRESOLVED_WORDS__
	this->unresolvedWord.push_back(word);
	string TTS_SYS_ROOT(getenv("TTS_SYS_ROOT"));
	if(!TTS_SYS_ROOT.empty()){
		std::ofstream ofs(TTS_SYS_ROOT + TTS_UNRESOLVED_PATH);
		ofs << word;
		ofs.close();
	}
#endif
	for(string::iterator it = word.begin(); it != word.end(); ++it){
		if((*it >= 'A')&&(*it <= 'Z'))
			phrase += this->alpha_text[*it - 'A'];
	}
	this->searchPhrase(phrase);
}

void UnitSelector::searchPhrase(std::string phrase){
	vector<string> tokens;
	unsigned int id = 0;
	unit_t unit;
	this->splitString(&tokens, &phrase, ' ');
	phrase.clear();
	for(vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it){
		phrase += *it;
		if((id =this->unitIdMap[phrase]) == 0){
			this->idList.push_back(unit);
			phrase.clear();
		}
		unit.key.id = id;
		phrase += " ";
	}
}

// Fill unit id left and right in struct key
void UnitSelector::fillNeighborId(void){
	std::vector<unit_t>::iterator it;
	it = this->idList.begin();
	it->key.id_left = 0;
	it->key.id_right = (it+1)->key.id;
	for(it = this->idList.begin() + 1; it != (this->idList.end() - 1); ++it){
		it->key.id_left = (it-1)->key.id;
		it->key.id_right = (it+1)->key.id;
	}
	it->key.id_left = (it-1)->key.id;
	it->key.id_right = 0;
}

inline void UnitSelector::splitString(vector<string> *tokens, const string *s, char delim){
	stringstream ss(*s);
	string item;
	tokens->clear();		// Clear before using
	while (getline(ss, item, delim)) {
		tokens->push_back(item);
	}
}
// Overload method
vector<string> UnitSelector::splitString(const string &s, char delim){
	stringstream ss(s);
	string item;
	vector<string> tokens;
	while (getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

wav_header_t *UnitSelector::initWavHeader(wav_header_t *hwav){
	memset(hwav,0,sizeof(wav_header_t));
	strncpy(hwav->chunk_id, "RIFF", 4);
	strncpy(hwav->format, "WAVE", 4);
	strncpy(hwav->sub_chunk1_id, "fmt ",4);
	strncpy(hwav->sub_chunk2_id, "data", 4);
	hwav->sub_chunk1_size = 16;
	hwav->audio_format = 1;
	hwav->num_channels = 1;
	hwav->sample_rate = 16000;
	hwav->bits_per_sample = 16;
	hwav->byte_rate = hwav->sample_rate * hwav->num_channels * hwav->bits_per_sample / 8;
	hwav->block_align = hwav->num_channels * hwav->bits_per_sample / 8;
	hwav->sub_chunk2_size = 0;
	hwav->chunk_size = 36;
	return hwav;
}

void UnitSelector::createWavFile(std::string path){
	DEBUG_INFO("Creating wave file %s", path.c_str());
	std::string TTS_SYS_ROOT = getenv("TTS_SYS_ROOT");
	if(TTS_SYS_ROOT.empty()){
		DEBUG_FATAL("TTS_SYS_ROOT is not set");
		return;
	}
	wav_header_t hwav;
	this->initWavHeader(&hwav);
	std::ifstream inputWav;
	std::ofstream outputWav(path);
	vector<unit_t>::iterator itu;
	unsigned int segment_size, data_size = 0;
	char *buffer;
	outputWav.write((const char *)&hwav, sizeof(wav_header_t));
	for(itu = this->idList.begin(); itu != this->idList.end(); ++itu){
		DEBUG_INFO("Concatenating segment {%s,%d,%d}",
				itu->segment.filename, itu->segment.begin, itu->segment.end);
		segment_size = (itu->segment.end - itu->segment.begin)*32;
		if(segment_size < MAX_ALLOC_SIZE){
			buffer = new char[segment_size];
		}else{
			DEBUG_INFO("Cannot allocate buffer size = %d", segment_size);
			continue;
		}
		inputWav.open((TTS_SYS_ROOT + TTS_DATABASE_PATH + std::string(itu->segment.filename) + ".wav").c_str(),
				std::ifstream::in);
		if(inputWav.is_open()){
			inputWav.seekg(itu->segment.begin*32);
			inputWav.read(buffer, segment_size);
			outputWav.write(buffer, segment_size);
			data_size += segment_size;
		}else{
			DEBUG_ERROR("Cannot open file %s", itu->segment.filename);
		}
		delete[] buffer;
		inputWav.close();
	}
	hwav.sub_chunk2_size = data_size;
	hwav.chunk_size = hwav.sub_chunk2_size + 36;
	outputWav.seekp(0);
	outputWav.write((const char *)&hwav, sizeof(wav_header_t));
	outputWav.close();
	DEBUG_INFO("Created wave file %s", path.c_str());
}

void UnitSelector::outputUnresolvedListToFile(std::string path){
#ifdef __LIST_UNRESOLVED_WORDS__
	std::ofstream ofs(path);
	vector<std::string>::iterator it;
	for(it = this->unresolvedWord.begin(); it != this->unresolvedWord.end(); ++it){
		ofs << *it << endl;
	}
#endif
}
} /* namespace iHearTech */

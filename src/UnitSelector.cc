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

std::tr1::unordered_map<std::string, std::string> UnitSelector::unicode_map = UnitSelector::init_unicode_map();

UnitSelector::UnitSelector(){
	this->_good = true;
	this->enable_unresolved_words_output = false;
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
	if(TTS_SYS_ROOT.empty()){
		DEBUG_FATAL("TTS_SYS_ROOT is not set");
		return;
	}
	std::ifstream ifsUnitId(TTS_SYS_ROOT + TTS_UNIT_ID_PATH);
	std::ifstream ifsUnitSel(TTS_SYS_ROOT + TTS_UNIT_SELECTOR_PATH);
	std::ifstream ifsSpecial(TTS_SYS_ROOT + TTS_SPEC_PATH);
	if(!(ifsUnitId.is_open()&&ifsUnitSel.is_open()&&ifsSpecial.is_open())){
		DEBUG_ERROR("Cannot open TTS dictionary, return now");
		this->_good = false;
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
		this->_good = false;
		return;
	}
	this->unitIdMap.serialize(unitIdMapSerializer(),fpUnitId);
	fclose(fpUnitId);
	// Serialize Unit Selector map
	FILE *fpUnitSel = fopen((TTS_SYS_ROOT + TTS_UNIT_SELECTOR_DAT).c_str(),"w");
	if(!fpUnitSel){
		DEBUG_ERROR("Cannot load unit map data");
		this->_good = false;
		return;
	}
	if(this->unitMap.empty()){
		DEBUG_ERROR("Unit map uninitialized, run initMaps first!");
		this->_good = false;
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
		this->_good = false;
		return;
	}
	this->specMap.serialize(specMapSerializer(), fpSpecDict);
	fclose(fpSpecDict);
}

void UnitSelector::restoreMaps(void){
	DEBUG_INFO("Restoring hash maps");
	char *root = getenv("TTS_SYS_ROOT");
	if(root == NULL){
		printf("%sUnitSelector::restoreMaps: TTS_SYS_ROOT variable is not set\n"
				"Abort operation...%s\n",KRED,KNRM);
		return;
	}
	std::string TTS_SYS_ROOT(root);
	FILE *fpUnitId = fopen((TTS_SYS_ROOT + TTS_UNIT_ID_DAT).c_str(),"r");
	// De-serialize unit id map
	if(!fpUnitId){
		DEBUG_ERROR("Cannot load unit ID map data");
		this->_good = false;
		return;
	}
	this->unitIdMap.set_empty_key(std::string());
	this->unitIdMap.unserialize(unitIdMapSerializer(), fpUnitId);
	fclose(fpUnitId);

	// De-serialize unit map
	FILE *fpUnitSel = fopen((TTS_SYS_ROOT + TTS_UNIT_SELECTOR_DAT).c_str(), "r");
	if(!fpUnitSel){
		DEBUG_ERROR("Cannot load unit map data");
		this->_good = false;
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
		this->_good = false;
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
	DEBUG_INFO("Creating ID list");
	vector<string>::iterator its;
	string phrase;
	unit_t unit;
	unsigned int id;
	/*
	 * Split input string into sentences
	 * TODO: Indicate end-of-sentence symbol for tokenizing, should be '\n' ??
	 */
	DEBUG_INFO("Slit input text into sentences");
	vector<string> sentence;
	vector<string> words;
	sentence.clear();
	this->splitString(&sentence, &str, '\n');
	/*
	 * Start searching each sentence
	 */
	this->idList.clear();
	for(its = sentence.begin(); its != sentence.end(); ++its){
		/*
		 * Split phrase into words
		 */
		DEBUG_INFO("Split sentence into words");
		words.clear();
		this->splitString(&words, &(*its), ' ');
		/*
		 * Search for longest phrase contents combination from 'words'
		 */
		std::vector<string>::iterator itw = words.begin();
		while(itw != words.end()){
			int i;
			if((itw + MAX_WORD_IN_PHRASE) < words.end()){
				i = MAX_WORD_IN_PHRASE;
			}else i = (words.end() - itw);
			DEBUG_INFO("Searching phrase with maximum length = %d", i);
			for( ;i > 0; i--){
				phrase.clear();
				for(int j = 0; j < i; j++){
					phrase += *(itw + j) + " ";
				}
				phrase.erase(phrase.length()-1,1);
				id = 0;
				if(!phrase.empty()){
					if((id = this->unitIdMap[phrase]) != 0){
						itw += i;
						break;
					}
				}
			}
			if(id == 0){
				phrase.clear();
				phrase += *itw;
				this->resolveAbbreWord(phrase);
				itw++;
			}else{
				DEBUG_INFO("Longest phrase found, id = %d", id);
				unit.key.id = id;
				this->idList.push_back(unit);
			}
		}
	}
	/*
	 * All phrase-id have been found, wave segment of each unit is select
	 * from the unitMap base on left and right unit
	 */
	DEBUG_INFO("All phrase id have been found, start unit selector");
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
		/*
		 * Select the most identical unit from unitMap
		 * Search process:
		 * {id, id_left, id_right} --[not exist?]-->
		 * {id, 0, id_right} --[not exist?]-->
		 * {id, id_left, 0} --[not exist?]-->
		 * {id, 0, 0}. This always match in unitMap
		 */
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

bool UnitSelector::good(void){
	return this->_good;
}

void UnitSelector::createWavFile(std::string path){
	DEBUG_INFO("Creating wave file %s", path.c_str());
	std::string TTS_SYS_ROOT(getenv("TTS_SYS_ROOT"));
	if(TTS_SYS_ROOT.empty()){
		DEBUG_FATAL("TTS_SYS_ROOT is not set");
		return;
	}
	wav_header_t hwav;
	this->initWavHeader(&hwav);
	FILE *outputWav = fopen(path.c_str(), "w+b");
	vector<unit_t>::iterator itu;
	unsigned int segment_size, data_size = 0;
	char *buffer;
	fwrite((const void *)&hwav, sizeof(wav_header_t), 1, outputWav);
	for(itu = this->idList.begin(); itu != this->idList.end(); ++itu){
		DEBUG_INFO("Concatenating segment {%s,%d,%d}",
				itu->segment.filename, itu->segment.begin, itu->segment.end);
		segment_size = (itu->segment.end - itu->segment.begin)*32;
		if(segment_size < MAX_ALLOC_SIZE){
			buffer = new char[segment_size];
			memset(buffer, 0, segment_size);
		}else{
			DEBUG_INFO("Cannot allocate buffer size = %d", segment_size);
			continue;
		}
		std::ifstream inputWav((TTS_SYS_ROOT + TTS_DATABASE_PATH + std::string(itu->segment.filename) + ".wav").c_str(),
				std::ifstream::in);
		if(inputWav.is_open()){
			inputWav.seekg(itu->segment.begin*32 + 44);
			inputWav.read(buffer, segment_size);
			fwrite(buffer, sizeof(char), segment_size, outputWav);
			data_size += segment_size;
		}else{
			DEBUG_ERROR("Cannot open file %s", itu->segment.filename);
			this->_good = false;
		}
		delete[] buffer;
		inputWav.close();
		inputWav.clear();
	}
	hwav.sub_chunk2_size = data_size;
	hwav.chunk_size = hwav.sub_chunk2_size + 36;
	fseek(outputWav, 0, 0);
	fwrite((const void *)&hwav, sizeof(wav_header_t), 1, outputWav);
	fclose(outputWav);
	DEBUG_INFO("Created wave file %s", path.c_str());
}

void UnitSelector::outputUnresolvedListToFile(std::string path){
#ifdef __LIST_UNRESOLVED_WORDS__
	if(this->enable_unresolved_words_output){
		FILE *pFile = fopen(path.c_str(), "w");
		if(pFile == NULL){
			DEBUG_ERROR("Cannot open file: %s", path.c_str());
			return;
		}
		fprintf(pFile, "Unresolved word list %s\n", __DATE__);
		std::vector<std::string>::iterator it;
		for(it = this->unresolvedWord.begin(); it != this->unresolvedWord.end(); ++it){
			fprintf(pFile, "Unresolved word:\t%s\n", it->c_str());
		}
		fclose(pFile);
	}
#endif
}

void UnitSelector::resolveAbbreWord(string word){
	DEBUG_INFO("Searching word in specMap: %s", word.c_str());
	if(word.empty()) return;
	// Unicode character mapping
	std::tr1::unordered_map<std::string, std::string>::iterator it;
	for(it = this->unicode_map.begin(); it != this->unicode_map.end(); ++it){
		std::size_t pos;
		if((pos = word.find(it->first)) != std::string::npos){
			DEBUG_INFO("Replace unicode character %s with %s", it->first.c_str(), it->second.c_str());
			word.replace(pos, it->first.length(), it->second);
			// Re-search in unit map
			this->searchPhrase(word);
			return;
		}
	}
	string phrase = this->specMap[word];
	if(phrase.empty()){
		// Spell every characters, send word to unresolved list for update
		// TODO: Implement a smarter approach to spell words that haven't been indexed
		DEBUG_WARNING("No matched word is found!");
		this->spellWord(word);
	}else{
		phrase.erase(phrase.find('\r'), 1);
		this->searchPhrase(phrase);
	}
}

void UnitSelector::spellWord(std::string word){
#ifdef __LIST_UNRESOLVED_WORDS__
	if(this->enable_unresolved_words_output){
		this->unresolvedWord.push_back(word);
	}
#endif
	string phrase;
	if(word.empty()) return;
	for(string::iterator it = word.begin(); it != word.end(); ++it){
		if((*it >= 'A')&&(*it <= 'Z'))
			phrase += this->alpha_text[*it - 'A'] + " ";
	}
	this->searchPhrase(phrase);
}

void UnitSelector::searchPhrase(std::string phrase){
	vector<string> tokens;
	unsigned int id = 0;
	unit_t unit;
	if(phrase.empty()) return;
	this->splitString(&tokens, &phrase, ' ');
	phrase.clear();
	int i;
	vector<string>::iterator it = tokens.begin();
	if((it + MAX_WORD_IN_PHRASE) < tokens.end()){
		i = MAX_WORD_IN_PHRASE;
	}else i = tokens.end() - it;
	for( ; i > 0; i--){
		phrase.clear();
		for(int j = 0; j < i; j++){
			phrase += *(it + j) + " ";
		}
		phrase.erase(phrase.length()-1,1);
		if(!phrase.empty()){
			if((id = this->unitIdMap[phrase]) != 0){
				it += i;
				unit.key.id = id;
				this->idList.push_back(unit);
				break;
			}
		}
	}
}

void UnitSelector::fillNeighborId(void){
	DEBUG_INFO("Fill unit id left and right in struct key");
	std::vector<unit_t>::iterator it;
	it = this->idList.begin();
	it->key.id_left = 0;
	if(this->idList.size() > 1) it->key.id_right = (it+1)->key.id;
	else it->key.id_right = 0; return;
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

std::tr1::unordered_map<std::string, std::string> UnitSelector::init_unicode_map(){
	std::tr1::unordered_map<std::string, std::string> map;
	map["Ậ"] = "Ậ";
	map["Ồ"] = "Ồ";
	map["Ề"] = "Ề";
	map["Ù"] = "Ù";
	map["Ỳ"] = "Ì";
	return map;
}

} /* namespace iHearTech */

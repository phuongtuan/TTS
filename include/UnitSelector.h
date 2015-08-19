/*
 * UnitSelector.h
 *
 *  Created on: Jul 16, 2015
 *      Author: messier
 */

#ifndef UNITSELECTOR_H_
#define UNITSELECTOR_H_
#include <tr1/unordered_map>
#include <sparsehash/dense_hash_map>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fstream>
#include <sstream>
#include "debug.h"
#include <vector>
#include <config.h>
#include <stdint.h>

#define MAX_ALLOC_SIZE			500000
#define MAX_WORD_IN_PHRASE		13
#define MAX_FILENAME_LENGTH		24

#define TTS_UNIT_ID_PATH		"/database/Dictionary/UnitID.txt"
#define TTS_UNIT_SELECTOR_PATH	"/database/Dictionary/UnitSelector.txt"
#define TTS_UNIT_ID_DAT			"/database/Dictionary/UnitID.dat"
#define TTS_UNIT_SELECTOR_DAT	"/database/Dictionary/UnitSelector.dat"
#define TTS_SPEC_PATH			"/database/Dictionary/SpecialDict.txt"
#define TTS_SPEC_DAT			"/database/Dictionary/SpecialDict.dat"
#define TTS_DATABASE_PATH		"/database/Sound/"
#define TTS_UNRESOLVED_PATH		"/log/unresolved_words.txt"

using namespace std::tr1;
using google::dense_hash_map;

namespace iHearTech {
struct unit_key_t{
	unsigned int id;
	unsigned int id_left;
	unsigned int id_right;
	bool operator ==(const unit_key_t &k1) const{
		return (k1.id == this->id)&&
				(k1.id_left == this->id)&&
				(k1.id_right == this->id);
	}
};
struct wav_segment_t{
	char filename[MAX_FILENAME_LENGTH];		// Wav file contains unit
	unsigned int begin;		// Begin position in wav file
	unsigned int end;		// End position in wav file
	bool operator ==(const wav_segment_t &w1) const{
		return (!strcmp(w1.filename,this->filename))&&
				(w1.begin == this->begin)&&
				(w1.end == this->end);
	}
};
struct unit_t{
	unit_key_t key;
	wav_segment_t segment;
	bool operator ==(const unit_t &unit) const {
		return (unit.key == this->key)&&
				(unit.segment == this->segment);
	}
};

struct eqstr{
	bool operator()(const std::string s1, const std::string s2) const
	{
		return s1.compare(s2)==0;
	}
};
struct eqkey{
	bool operator()(unit_key_t k1, unit_key_t k2) const
	{
		return ((k1.id == k2.id)&&
				(k1.id_left == k2.id_left)&&
				(k1.id_right == k2.id_right));
	}
};

struct hash_unit_key{
	std::size_t operator()(const unit_key_t& k) const
	{
		const unsigned char *p = reinterpret_cast<const unsigned char *>(&k);
		std::size_t h = 2166136261;
		for (unsigned int i = 0; i < sizeof(k); ++i)
			h = (h * 16777619) ^ p[i];
		return h;
	}
};
// FNV general bit-string hash function
struct hash_str{
	std::size_t operator()(const std::string str) const
	{
		const unsigned char *p = reinterpret_cast<const unsigned char *>(str.c_str());
		unsigned int str_sz = str.size();
		std::size_t h = 2166136261;
		for (unsigned int i = 0; i < str_sz; ++i)
			h = (h * 16777619) ^ p[i];
		return h;
	}
};

struct unitMapSerializer{
	bool operator()(FILE *fp, const std::pair<const unit_key_t, wav_segment_t>&value)const{
		// Write the key
		if(fwrite(&value.first, sizeof(value.first), 1, fp) != 1)
			return false;
		// Write the value
		if(fwrite(&value.second, sizeof(value.second), 1, fp) != 1)
			return false;
		return true;
	}
	bool operator()(FILE *fp, std::pair<const unit_key_t, wav_segment_t> *value) const{
		if(fread(const_cast<unit_key_t *>(&value->first),sizeof(value->first), 1, fp) != 1)
			return false;
		if(fread((wav_segment_t *)(&value->second),sizeof(value->second), 1, fp) != 1){
			return false;
		}
		return true;
	}
};
struct unitIdMapSerializer{
	bool operator()(FILE *fp, const std::pair<const std::string, unsigned int>&value) const {
		const unsigned char size = value.first.length();
		if(fwrite(&size, 1, 1, fp) != 1)
			return false;
		if(fwrite(value.first.data(), size, 1, fp) != 1)
			return false;
		if(fwrite(&value.second, sizeof(value.second), 1, fp) != 1)
			return false;
		return true;
	}
	bool operator()(FILE *fp, std::pair<const std::string, unsigned int> *value) const {
		unsigned char size;
		if(fread(&size, 1, 1, fp) != 1)
			return false;
		char *buf = new char[size];
		if(fread(buf, size, 1, fp) != 1){
			delete[] buf;
			return false;
		}
		(const_cast<std::string *>(&value->first))->assign(buf, size);
		if(fread(&value->second, sizeof(value->second), 1, fp) != 1)
			return false;
		delete[] buf;
		return true;
	}
};
struct specMapSerializer{
	bool operator()(FILE *fp, const std::pair<const std::string, std::string>&value)const{
		unsigned char size;
		size = value.first.length();
		if(fwrite(&size, 1, 1, fp) != 1)
			return false;
		if(fwrite(value.first.data(), size, 1, fp) != 1)
			return false;
		size = value.second.length();
		if(fwrite(&size, 1, 1, fp) != 1)
			return false;
		if(fwrite(value.second.data(), size, 1, fp) != 1)
			return false;
		return true;
	}
	bool operator()(FILE *fp, std::pair<const std::string, std::string>*value)const{
		unsigned char size;
		if(fread(&size, 1, 1, fp) != 1)
			return false;
		char *buf = new char[size];
		if(fread(buf, size, 1, fp) != 1){
			delete[] buf;
			return false;
		}
		(const_cast<std::string *>(&value->first))->assign(buf, size);
		delete[] buf;
		if(fread(&size, 1, 1, fp) != 1)
			return false;
		buf = new char[size];
		if(fread(buf, size, 1, fp) != 1){
			delete[] buf;
			return false;
		}
		value->second.assign(buf,size);
		delete[] buf;
		return true;
	}
};

struct wav_header_t {
	char chunk_id[4];
	uint32_t chunk_size;
	char format[4];
	char sub_chunk1_id[4];
	uint32_t sub_chunk1_size;
	uint16_t audio_format;
	uint16_t num_channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
	char sub_chunk2_id[4];
	uint32_t sub_chunk2_size;
};

class UnitSelector {
public:
	UnitSelector();
	virtual ~UnitSelector();
	void initMaps(void);
	void storeMaps(void);
	void restoreMaps(void);
	void createIdList(std::string);
	void outputIdListToFile(std::string path);
	std::vector<unit_t> getIdList();
	void createWavFile(std::string path);
	void outputUnresolvedListToFile(std::string path);
	bool good(void);
#ifndef __UNITTEST__
private:
#endif
	bool _good;
	static std::tr1::unordered_map<std::string, std::string> unicode_map;
	unit_key_t default_key = {0,0,0};
	const static std::string alpha_text[];
	dense_hash_map<std::string , unsigned int, hash_str, eqstr> unitIdMap;
	dense_hash_map<unit_key_t, wav_segment_t, hash_unit_key, eqkey> unitMap;
	dense_hash_map<std::string, std::string, hash_str, eqstr> specMap;
	// Output ID list for concatenation process
	std::vector<unit_t> idList;
	// Vector for unresolved word
	std::vector<std::string> unresolvedWord;
	// Private method
	static std::vector<std::string> splitString(const std::string &s, char delim);
	static void splitString(std::vector<std::string> *tokens, const std::string *s, char delim);
	void resolveAbbreWord(std::string word);
	void spellWord(std::string word);
	void fillNeighborId(void);
	void searchPhrase(std::string phrase);
	static wav_header_t *initWavHeader(wav_header_t *hwav);
	static std::tr1::unordered_map<std::string, std::string> init_unicode_map();
};

} /* namespace iHearTech */

#endif /* UNITSELECTOR_H_ */

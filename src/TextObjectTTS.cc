/*
 * TextObjectTTS.cpp
 *
 *  Created on: Jul 5, 2015
 *      Author: messier
 */
#include "TextObjectTTS.h"
using iHearTech::TextObjectTTS;

namespace iHearTech {

const char *TextObjectTTS::num_text[] = {"KHÔNG ", "MỘT ", "HAI ", "BA ", "BỐN ", "NĂM ", "SÁU ", "BẢY ", "TÁM ", "CHÍN "};
const char *TextObjectTTS::kh[] = {"", "NGÀN ", "TRIỆU ", "TỶ ", "NGÀN TỶ ", "TRIỆU TỶ ", "TỶ TỶ "};

//*************************************************************************
// CAUTION: This type of declaration can only be compiled with c++11 flag *
//*************************************************************************
std::vector<pattern_t> TextObjectTTS::regexPattern = {
		{(char *)"(“|”|<.*?>|\302\240|\342\200\246|[.][.])", &normalize_rmhtml},
		{(char *)"(NGÀY )?([[:digit:]]{1,2})-([[:digit:]]{1,2})-([[:digit:]]{4}) ?", &normalize_date_1},
		{(char *)"(NGÀY )?([[:digit:]]{1,2})/([[:digit:]]{1,2})/([[:digit:]]{4}) ?", &normalize_date_1},
		{(char *)"(NGÀY )?([[:digit:]]{1,2})[.]([[:digit:]]{1,2})[.]([[:digit:]]{4}) ?", &normalize_date_1},
		{(char *)"(TỪ |NGÀY )([[:digit:]]{1,2})[ ]?(-|ĐẾN)[ ]?([[:digit:]]{1,2})(\\.|\\/)([[:digit:]]{1,2} ?)", &normalize_date_2},
		{(char *)"(TỪ |THÁNG )([[:digit:]]{1,2})[ ]?(-|ĐẾN)[ ]?([[:digit:]]{1,2})(\\.|\\/)([[:digit:]]{4}) ?", &normalize_date_3},
		{(char *)"(THÁNG )?([[:digit:]]{1,2})(\\.|-|\\/)([[:digit:]]{4}) ?", &normalize_date_4},
		{(char *)"(NGÀY |SÁNG |TRƯA |CHIỀU |TỐI |ĐÊM |KHUYA |HAI |BA |TƯ |NĂM |SÁU |BẢY |NHẬT |QUA |NAY )([[:digit:]]{1,2})[\\.\\/-]([[:digit:]]{1,2}) ?", &normalize_date_5},
		{(char *)"(THỨ |HẠNG |GIẢI )([[:digit:]]+)",&normalize_rank},
		{(char *)"([[:digit:]]{1,2})(H|G|:)([[:digit:]]{1,2})?( AM| PM)? ?", &normalize_time},
		{(char *)"([[:digit:]]+)[ ]?(KG|G|MG|KM|M|CM|MM|UM|NM|HA)(2|3)?[ |.|,|:|;]", &normalize_size},
		{(char *)"([[:digit:]]+) O C",&normalize_degree},
		{(char *)"([[:digit:]]+)/([[:digit:]]+)",&normalize_proportion},
		{(char *)"([[:digit:]]+),([[:digit:]]+) ?", &normalize_numcomma},
		{(char *)"[[:digit:]](\\.)[[:digit:]]", &normalize_numdot},
		{(char *)"(\\$)?([[:digit:]]+)(\\$|%)? ?", &normalize_snum},
		{(char *)"(^| )(TP|Q|H|P|X)\\.", &normalize_cidis},
		{(char *)"/", &normalize_slash},
		// These patterns should be at the end of this list
		{(char *)"(,|;|:|\\(|\\)|\\.{3}|\\?)", &normalize_addsp},
		{(char *)"(\\.|!| \n)", &normalize_punctuation},
		{(char *)"(“|”|-|\"|\\*)",&normalize_remove},
		{(char *)"( {2,}|\n{2,})",&normalize_rmduplicate},
};

//***********************************************************
// Public methods
//***********************************************************
TextObjectTTS::TextObjectTTS(){
	this->_good = true;
}
TextObjectTTS::~TextObjectTTS(){
}

// Run normalize process on internal string
void TextObjectTTS::normalize(){
	DEBUG_INFO("Start normalize process");
	if(this->inputStr.empty()){
		DEBUG_ERROR("Input string is empty");
		return;
	}
	int reti;
	regex_t regex;
	std::string temp;
	regmatch_t pmatch[REGMATCH_MAX_SIZE];
	// Upper case string
	normalize_uppercase(&temp,&this->inputStr);
	std::vector<pattern_t>::iterator it;
	for(it = regexPattern.begin(); it != regexPattern.end(); it++){
		DEBUG_INFO("Compiling regex pattern \"%s\"", it->pattern);
		reti = regcomp(&regex, it->pattern, REG_EXTENDED);
		if(reti){
			DEBUG_ERROR("Cannot compile regex pattern, return code = %d", reti);
		}else{
			while(search_pattern((char *)it->pattern, &regex, &temp, pmatch)!=NULL){
				// if expression is matched, replace matched string with normalized string
				// TODO: regex should be compile only one time
				it->handler(&temp, pmatch);
			}
			DEBUG_INFO("Freeing allocated regex");
			regfree(&regex);
			DEBUG_INFO("Regex freed");
		}
	}
	this->outputStr = temp;
	DEBUG_INFO("Store normalize string to ./text.nml");
	std::ofstream ofs("./text.nml", std::ofstream::out);
	ofs << this->outputStr;
	ofs.close();
	DEBUG_INFO("End of normalize process");
}

// Set internal string
void TextObjectTTS::setInputStr(std::string str){
	this->inputStr = str;
}
// Get internal string
std::string TextObjectTTS::getOutputStr(){
	return this->outputStr;
}

// Download webpage and extract text from it
bool TextObjectTTS::getTextFromUrl(std::string url){
	DEBUG_INFO("Getting news content from URL");
	int res = 0;
	regex_t regex1, regex2;
	regmatch_t pmatch[3];
	this->inputStr.clear();
	char *root = getenv("TTS_SYS_ROOT");
	if(root == NULL){
		printf("%sTextObjectTTS::getTextFromUrl: TTS_SYS_ROOT variable is not set!\n"
				"Abort operation...%s\n",KRED,KNRM);
		return false;
	}
	std::string TTS_SYS_ROOT(root);
	if(TTS_SYS_ROOT.empty()){
		return false;
	}
	system(("wget -O $TTS_SYS_ROOT/news.html " + url + " 2> /dev/null").c_str());
	std::ifstream ifs(TTS_SYS_ROOT + "news.html");
	std::ofstream ofs(TTS_SYS_ROOT + "news.txt");
	if(!ifs.is_open()){
		DEBUG_ERROR("Error while download web page or permission denied");
		this->_good = false;
		return false;
	}
	std::string str((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
	if(str.empty()){
		DEBUG_ERROR("HTML file is empty");
		return false;
	}
	res += regcomp(&regex1, "((<p class=\"MsoNormal\">(<[^<>]>)*)|<p>|(<p class=\"txt-head\">TT - )|<p class=\"txt-head\">TTO - )([[:alnum:]]|Ă|Â|Ê|Ơ|Ô|Ư|Đ|[\"])", REG_EXTENDED);
	res += regcomp(&regex2, "</p>", REG_EXTENDED);
	if(res){
		DEBUG_ERROR("Cannot compile regex pattern, return now.");
		this->_good = false;
		return false;
	}
	//char *cptr = (char*)malloc(str.length() + 1);
	char *cptr = new char[str.length() + 1];
	strcpy(cptr, str.c_str());
	char *ptr = cptr;
	while(ptr < (cptr + str.size())){
		res = regexec(&regex1, ptr, 3, pmatch, REG_NOTBOL);
		if(res) break;
		ptr += pmatch[1].rm_eo;
		res = regexec(&regex2, ptr, 3, pmatch, REG_NOTBOL);
		if(res) break;
		ptr[pmatch[0].rm_so] = 0;
		this->inputStr.append(ptr);
		this->inputStr.append("\n");
		ptr += pmatch[0].rm_eo;
	}
	ofs << this->inputStr;
	delete[] cptr;
	//remove((TTS_SYS_ROOT + "/news.html").c_str());
	DEBUG_INFO("Store news content in file %s", (TTS_SYS_ROOT + "news.txt").c_str());
	return true;
}

bool TextObjectTTS::good(){
	return this->_good;
}
//***********************************************************
// Private methods
//***********************************************************
std::string* TextObjectTTS::normalize_date_1(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_date_1 is called");
	std::string temp;
	std::string num_string;
	int num;
	temp.clear();
	temp.append("NGÀY ");
	num = atoi(src->c_str() + pmatch[2].rm_so);
	temp.append(*normalize_number(&num_string, num) + "THÁNG ");
	num = atoi(src->c_str() + pmatch[3].rm_so);
	temp.append(*normalize_number(&num_string, num) + "NĂM ");
	num = atoi(src->c_str() + pmatch[4].rm_so);
	temp.append(*normalize_number(&num_string, num));
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s",temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_date_2(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_date_2 is called");
	std::string temp;
	std::string num_string;
	int num;
	temp.clear();
	temp.append("TỪ NGÀY ");
	num = atoi(src->c_str() + pmatch[2].rm_so);
	temp.append(*normalize_number(&num_string, num) + "ĐẾN ");
	num = atoi(src->c_str() + pmatch[4].rm_so);
	temp.append(*normalize_number(&num_string, num) + "THÁNG ");
	num = atoi(src->c_str() + pmatch[6].rm_so);
	temp.append(*normalize_number(&num_string, num));
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s", temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_date_3(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_date_3 is called");
	std::string temp;
	std::string num_string;
	int num;
	temp.clear();
	temp.append("TỪ THÁNG ");
	num = atoi(src->c_str() + pmatch[2].rm_so);
	temp.append(*normalize_number(&num_string, num) + "ĐẾN ");
	num = atoi(src->c_str() + pmatch[4].rm_so);
	temp.append(*normalize_number(&num_string, num) + "NĂM ");
	num = atoi(src->c_str() + pmatch[6].rm_so);
	temp.append(*normalize_number(&num_string, num));
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s", temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_date_4(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_date_4 is called");
	std::string temp;
	std::string num_string;
	int num;
	temp.clear();
	temp.append("THÁNG ");
	num = atoi(src->c_str() + pmatch[2].rm_so);
	temp.append(*normalize_number(&num_string, num) + "NĂM ");
	num = atoi(src->c_str() + pmatch[4].rm_so);
	temp.append(*normalize_number(&num_string, num));
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s", temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_date_5(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_date_4 is called");
	std::string temp;
	std::string num_string;
	int num;
	temp.clear();
	if(pmatch[1].rm_so != -1){
		temp.append(*src, pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
	}else{
		temp.append("NGÀY ");
	}
	num = atoi(src->c_str() + pmatch[2].rm_so);
	temp.append(*normalize_number(&num_string, num) + "THÁNG ");
	num = atoi(src->c_str() + pmatch[3].rm_so);
	temp.append(*normalize_number(&num_string, num));
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s", temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_rank(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalized_rank is called");
	std::string num_string;
	int num;
	num = atoi(src->c_str() + pmatch[2].rm_so);
	if(num == 1){
		src->replace(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so, "NHẤT");
	}else if(num == 4){
		src->replace(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so, "TƯ");
	}else{
		src->replace(pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so,
				*normalize_number(&num_string, num));
	}
	return src;
}

std::string* TextObjectTTS::normalize_time(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_time is called");
	std::string temp;
	std::string num_string;
	int num;
	temp.clear();
	num = atoi(src->c_str() + pmatch[1].rm_so);
	temp.append(*normalize_number(&num_string, num) + "GIỜ ");
	if(pmatch[3].rm_so != -1){
		num = atoi(src->c_str() + pmatch[3].rm_so);
		temp.append(*normalize_number(&num_string, num) + "PHÚT ");
	}
	if(pmatch[4].rm_so != -1){
		if(src->substr(pmatch[4].rm_so, pmatch[4].rm_eo - pmatch[4].rm_so) == " AM"){
			temp.append("SÁNG ");
		}
		if(src->substr(pmatch[4].rm_so, pmatch[4].rm_eo - pmatch[4].rm_so) == " PM"){
			temp.append("CHIỀU ");
		}
	}
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s", temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_snum(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_snum is called");
	std::string temp;
	std::string num_string;
	int num;
	temp.clear();
	num = atoi(src->c_str() + pmatch[2].rm_so);
	if((src->c_str()[pmatch[1].rm_so] == '$')||(src->c_str()[pmatch[3].rm_so] == '$')){
		temp.append(*normalize_number(&num_string, num) + "ĐÔ LA MỸ ");
	}else if(src->c_str()[pmatch[3].rm_so] == '%'){
		temp.append(*normalize_number(&num_string, num) + "PHẦN TRĂM ");
	}else{
		temp.append(*normalize_number(&num_string, num));
	}
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s", temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_remove(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_remove is called");
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, " ");
	return src;
}

std::string* TextObjectTTS::normalize_rmduplicate(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("Remove excessive blanks and $SP");
	src->erase(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so - 1);
	return src;
}

std::string* TextObjectTTS::normalize_size(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_size is called");
	std::string temp;
	std::string num_string;
	int num;
	const char *cptr = src->c_str();
	temp.clear();
	num = atoi(cptr + pmatch[1].rm_so);
	temp.append(*normalize_number(&num_string, num));
	switch(cptr[pmatch[2].rm_so]){
	case 'K':
		if(cptr[pmatch[2].rm_so+1]=='M'){
			temp.append("KI LÔ MÉT ");
		}
		else{
			temp.append("KI LÔ GAM ");
		}
		break;
	case 'M':
		if(cptr[pmatch[2].rm_so+1]=='M'){
			temp.append("MI LI MÉT ");
		}
		else if (cptr[pmatch[3].rm_so+1]=='G'){
			temp.append("MI LI GAM ");
		}
		else{
			temp.append("MÉT ");
		}
		break;
	case 'C':
		temp.append("XEN TI MÉT ");
		break;
	case 'U':
		temp.append("MI CỜ RÔ MÉT ");
		break;
	case 'N':
		temp.append("NA NÔ MÉT ");
		break;
	case 'H':
		temp.append("HÉC TA ");
		break;
	case 'G':
		temp.append("GAM ");
		break;
	}
	if(pmatch[3].rm_so != -1){
		if(cptr[pmatch[3].rm_so] == '2'){
			temp.append("VUÔNG ");
		}
		if(cptr[pmatch[3].rm_so] == '3'){
			temp.append("KHỐI ");
		}
	}
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s", temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_numcomma(std::string *src, regmatch_t *pmatch){
	int num;
	DEBUG_INFO("normalize_numcomma is called");
	std::string num_string;
	std::string temp;
	num = atoi(src->c_str() + pmatch[1].rm_so);
	temp.append(*normalize_number(&num_string, num) + "PHẨY ");
	num = atoi(src->c_str() + pmatch[2].rm_so);
	temp.append(*normalize_number(&num_string, num));
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s", temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_numdot(std::string *src, regmatch_t *pmatch){
	src->erase(pmatch[1].rm_so, 1);
	return src;
}

std::string* TextObjectTTS::normalize_proportion(std::string *src, regmatch_t *pmatch){
	int num;
	DEBUG_INFO("normalize_proportion is called");
	std::string num_string;
	std::string temp;
	num = atoi(src->c_str() + pmatch[1].rm_so);
	temp.append(*normalize_number(&num_string, num) + "PHẦN ");
	num = atoi(src->c_str() + pmatch[2].rm_so);
	temp.append(*normalize_number(&num_string, num));
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, temp);
	DEBUG_INFO("Normalized string is %s", temp.c_str());
	return src;
}

std::string* TextObjectTTS::normalize_cidis(std::string *src, regmatch_t *pmatch){
	DEBUG_INFO("normalize_cidis is called");
	const char *cstr;
	cstr = src->c_str();
	switch(cstr[pmatch[2].rm_so]){
	case 'T':
		src->replace(pmatch[2].rm_so, pmatch[0].rm_eo - pmatch[2].rm_so, "THÀNH PHỐ ");
		break;
	case 'Q':
		src->replace(pmatch[2].rm_so, pmatch[0].rm_eo - pmatch[2].rm_so, "QUẬN ");
		break;
	case 'P':
		src->replace(pmatch[2].rm_so, pmatch[0].rm_eo - pmatch[2].rm_so, "PHƯỜNG ");
		break;
	case 'X':
		src->replace(pmatch[2].rm_so, pmatch[0].rm_eo - pmatch[2].rm_so, "XÃ ");
		break;
	case 'H':
		src->replace(pmatch[2].rm_so, pmatch[0].rm_eo - pmatch[2].rm_so, "HUYỆN ");
		break;
	}
	DEBUG_INFO("Exit normalize_cidis");
	return src;
}

std::string* TextObjectTTS::normalize_punctuation(std::string *src, regmatch_t *pmatch){
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, " $SP\n");
	return src;
}

std::string* TextObjectTTS::normalize_addsp(std::string *src, regmatch_t *pmatch){
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, " $SP ");
	return src;
}

std::string* TextObjectTTS::normalize_rmhtml(std::string *src, regmatch_t *pmatch){
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, " ");
	return src;
}

std::string* TextObjectTTS::normalize_slash(std::string *src, regmatch_t *pmatch){
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, " MỘT ");
	return src;
}

std::string* TextObjectTTS::normalize_degree(std::string *src, regmatch_t *pmatch){
	int num;
	std::string num_string;
	num = atoi(src->c_str() + pmatch[1].rm_so);
	normalize_number(&num_string, num);
	src->replace(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so, num_string + " ĐỘ XÊ ");
	return src;
}

std::string* TextObjectTTS::normalize_number(std::string *dst, unsigned int num){
	dst->clear();
	int i, j = 1;
	for(i = 0; (num / j) >= 1000; i++){
		j *= 1000;
	}
	normalize_3digits(dst, num/j, 0);
	dst->append(kh[i]);
	num %= j;
	i--;
	j /= 1000;
	for(;i >= 0; i--){
		if(normalize_3digits(dst, num / j, 1) != NULL){
			dst->append(kh[i]);
		}
		num %= j;
		j /= 1000;
	}
	return dst;
}

std::string* TextObjectTTS::normalize_3digits(std::string *dst, short num, short hnum){
	int unit = num % 10;
	int dozen = (num / 10) % 10;
	int hundred = (num / 100) % 10;
	if(num < 10){
		if((num == 0)&&(hnum != 0)){
			return NULL;
		}else{
			if(hnum) dst->append("KHÔNG TRĂM LẺ ");
			dst->append(num_text[num]);
		}
		return dst;
	}
	if(hundred == 0){
		if(hnum){
			dst->append("KHÔNG TRĂM ");
		}
	}else{
		dst->append(num_text[hundred]);
		dst->append("TRĂM ");
	}
	if(dozen == 0){
		if(unit){
			dst->append("LẺ ");
			dst->append(num_text[unit]);
		}
	}
	else if(dozen == 1){
		dst->append("MƯỜI ");
		if(unit){
			if(unit == 5) dst->append("LĂM ");
			else dst->append(num_text[unit]);
		}
	}
	else{
		dst->append(num_text[dozen]);
		dst->append("MƯƠI ");
		if(unit){
			if(unit == 1) dst->append("MỐT ");
			else if(unit == 5) dst->append("LĂM ");
			else dst->append(num_text[unit]);
		}
	}
	return dst;
}

//********************************************************************************
// TODO: if g_utf8_strup() is okay, replace this function with macro to speed up *
//********************************************************************************
std::string* TextObjectTTS::normalize_uppercase(std::string *dst, std::string *src){
	// Upper case src string and store in dst string
	DEBUG_INFO("Start uppercase string");
	dst->clear();
	dst->append(g_utf8_strup(src->c_str(), src->size()));
	DEBUG_INFO("Finish uppercase string");
	return dst;
}

inline regmatch_t* TextObjectTTS::search_pattern(char * pattern, regex_t* regex, std::string* src, regmatch_t* pmatch){
	int reti;
	reti = regexec(regex, src->c_str(), REGMATCH_MAX_SIZE, pmatch, REG_NOTBOL);
	if(reti){
		DEBUG_WARNING("Cannot match any expression, return code = %d", reti);
		return NULL;
	}
	DEBUG_INFO("Found match expression at position %d",pmatch->rm_so);
	return pmatch;
}
} /* namespace iHearTech */

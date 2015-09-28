#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int remove_duplication(){
	FILE *pFile;
	char *text, *ptr, *end;
	char link[256], title[128];
	long int size;
	regex_t regex1, regex2;
	regmatch_t pmatch[2];
	int res, i, j;
	pFile = fopen("./index.txt", "rb");

	if(pFile == NULL) return -1;
	fseek(pFile, 0, SEEK_END);
	size = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	text = (char *)malloc(size);
	if(text == NULL) return 0;
	fread(text, sizeof(char), size, pFile);
	fclose(pFile);
	pFile = fopen("./index.txt", "wb");
	text[size] = 0;
	ptr = text;
	end = text + size;
	res = regcomp(&regex1,"http://tuoitre.vn/tin/", REG_EXTENDED);
	if(res) return -1;
	while(ptr < end){
		res = regexec(&regex1, ptr, 2, pmatch, REG_NOTBOL);
		if(res) break;
		for(j = pmatch[0].rm_so; ptr[j]!=0x09; j++);
		ptr[j] = 0;
		strcpy(link, ptr+pmatch[0].rm_so);
		ptr += j + 1;
		for(i = 0; ptr[i]!=0x0a; i++){
			title[i] = ptr[i];
		}
		// Check for duplications
		res = regcomp(&regex2, link, REG_EXTENDED);
		while(!res){
			res = regexec(&regex2, ptr, 2, pmatch, REG_NOTBOL);
			if(!res){
				// Clear duplications
				memset(ptr+pmatch[0].rm_so, 0x20, pmatch[0].rm_eo - pmatch[0].rm_so + strlen(title) + 1);
			}
		}
		// Write to file;
		title[i] = 0x0A;
		title[i+1] = 0;
		strcat(link,"	");
		fputs(link, pFile);
		fputs(title, pFile);
	}
	free(text);
	regfree(&regex1);
	regfree(&regex2);
	fclose(pFile);
	return 0;
}

int get_link(void){
	FILE *html_file;
	FILE *link_file;
	char *text;
	char *ptr, *ptr2;
	regex_t regex_1, regex_2, regex_3;
	regmatch_t pmatch[2];
	char link[256], title[256];
	int size = 0, res = 0;
	unsigned int i;

	system("wget -O ./tuoitre.html http://tuoitre.vn");
	html_file = fopen("./tuoitre.html", "rb");
	link_file = fopen("./index.txt", "wb");
	if(html_file == NULL) return -1;

	fseek(html_file, 0, SEEK_END);
	size = ftell(html_file);
	fseek(html_file, 0, SEEK_SET);
	text = (char *)malloc(size);

	if(text == NULL) return -2;
	ptr = text;

	fread(text, sizeof(char), size, html_file);

	res = regcomp(&regex_1, "http://tuoitre.vn/tin/([[:alnum:]]|-)+/201", REG_EXTENDED);
	res = regcomp(&regex_2, "<a title=", REG_EXTENDED);
	res = regcomp(&regex_3, "(</a>|</span>|<span class=)", REG_EXTENDED);

	res = regexec(&regex_2, ptr, 2, pmatch, REG_NOTBOL);
	ptr += pmatch[0].rm_eo;

	while(ptr < (text + size)){
		res = regexec(&regex_2, ptr, 2, pmatch, REG_NOTBOL);
		if(res) break;
		ptr2 = ptr + pmatch[0].rm_eo;
		*(ptr2 - 1) = 0;

		// Get html link
		res = regexec(&regex_1, ptr, 2, pmatch, REG_NOTBOL);
		if(!res){
			ptr += pmatch[0].rm_so;
			for(i = 0; ptr[i] != '"'; i++){
				link[i] = ptr[i];
			}
			link[i] = 0x09;
			link[i+1] = 0;

			// Get html title
			res = regexec(&regex_3, ptr, 2, pmatch, REG_NOTBOL);
			if(!res){
				for(i = pmatch[0].rm_so - 1; ptr[i] == 0x09; i--);
				ptr[i+1] = 0;
				while((ptr[i] != 0x09)&&(ptr[i] != '>'))  i--;
				strcpy(title, ptr+i+1);
				strcat(title,"\n");
				if(strlen(title)>2){
					//puts(link);
					//puts(title);
					fputs(link, link_file);
					fputs(title, link_file);
				}
			}
		}
		ptr = ptr2;

	}
	fclose(html_file);
	fclose(link_file);
	free(text);
	regfree(&regex_1);
	regfree(&regex_2);
	return 0;
}

inline void update_link(){
	get_link();
	remove_duplication();
}

/*
 * debug.c
 *
 *  Created on: Jul 3, 2015
 *      Author: messier
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "time.h"
// Comment out to disable debug when compile code
#define CONFIG_FILE_RELATIVE_PATH	"/config/logtrace.conf"
#define LOG_BUFFER_SIZE		1024
#define TIME_BUFFER_SIZE	128
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"

// Runtime option
#ifdef __DEBUG__
bool DEBUG_ENABLE = false;
static char *LOG_FILE_PATH = NULL;
static char *TTS_SYS_ROOT = NULL;
static char *CONFIG_FILE_PATH = NULL;
#endif

void tts_debug_init(){
#ifdef __DEBUG__
	FILE *config_file, *log_file;
	char *buffer, *ptr, *path_to_logfile, *buffer_boundary;
	unsigned int sz;
	ptr = getenv("TTS_SYS_ROOT");
	if(ptr == NULL){
		printf("%sERROR: TTS_SYS_ROOT variable is not set!\n",KRED);
		printf("Please run command:\n\texport TTS_SYS_ROOT=/PATH/TO/TTS/ROOT/%s\n\n",KNRM);
		DEBUG_ENABLE = false;
		abort();
		return;
	}else{
		TTS_SYS_ROOT = (char *)calloc(strlen(ptr)+1, sizeof(char));
		strcpy(TTS_SYS_ROOT, ptr);
		CONFIG_FILE_PATH = (char *)calloc(strlen(TTS_SYS_ROOT) + strlen(CONFIG_FILE_RELATIVE_PATH) + 1, sizeof(char));
		strcpy(CONFIG_FILE_PATH, TTS_SYS_ROOT);
		strcat(CONFIG_FILE_PATH, CONFIG_FILE_RELATIVE_PATH);
		// Open configuration file
		config_file = fopen(CONFIG_FILE_PATH, "r");
		if(config_file != NULL){
			fseek(config_file, 0L, SEEK_END);
			sz = ftell(config_file);
			rewind(config_file);
			buffer = (char *)calloc(sz + 1,sizeof(char));
			buffer_boundary = buffer + sz;
			fread(buffer, sizeof(char), sz, config_file);
			// Enable/disable debug interface
			ptr = strstr(buffer, "DEBUG_ENABLE");
			if(ptr != NULL){
				if(ptr[strlen("DEBUG_ENABLE")+1] != '0'){
					DEBUG_ENABLE = true;
				}else DEBUG_ENABLE = false;
			}
			// Configure log file location
			ptr = strstr(buffer, "LOG_FILE_PATH");
			if(ptr != NULL){
				path_to_logfile = strtok(ptr + strlen("LOG_FILE_PATH")," \t\n\r");
				while(path_to_logfile == NULL){
					path_to_logfile = strtok(NULL, " \t\n\r");
					if(path_to_logfile > buffer_boundary) break;
				}
				LOG_FILE_PATH = (char *)calloc(strlen(path_to_logfile) + strlen(TTS_SYS_ROOT) + 1,sizeof(char));
				strcpy(LOG_FILE_PATH, TTS_SYS_ROOT);
				strcat(LOG_FILE_PATH, path_to_logfile);
			}
			// Discard old log file
			if(LOG_FILE_PATH != NULL){
				log_file = fopen(LOG_FILE_PATH, "w");
				if(log_file == NULL) DEBUG_ENABLE = false;
				fclose(log_file);
			}else{
				DEBUG_ENABLE = false;
			}
			free(buffer);
		}
	}
#endif
}

void tts_log(const char *severity, const char *file, const char *func, const int line,...){
#ifdef __DEBUG__
	if(DEBUG_ENABLE){
		FILE *log_file = fopen(LOG_FILE_PATH, "a");
		va_list args;
		va_start (args, line);
		char *msg = va_arg(args,char*);
		char *buffer = (char *)calloc(LOG_BUFFER_SIZE, sizeof(char));
		char *time_buffer = (char *)calloc(TIME_BUFFER_SIZE, sizeof(char));
		time_t now = time(0);
		struct tm tstruct;
		tstruct = *localtime(&now);
		strftime(time_buffer, TIME_BUFFER_SIZE, "%d-%m-%Y %X", &tstruct);
		sprintf(buffer, "[%s] %-5s %-25s%-25s%-5i %s\n", time_buffer, severity,
				file, func, line, msg);
		vfprintf(log_file, buffer, args);
		fclose(log_file);
		va_end(args);
		free(buffer);
		free(time_buffer);
	}
#endif
}



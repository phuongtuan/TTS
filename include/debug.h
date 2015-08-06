/*
 * debug.h
 *
 *  Created on: Jul 3, 2015
 *      Author: messier
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG_INIT()		tts_debug_init()
#define DEBUG_INFO(...) 	tts_log(__DATE__,__TIME__,"INFO",__FILE__,__func__,__LINE__,__VA_ARGS__)
#define DEBUG_WARNING(...) 	tts_log(__DATE__,__TIME__,"WARN",__FILE__,__func__,__LINE__,__VA_ARGS__)
#define DEBUG_ERROR(...) 	tts_log(__DATE__,__TIME__,"ERROR",__FILE__,__func__,__LINE__,__VA_ARGS__)
#define DEBUG_FATAL(...) 	tts_log(__DATE__,__TIME__,"FATAL",__FILE__,__func__,__LINE__,__VA_ARGS__)

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

extern "C" {
void tts_debug_init();
void tts_log(const char *date, const char *time, const char *severity, const char *file,
			 const char *func, const int line,...);
}
#endif /* DEBUG_H_ */

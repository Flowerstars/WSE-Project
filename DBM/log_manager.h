/* 
 * File:   log_manager.h
 * Author: songhan
 *
 * Created on April 28, 2011, 2:31 PM
 */

#ifndef LOG_MANAGER_H
#define	LOG_MANAGER_H


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define FILE_NAME_LEN	256
#define TIME_BUF_LEN	32

typedef enum
{
  LOG_TYPE_DBM = 0,
  LOG_TYPE_CLUSTERING,
  LOG_TYPE_PARSER,

  LOG_TYPE_OTHER,
}LOG_TYPE_EN;

typedef enum
{
  LOG_LEVEL_NOLOG = 0,
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERR,
}LOG_LEVEL_EN;

void set_log_level(const char* level);
void set_log_file_info(const char* path);
void print_local_time(FILE * fd);
void create_log(LOG_LEVEL_EN level, LOG_TYPE_EN type, const char* format, va_list args);

void LOG_INFO(LOG_TYPE_EN type, const char* format, ...);
void LOG_WARN(LOG_TYPE_EN type, const char* format, ...);
void LOG_ERR(LOG_TYPE_EN type, const char* format, ...);




#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* LOG_MANAGER_H */


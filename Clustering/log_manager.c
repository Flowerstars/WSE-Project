

/*
 * File:   log_manager.c
 * Author: songhan
 *
 * Created on April 28, 2011, 2:31 PM
 */

#include "log_manager.h"


// global variables
static const char* const g_c_log_type_str[] =
{
    "DBM",          // LOG_TYPE_DBM
    "CLUSTERING",   //LOG_TYPE_CLUSTERING
    "PARSER",       // LOG_TYPE_PARSER

    "OTHER",	//LOG_OTHER

    "***",		//CT_LOG_TYPE_LAST
};

static const char* const g_c_log_level_str[] =
{
    "NOLOG",	// CT_LOG_LEVEL_NOLOG
    "INFO",	// CT_LOG_LEVEL_INFO
    "WARN",	//CT_LOG_LEVEL_WARN
    "ERROR",	// CT_LOG_LEVEL_ERR
};

LOG_LEVEL_EN	 g_log_level			= LOG_LEVEL_ERR;
FILE		*g_log_file			= NULL;
char		 g_file_name[FILE_NAME_LEN]     = "";



void set_log_level(const char* level)
{
    LOG_LEVEL_EN log_level;

    if (NULL == level)
    {
    	return;
	}

    if (strncmp(level, "nolog", 5) == 0)
    {
    	log_level = LOG_LEVEL_NOLOG;
    }
    else if (strncmp(level, "info", 4) == 0)
    {
   		log_level = LOG_LEVEL_INFO;
    }
    else if (strncmp(level, "warn", 4) == 0)
    {
    	log_level = LOG_LEVEL_WARN;
    }
	else if (strncmp(level, "err", 3) == 0)
	{
		log_level = LOG_LEVEL_ERR;
	}
    else
    {
    	log_level = LOG_LEVEL_ERR;  /* default: LOG_LEVEL_ERR */
    }

    g_log_level = log_level;
}

void set_log_file_info(const char* path)
{
    if (NULL == path)
    {
        if (g_log_file)
        {
        	fclose(g_log_file);
        }
        g_log_file = NULL;

		return;
    }

	if (!(g_log_file = fopen(path, "a+")))
	{
  		LOG_ERR(LOG_TYPE_OTHER, "log_manager: Cannot open file %s.", path);

		return;
	}

    strcpy(g_file_name, path);
}

void print_local_time(FILE* fp)
{
    int			ret;
    char		buf[256];
    time_t		clock;
    struct tm  *tm;

	memset(buf, 0, sizeof(buf));

    if (NULL == fp)
    {
    	return;
    }

    time(&clock);
    tm = localtime(&clock);
    ret = strftime(buf, TIME_BUF_LEN, "%Y/%m/%d %H:%M:%S", tm);

    if (0 == ret)
    {
        LOG_ERR(LOG_TYPE_OTHER, "log_manager: Print local time unsuccessful");
    }

    fprintf (fp, "%s ", buf);
}

void create_log(LOG_LEVEL_EN level, LOG_TYPE_EN type, const char* format, va_list args)
{
	FILE* fp = g_log_file;

	// if the level of the log to be printed is stricter than the set one, the manager won't print it
	if (level > g_log_level)
	{
		return;
	}

    // Output to file
	if(fp)
	{
        print_local_time(fp);
        fprintf(fp, "%s: [%s] ", g_c_log_level_str[level], g_c_log_type_str[type]);
        vfprintf(fp, format, args);
        fprintf(fp, "\n");
        fflush(fp);
	}

	// Output to console
	print_local_time(stdout);
	fprintf(stdout, "%s: [%s] ", g_c_log_level_str[level], g_c_log_type_str[type]);
	vfprintf(stdout, format, args);
	fprintf(stdout, "\n");
	fflush(stdout);
}

void LOG_INFO(LOG_TYPE_EN type, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    create_log(LOG_LEVEL_INFO, type, format, args);

    va_end(args);
}

void LOG_WARN(LOG_TYPE_EN type, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    create_log(LOG_LEVEL_WARN, type, format, args);

    va_end(args);
}

void LOG_ERR(LOG_TYPE_EN type, const char* format, ...)
{
    va_list args;
    va_start(args, format);

    create_log(LOG_LEVEL_ERR, type, format, args);

    va_end(args);
}

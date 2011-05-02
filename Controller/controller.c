/* 
 * File:   controller.c
 * Author: songhan
 *
 * Created on May 1, 2011, 10:48 PM
 */

#include "controller.h"

#define MAX_FILE_NAME_LEN       36

#define DBM_PRE_COMMAND_DBM         "./dbm "
#define DBM_PRE_COMMAND_DB          " -d ./conn_db.conf "
#define DBM_PRE_COMMAND_DISH_FILE   " -c "
#define DBM_PRE_COMMAND_LOG_PRE     " -o ./dbm_logs/"

#define CLST_PRE_COMMAND_CLST        "./clustering "
#define CLST_PRE_COMMAND_DB          " -d ./conn_db.conf "
#define CLST_PRE_COMMAND_DISH_FILE   " -c "
#define CLST_PRE_COMMAND_LOG_PRE     " -o ./clst_logs/"


// global variables
char g_parser_req_path[MAX_SHORT_FIELD_SIZE] = "";
int  g_file_num = 0;
char **g_call_dbm = NULL;
char **g_call_clustering = NULL;


//read config infos from config file
int read_config_file(char *configfile)
{
    int true = 1, false = 0;
    char *filename = configfile;
    unsigned char tempchar;
    int index;
    char match_word[32], config_info[MAX_SHORT_FIELD_SIZE];
    FILE *fp;

    memset(match_word, 0, sizeof(match_word));
    memset(config_info, 0, sizeof(config_info));

    if (!(fp = fopen(filename, "r")))
    {
        LOG_ERR(LOG_TYPE_CONTROLLER, "Can not open config file: %s\n", filename);
        return false;
    }

    while(fread(&tempchar, sizeof(tempchar), 1, fp) == 1)// to read one character each time while reading a line
    {
        index = 0;

        if ((tempchar == '\r') || (tempchar == '\n') || (tempchar == '\t') || (tempchar == ' ')) // if the first character is '\n' or '\r' or '\t' or ' ', then skip them
            continue;
        match_word[index++] = tempchar; // if not, then store this character into match_word[]
        while(fread(&tempchar, sizeof(tempchar), 1, fp) == 1)
        {
            if ((tempchar == ':'))// read ':' means the end of the match word
                break;
            match_word[index++]  = tempchar;
        }
        index = 0;
	while(fread(&tempchar, sizeof(tempchar), 1, fp) == 1)//record the following config info
        {
            if ((tempchar == '\t') || (tempchar == ' '))
	        continue;
            if ((tempchar == '\r') || (tempchar == '\n'))
                break;
            config_info[index++] = tempchar;
        }
        memset(g_parser_req_path, 0, sizeof(g_parser_req_path));
        strcpy(g_parser_req_path, config_info);
        break;
    }

    return true;
}

int read_parser_req(char *paser_req_path)
{
    int true = 1, false = 0;
    unsigned char tempchar;
    int index;
    char req_info[8];
    FILE *fp;

    memset(req_info, 0, sizeof(req_info));

    if (!(fp = fopen(paser_req_path, "r")))
    {
        LOG_ERR(LOG_TYPE_CONTROLLER, "Can not open parser.req file: %s\n", paser_req_path);
        return false;
    }

    index = 0;
    while(fread(&tempchar, sizeof(tempchar), 1, fp) == 1)
    {
        if ((tempchar == '\t') || (tempchar == ' '))
        {
            continue;
        }
        if ((tempchar == '\r') || (tempchar == '\n'))
        {
            break;
        }
        req_info[index++] = tempchar;
    }
    g_file_num = atoi(req_info);

    return true;
}



void construct_commands()
{
    int index;
    char dish_file_name[MAX_FILE_NAME_LEN];

    g_call_dbm = (char **)malloc(g_file_num * sizeof(char *));
    memset(g_call_dbm, 0, sizeof(g_call_dbm));
    g_call_clustering = (char **)malloc(g_file_num * sizeof(char *));
    memset(g_call_clustering, 0, sizeof(g_call_clustering));
    for (index=0; index<g_file_num; index++)
    {
        g_call_dbm[index] = (char *)malloc(MAX_SHORT_FIELD_SIZE * sizeof(char));
        memset(g_call_dbm[index], 0, sizeof(g_call_dbm[index]));
        g_call_clustering[index] = (char *)malloc(MAX_SHORT_FIELD_SIZE * sizeof(char));
        memset(g_call_clustering[index], 0, sizeof(g_call_clustering[index]));
    }

   
    for (index=0; index<g_file_num; index++)
    {
        memset(dish_file_name, 0, sizeof(dish_file_name));
        sprintf(dish_file_name, "%d", index);
        strcat(dish_file_name, "-1.out ");

        strcpy(g_call_dbm[index], DBM_PRE_COMMAND_DBM);
        strcat(g_call_dbm[index], DBM_PRE_COMMAND_DB);
        strcat(g_call_dbm[index], DBM_PRE_COMMAND_DISH_FILE);
        strcat(g_call_dbm[index], dish_file_name);
        strcat(g_call_dbm[index], DBM_PRE_COMMAND_LOG_PRE);

        memset(dish_file_name, 0, sizeof(dish_file_name));
        sprintf(dish_file_name, "%d", index);
        strcat(dish_file_name, "-1.log ");
        strcat(g_call_dbm[index], dish_file_name);
        printf("command: %s\n", g_call_dbm[index]);
    }

    for (index=0; index<g_file_num; index++)
    {
        memset(dish_file_name, 0, sizeof(dish_file_name));
        sprintf(dish_file_name, "%d", index);
        strcat(dish_file_name, "-2.out ");

        strcpy(g_call_clustering[index], CLST_PRE_COMMAND_CLST);
        strcat(g_call_clustering[index], CLST_PRE_COMMAND_DB);
        strcat(g_call_clustering[index], CLST_PRE_COMMAND_DISH_FILE);
        strcat(g_call_clustering[index], dish_file_name);
        strcat(g_call_clustering[index], CLST_PRE_COMMAND_LOG_PRE);

        memset(dish_file_name, 0, sizeof(dish_file_name));
        sprintf(dish_file_name, "%d", index);
        strcat(dish_file_name, "-2.log ");
        strcat(g_call_clustering[index], dish_file_name);
        printf("command: %s\n", g_call_clustering[index]);
    }
}

int execute_commands()
{
    int false = 0, true = 1;
    int index;
    int system_return_value;

    for (index=0; index<g_file_num; index++)
    {
        system_return_value = system(g_call_dbm[index]);
        if(system_return_value == -1)
        {
            LOG_ERR(LOG_TYPE_CONTROLLER, "Command: %s execute error!!\n", g_call_dbm[index]);
            LOG_ERR( LOG_TYPE_CONTROLLER, "Return_value from system() is %d\n", system_return_value);
            return false;
        }
    }

    for (index=0; index<g_file_num; index++)
    {
        system_return_value = system(g_call_clustering[index]);
        if(system_return_value == -1)
        {
            LOG_ERR(LOG_TYPE_CONTROLLER, "Command: %s execute error!!\n", g_call_clustering[index]);
            LOG_ERR( LOG_TYPE_CONTROLLER, "Return_value from system() is %d\n", system_return_value);
            return false;
        }
    }

    return true;
}

void usage(const char* program)
{
    printf("usage: %s [options]\n\n", program);
    printf("Opion list:\n");
    printf("-h,-?                           print this help\n");
    printf("-o output file                  writelogging output into file\n");
    printf("-l log level                    enable given list of log levels\n");
    printf("-c config file                  read configure infos\n");
    printf("-v                              display program version information\n");
    printf("For log levels, choose from:\n");
    printf("nolog, err, warn, info\n\n");
}


void version(char* progname)
{
    printf("%s version %s\n", progname, STR_VERSION);
    printf("Copyright 2008-2009, CertusNet Inc.\n");
}


int main(int argc, char** argv)
{
    char *loglevel = "err", *config_file = NULL;
    char *logfile = "log_controller.txt";
    char *progname, *p;

    progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);

    for (;;)
    {
        int option = getopt(argc, argv, "?hl:o:c:v");
        if (option == -1)
        {
            break;
        }

        switch(option)
	{
            case 'c':
                config_file = optarg;
                break;
            case 'l':
                loglevel = ((optarg != NULL) ? optarg : loglevel);
                break;
            case 'o':
                logfile = ((optarg != NULL) ? optarg : logfile);
                break;
            case 'v':
                version(progname);
                exit(0);
            default:
                usage(progname);
		exit(0);
        }
    }

    set_log_level(loglevel);
    set_log_file_info(logfile);

    if (read_config_file(config_file))
    {
        LOG_INFO(LOG_TYPE_CONTROLLER, "Read Config File OK.\n");
        LOG_INFO(LOG_TYPE_CONTROLLER, "The path is: %s\n", g_parser_req_path);
        if (read_parser_req(g_parser_req_path))
        {
            LOG_INFO(LOG_TYPE_CONTROLLER, "Read parser.req OK.\n");
            LOG_INFO(LOG_TYPE_CONTROLLER, "The number is %d\n", g_file_num);
            construct_commands();
            printf("system_return: %d\n", system("./hello"));
            if (0 == execute_commands())
            {
                LOG_ERR(LOG_TYPE_CONTROLLER, "Execute commands ERROR!\n");
            }
            else
            {
                LOG_INFO(LOG_TYPE_CONTROLLER, "Execution Done!\n");
            }
            
        }
        else
        {
            LOG_ERR(LOG_TYPE_CONTROLLER, "Read parser.req ERROR!\n");
        }

    }
    else
    {
        LOG_ERR(LOG_TYPE_CONTROLLER, "Read Config File ERROR!\n");
    }

    return (EXIT_SUCCESS);
}


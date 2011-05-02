/* 
 * File:   clustering.c
 * Author: songhan
 *
 * Created on May 1 2011, 04:00am
 */

#include "clustering.h"

#define DEBUG



// global variables
PGconn      *g_conn                                    = NULL;
PGresult    *g_PGres                                   = NULL;
char        **g_link_dish_names                        = NULL;
int         g_link_num                                 = 0;
char        g_master_dish[MAX_SHORT_FIELD_SIZE]         = "";



int init_connect_database(char *connect_file)
 {
    enum RETURN_VALUE return_value = FAILED;
    FILE *fp;
    char read_buf[MAX_CONN_INFO_SIZE] = "";

    memset(read_buf, 0, sizeof(read_buf));

    if (NULL == connect_file)
    {
        LOG_INFO(LOG_TYPE_CLUSTERING, "There is no database connection file\n");
        return return_value;
    }
    else
    {
        LOG_INFO(LOG_TYPE_CLUSTERING, "There is a database connection file\n");
        fp = fopen(connect_file, "r");
        if (NULL == fp)
        {
            LOG_ERR(LOG_TYPE_CLUSTERING, "Can not open the configure file\n");
            return return_value;
        }
	else
	{
	    fread(read_buf, MAX_CONN_INFO_SIZE-1, 1, fp);
	    fclose(fp);
	}
    }

    g_conn = PQconnectdb(read_buf);
    if (PQstatus(g_conn) != CONNECTION_OK)
    {
        LOG_ERR(LOG_TYPE_CLUSTERING, "Connection to database failed: %s \n", PQerrorMessage(g_conn));
    }
    else
    {
        LOG_INFO(LOG_TYPE_CLUSTERING, "Connect OK!\n");
        return_value = SUCCESS;
    }

    return return_value;
}

 //to check the status of execution database command
static int command_status(PGresult *res, PGconn *conn)
{
    enum RETURN_VALUE return_value = FAILED;

    if (PQresultStatus(res) != PGRES_COMMAND_OK && PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        LOG_ERR(LOG_TYPE_CLUSTERING, "Command failed: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return return_value;
    }
    return_value = SUCCESS;

    return return_value;
}

//to construct a string with variables used by executing database
int safe_snprintf(char* buf, size_t sizeofbuf, char* format, ...)
{
    va_list va_ap;
    int rc;

    va_start (va_ap, format);
    rc = vsnprintf(buf, sizeofbuf, format, va_ap);
    va_end (va_ap);

    return rc;
}

//to execute the command of database
int operate_database(char *sql)
{
    g_PGres = PQexec(g_conn, sql);

    return command_status(g_PGres, g_conn);
}


int abstract_link_page_info(char *dish_file)
{
    enum RETURN_VALUE return_value = FAILED;
    FILE *fp;
    char tmp_buf[MAX_BUF_SIZE] = "";
    char *strtok_state;
    char *link_num, *dish_name;
    int index;

    memset(tmp_buf, 0, sizeof(tmp_buf));

    if(NULL == dish_file)
    {
        LOG_INFO(LOG_TYPE_CLUSTERING, "There is no dish file\n");
        return return_value;
    }
    else
    {
        LOG_INFO(LOG_TYPE_CLUSTERING, "There is a dish file\n");
        fp = fopen(dish_file, "r");
        if(NULL == fp)
        {
            LOG_ERR(LOG_TYPE_CLUSTERING, "Can not open the dish file\n");
            return return_value;
        }
        else
        {
            fread(tmp_buf, MAX_BUF_SIZE-1, 1, fp);
	    fclose(fp);

            // the first is the master dish
            dish_name = strtok_r(tmp_buf, "|", &strtok_state);
	    if (!strcmp(dish_name, "NULL"))
            {
                LOG_ERR(LOG_TYPE_CLUSTERING, "There is no dish name in the dish file.\n");
                return return_value;
            }
            memset(g_master_dish, 0, sizeof(g_master_dish));
            strcpy(g_master_dish, dish_name);
            printf("master_dish: %s\n", g_master_dish);


            // the second is the number of links
	    link_num = strtok_r(NULL, "|", &strtok_state);
	    if (!strcmp(link_num, "NULL"))
            {
                LOG_ERR(LOG_TYPE_CLUSTERING, "There is no number of dishes in the dish file.\n");
                return return_value;
            }
            g_link_num = atoi(link_num);
            printf("link_num: %d\n", g_link_num);

            // followings are the linked dish names
            g_link_dish_names = (char **)malloc(g_link_num * sizeof(char *));
            memset(g_link_dish_names, 0, sizeof(g_link_dish_names));
            for (index=0; index<g_link_num; index++)
            {
                g_link_dish_names[index] = (char *)malloc(MAX_SHORT_FIELD_SIZE * sizeof(char));
                memset(g_link_dish_names[index], 0, sizeof(g_link_dish_names[index]));
            }

            for (index=0; index<g_link_num; index++)
            {
                dish_name = strtok_r(NULL, "|", &strtok_state);
                if (!strcmp(dish_name, "NULL"))
                {
                    LOG_ERR(LOG_TYPE_CLUSTERING, "There is no linked dish name in the dish file.\n");
                    return return_value;
                }
                else
                {
                    strcpy(g_link_dish_names[index], dish_name);
                    printf("g_link_dish_names[%d]: %s\n", index, g_link_dish_names[index]);
                }
            }
        }
    }
    
    return_value = SUCCESS;
    return return_value;
}

int process_link_pages(char **dish_names, int link_num)
{
    int true = 1, false = 0;
    int index;
    char sql[MAX_MID_FIELD_SIZE];
    int rc;
    int pseudo_ids[link_num];
    char pseudo_id_list[link_num+1];

    memset(pseudo_ids, 0, sizeof(pseudo_ids));
    memset(pseudo_id_list, 0, sizeof(pseudo_id_list));
    
    if(operate_database("BEGIN"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_CLUSTERING, "BEGIN OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }
    
    for (index=0; index<link_num; index++)
    {
        memset(sql, 0, sizeof(sql));
        rc = safe_snprintf(sql, sizeof(sql),
             "SELECT pseudo_dish_id FROM dish_info WHERE dish_name = '%s'", dish_names[index]);
        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_CLUSTERING, "SELECT OK!\n");
                #endif
                pseudo_ids[index] = atoi(PQgetvalue(g_PGres, 0, 0));
                PQclear(g_PGres);
            }
            else
            {
                LOG_INFO(LOG_TYPE_CLUSTERING, "SELECT ERROR, There is no such dish name in db: %s!\n", dish_names[index]);
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    convert_ids_to_list(pseudo_ids, pseudo_id_list, link_num);
    printf("id_list: %s\n", pseudo_id_list);

    memset(sql, 0, sizeof(sql));
    rc = safe_snprintf(sql, sizeof(sql),
         "UPDATE pseudo_dish SET link_relative_pd_ids = '%s'  WHERE pseudo_dish_id = (SELECT pseudo_dish_id FROM dish_info WHERE dish_name = '%s')", pseudo_id_list, g_master_dish);
    if(rc>0 && rc<=sizeof(sql))
    {
        if(operate_database(sql))
        {
            #ifdef DEBUG
            LOG_INFO(LOG_TYPE_CLUSTERING, "UPDATE OK!\n");
            #endif
            PQclear(g_PGres);
        }
        else
        {
            LOG_INFO(LOG_TYPE_CLUSTERING, "UPDATE ERROR!\n");
            return false;
        }
    }
    else
    {
        return false;
    }

    if(operate_database("END"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_CLUSTERING, "END OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }
    return true;
}


//convert int* to int|int|int
void convert_ids_to_list(int *ids, char *id_list, unsigned short id_num)
{
    unsigned short index;
    int counter = 0;

    for (index=0; index<id_num; index++)
    {
        id_list[counter++] = ids[index] + 0x30;
        if (index < id_num-1)
        {
            id_list[counter++] = '|';
        }
    }
}

//convert int|int|int to int*
void convert_list_to_ids(int *ids, char *id_list, unsigned short id_num)
{
    unsigned short index;
    int counter = 0;

    for (index=0; index<id_num; index++)
    {
        ids[index] = id_list[counter]- 0x30;
        counter += 2;
    }
}

int get_max_id(int *ids, int id_num)
{
    int index;
    int max = ids[0];
    
    for (index=0; index<id_num; index++)
    {
        if (max < ids[index])
        {
            max = ids[index];
        }
    }
    
    return max;
}

void counting_sort_ids(int *ids, int id_num)
{
    int index;
    int max_id = get_max_id(ids, id_num);
    int counting_B[id_num];
    int counting_C[max_id+1];

    printf("id_num is %d\n", id_num);
    printf("max is %d\n", max_id);

    memset(counting_B, 0, id_num * sizeof(int));
    memset(counting_C, 0, (max_id+1)*sizeof(int));

    for (index=0; index<id_num; index++)
    {
        counting_C[ids[index]]++;
    }

    for (index=0; index<max_id; index++)
    {
        counting_C[index+1] += counting_C[index];
    }

    for (index=id_num-1; index>=0; index--)
    {
        counting_B[counting_C[ids[index]]-1] = ids[index];
        counting_C[ids[index]]--;
    }


    for (index=0; index<id_num; index++)
    {
        ids[index] = counting_B[index];
    }
}

int get_master_dish_info(int *master_pseudo_dish_id, char *master_standard_material_list)
{
    int true = 1, false = 0;
    char sql[MAX_MID_FIELD_SIZE];
    int rc;

    memset(sql, 0, sizeof(sql));
    rc = safe_snprintf(sql, sizeof(sql),
         "SELECT b.pseudo_dish_id, b.standard_material_list\
          FROM (dish_info as a JOIN pseudo_dish as b ON a.pseudo_dish_id = b.pseudo_dish_id)\
          WHERE a.dish_name = '%s'", g_master_dish);
    if(rc>0 && rc<=sizeof(sql))
    {
        if(operate_database(sql))
        {
            #ifdef DEBUG
            LOG_INFO(LOG_TYPE_CLUSTERING, "SELECT OK!\n");
            #endif
            *master_pseudo_dish_id = atoi(PQgetvalue(g_PGres, 0, 0));
            strcpy(master_standard_material_list, PQgetvalue(g_PGres, 0, 1));
            PQclear(g_PGres);
        }
        else
        {
            LOG_INFO(LOG_TYPE_CLUSTERING, "SELECT ERROR!\n");
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

int is_similar(int *master_ids, int *other_ids, int master_material_num, int other_material_num, int similar_rate)
{
    int true = 1, false = 0;
    int index;
    double rate = (double)similar_rate/100.0;
    int master_ptr = 0, other_ptr = 0;
    int same_num = 0;


    printf("rate: %f\n", rate);

    printf("master: ");
    for(index=0; index<master_material_num; index++)
        printf("%d ", master_ids[index]);
    printf("\n");

    printf("other: ");
    for(index=0; index<other_material_num; index++)
        printf("%d ", other_ids[index]);
    printf("\n");

    if ((double)other_material_num/(double)master_material_num < rate)
    {
        return false;
    }
    else
    {
        if ((master_ids[master_material_num-1] < other_ids[0])
           ||(master_ids[0]>other_ids[other_material_num-1]))
        {
            return false;
        }
        else
        {
            while (1)
            {
                if ((double)same_num/(double)master_material_num >= rate)
                {
                    return true;
                }
                else if ((double)(same_num+master_material_num-master_ptr)/(double)master_material_num < rate)
                {
                    return false;
                }
                else if (other_ptr == other_material_num)
                {
                    return false;
                }
                else
                {
                    if (master_ptr != master_material_num) // the condition of "==" will not happen here
                    {
                        if (master_ids[master_ptr] == other_ids[other_ptr])
                        {
                            same_num++;
                            master_ptr++;
                            other_ptr++;
                        }
                        else if (master_ids[master_ptr] > other_ids[other_ptr])
                        {
                            other_ptr++;
                        }
                        else if (master_ids[master_ptr] < other_ids[other_ptr])
                        {
                            master_ptr++;
                        }
                    }
                }
            } // end of while (1)
        }
    }
}

int similar_dish_clustering(int similar_rate)
{
    int true = 1, false = 0;
    int record_num, index, k;
    char sql[MAX_MID_FIELD_SIZE];
    int rc;
    int master_pseudo_dish_id;
    char *other_pseudo_dish_id;
    char master_standard_material_list[MAX_SHORT_FIELD_SIZE];
    char **standard_material_list = NULL; // others'
    int *master_ids, master_material_num, *other_ids, other_material_num;
    char *similar_dishes;
    char append_content[MAX_MATERIAL_NUM];


    memset(master_standard_material_list, 0, sizeof(master_standard_material_list));

    if(operate_database("BEGIN"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_CLUSTERING, "BEGIN OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    if (get_master_dish_info(&master_pseudo_dish_id, master_standard_material_list))
    {
        printf("master_pseudo_dish_id = %d\n", master_pseudo_dish_id);
        printf("list is: %s, its length is: %d\n", master_standard_material_list, strlen(master_standard_material_list));

        memset(sql, 0, sizeof(sql));
        rc = safe_snprintf(sql, sizeof(sql),
             "SELECT pseudo_dish_id, standard_material_list\
              FROM pseudo_dish\
              WHERE pseudo_dish_id != %d\
              ORDER BY pseudo_dish_id", master_pseudo_dish_id);
        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_CLUSTERING, "SELECT OK!\n");
                #endif
                record_num = PQntuples(g_PGres);
                other_pseudo_dish_id = (char *)malloc(record_num*sizeof(char));
                memset(other_pseudo_dish_id, 0, sizeof(other_pseudo_dish_id));
                standard_material_list = (char **)malloc(record_num*sizeof(char *));
                memset(standard_material_list, 0, sizeof(standard_material_list));
                for (index=0; index<record_num; index++)
                {
                    standard_material_list[index] = (char *)malloc(MAX_SHORT_FIELD_SIZE * sizeof(char));
                    memset(standard_material_list[index], 0, sizeof(standard_material_list[index]));
                    strcpy(standard_material_list[index], PQgetvalue(g_PGres, index, 1));
                    other_pseudo_dish_id[index] = atoi(PQgetvalue(g_PGres, index, 0))+0x30;
                }
                PQclear(g_PGres);
                #ifdef DEBUG
                for(index=0; index!=record_num; index++)
                    printf("other_pseudo_dish_id[%d] = %c\n", index, other_pseudo_dish_id[index]);
                for(index=0; index!=record_num; index++)
                    printf("standard_material_list[%d] = %s\n", index, standard_material_list[index]);
                LOG_INFO(LOG_TYPE_CLUSTERING, "Get standard_material_list Successfully!\n");
                #endif
            }
            else
            {
                LOG_INFO(LOG_TYPE_CLUSTERING, "SELECT ERROR!\n");
                return false;
            }
        }
        else
        {
            return false;
        }

        master_material_num = strlen(master_standard_material_list)/2+1;
        master_ids = (int *)malloc(master_material_num * sizeof(int));
        convert_list_to_ids(master_ids, master_standard_material_list, master_material_num);
        counting_sort_ids(master_ids, master_material_num);
        #ifdef DEBUG
        for(index=0; index!=master_material_num; index++)
            printf("master_ids[%d] = %d\n", index, master_ids[index]);
        #endif

        similar_dishes = (char *)malloc(record_num * sizeof(char));
        memset(similar_dishes, 0, sizeof(similar_dishes));


        for (index=0; index<record_num; index++)
        {
            printf("processing: %s:\n", standard_material_list[index]);
            other_material_num = strlen(standard_material_list[index])/2+1;
            other_ids = (int *)malloc(other_material_num * sizeof(int));
            convert_list_to_ids(other_ids, standard_material_list[index], other_material_num);
            counting_sort_ids(other_ids, other_material_num);
            #ifdef DEBUG
            for(k=0; k!=other_material_num; k++)
                printf("other_ids[%d] = %d\n", k, other_ids[k]);
            #endif

            if (is_similar(master_ids, other_ids, master_material_num, other_material_num, similar_rate))
            {
                if (0 == strlen(similar_dishes))
                {
                    similar_dishes[0] = other_pseudo_dish_id[index];
                }
                else
                {
                    memset(append_content, 0, sizeof(append_content));
                    rc = safe_snprintf(append_content, sizeof(append_content),"|%c", other_pseudo_dish_id[index]);
                    similar_dishes = strcat(similar_dishes, append_content);
                }
            }
            printf("similar_dishes = %s\n", similar_dishes);
        }

        if (strlen(similar_dishes) != 0)
        {
            memset(sql, 0, sizeof(sql));
            rc = safe_snprintf(sql, sizeof(sql), "UPDATE pseudo_dish SET similar_dish_ids = '%s' WHERE pseudo_dish_id = %d", similar_dishes, master_pseudo_dish_id);
        }
        else
        {
            memset(sql, 0, sizeof(sql));
            rc = safe_snprintf(sql, sizeof(sql), "UPDATE pseudo_dish SET similar_dish_ids = 0 WHERE pseudo_dish_id = %d", master_pseudo_dish_id);
        }

        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_CLUSTERING, "UPDATE OK!\n");
                #endif
                PQclear(g_PGres);
            }
            else
            {
                LOG_ERR(LOG_TYPE_CLUSTERING, "UPDATE ERROR!\n");
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        LOG_ERR(LOG_TYPE_CLUSTERING, "Get Master dish info ERROR!!!\n");
        if(operate_database("END"))
        {
            #ifdef DEBUG
            LOG_INFO(LOG_TYPE_CLUSTERING, "END OK!\n");
            #endif
            PQclear(g_PGres);
        }
        else
        {
            return false;
        }
        return false;
    }

    if(operate_database("END"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_CLUSTERING, "END OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    return true;
}


void usage(const char* program)
{
    printf("usage: %s [options]\n\n", program);
    printf("Opion list:\n");
    printf("-c config file                  read the path of the dish file\n");
    printf("-d db conn file                 read db connection infos from the file\n");
    printf("-h,-?                           print this help\n");
    printf("-o output file                  write logging output into file\n");
    printf("-l log level                    enable given list of log levels\n");
    printf("-v                              display program version information\n");
    printf("For log levels, choose from:\n");
    printf("  nolog, err, warn, info\n\n");
}


void version(char* progname)
{
    printf("%s version %s\n", progname, STR_VERSION);
    printf("Copyright 2010-2011, Courant Institute NYU.\n");
}

int main(int argc, char** argv)
{
    char *loglevel = "err", *dish_file = NULL, *db_connect_file_path = NULL;
    char *logfile = "log_clustering.txt";
    char *progname, *p;

    progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);

    for (;;)
    {
        int option = getopt(argc, argv, "?hl:o:c:d:v");
        if (option == -1)
        {
            break;
        }

        switch(option)
	{
            case 'c':
                dish_file = optarg;
                break;
            case 'd':
                db_connect_file_path = optarg;
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

    printf("level: %s   file: %s\n", loglevel, logfile);
    set_log_level(loglevel);
    set_log_file_info(logfile);

    while (FAILED == init_connect_database(db_connect_file_path))
    {
        LOG_ERR(LOG_TYPE_CLUSTERING, "Login database ERROR!\n");
        sleep(10);
        continue;
    }

    if (NULL == dish_file)
    {
        LOG_ERR(LOG_TYPE_CLUSTERING, "NO Dish File ...\n");
    }
    else
    {
        if (abstract_link_page_info(dish_file))
        {
            LOG_INFO(LOG_TYPE_CLUSTERING, "Abstract link pages OK!\n");

            if (process_link_pages(g_link_dish_names, g_link_num))
            {
                LOG_INFO(LOG_TYPE_CLUSTERING, "Process Link Pages OK!\n");
            }
            else
            {
                LOG_ERR(LOG_TYPE_CLUSTERING, "Process Link Pages ERROR!\n");
            }

            if (similar_dish_clustering(SIMILAR_RATE))
            {
                LOG_INFO(LOG_TYPE_CLUSTERING, "Clustering OK!\n");
            }
            else
            {
                 LOG_ERR(LOG_TYPE_CLUSTERING, "Clustering ERROR!\n");
            }
        }
        else
        {
            LOG_ERR(LOG_TYPE_CLUSTERING, "Abstract link pages ERROR!\n");
        }
    }

    PQfinish(g_conn);

    return (EXIT_SUCCESS);
}


/* 
 * File:   main.c
 * Author: songhan
 *
 * Created on April 28, 2011, 2:09 PM
 */

#include "dbm.h"

#define DEBUG

#define STR_VERSION		"1.0"
#define MAX_CONN_INFO_SIZE      128
#define MAX_SHORT_FIELD_SIZE    256
#define MAX_MID_FIELD_SIZE      1024
#define MAX_LONG_FIELD_SIZE     2048
#define MAX_BUF_SIZE            4096
#define MAX_MATERIAL_NUM        64
#define FAKE_ID                 0

// global variables
char        *dish_file                                 = NULL;
PGconn      *g_conn                                    = NULL;
PGresult    *g_PGres                                   = NULL;
char        g_dish_name[MAX_SHORT_FIELD_SIZE]          = "";
char        g_material_list[MAX_LONG_FIELD_SIZE]       = "";
char        g_URL[MAX_SHORT_FIELD_SIZE]                = "";
char        g_title[MAX_SHORT_FIELD_SIZE]              = "";
char        g_link_relative_list[MAX_LONG_FIELD_SIZE]  = "";
char        g_snatch[MAX_LONG_FIELD_SIZE]              = "";
char        g_material_id_list[MAX_LONG_FIELD_SIZE]    = "";
int         g_dish_id                                  = 0;
int         g_recipe_id                                = 0;
int         g_url_id                                   = 0;
int         g_pseudo_id                                = 0;


 int init_connect_database(char *connect_file)
 {
    enum RETURN_VALUE return_value = FAILED;
    FILE *fp;
    char read_buf[MAX_CONN_INFO_SIZE] = "";
    
    memset(read_buf, 0, sizeof(read_buf));

    if (NULL == connect_file)
    {
        LOG_INFO(LOG_TYPE_DBM, "There is no database connection file\n");
        return return_value;
    }
    else
    {
        LOG_INFO(LOG_TYPE_DBM, "There is a database connection file\n");
        fp = fopen(connect_file, "r");
        if (NULL == fp)
        {
            LOG_ERR(LOG_TYPE_DBM, "Can not open the configure file\n");
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
        LOG_ERR(LOG_TYPE_DBM, "Connection to database failed: %s \n", PQerrorMessage(g_conn));
    }
    else
    {
        LOG_INFO(LOG_TYPE_DBM, "Connect OK!\n");
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
        LOG_ERR(LOG_TYPE_DBM, "Command failed: %s\n", PQerrorMessage(conn));
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


int abstract_classifed_info_from(char *dish_file)
{
    enum RETURN_VALUE return_value = FAILED;
    FILE *fp;
    char tmp_buf[MAX_BUF_SIZE] = "";
    char *strtok_state;
    char *dish_name, *material_list, *URL, *title, *link_relative_list, *snatch;

    memset(tmp_buf, 0, sizeof(tmp_buf));
    memset(g_dish_name, 0, sizeof(g_dish_name));
    memset(g_material_list, 0, sizeof(g_material_list));
    memset(g_URL, 0, sizeof(g_URL));
    memset(g_title, 0, sizeof(g_title));
    memset(g_link_relative_list, 0, sizeof(g_link_relative_list));
    memset(g_snatch, 0, sizeof(g_snatch));

    if(NULL == dish_file)
    {
        LOG_INFO(LOG_TYPE_DBM, "There is no dish file\n");
        return return_value;
    }
    else
    {
        LOG_INFO(LOG_TYPE_DBM, "There is a dish file\n");
        fp = fopen(dish_file, "r");
        if(NULL == fp)
        {
            LOG_ERR(LOG_TYPE_DBM, "Can not open the dish file\n");
            return return_value;
        }
        else
	{
	    fread(tmp_buf, MAX_BUF_SIZE-1, 1, fp);
	    fclose(fp);

	    dish_name = strtok_r(tmp_buf, "|", &strtok_state);
	    if (!strcmp(g_dish_name, "NULL"))
            {
                LOG_ERR(LOG_TYPE_DBM, "There is no dish name in the dish file.\n");
                return return_value;
            }
            strcpy(g_dish_name, dish_name);
            printf("dish_name: %s\n", g_dish_name);

	    material_list = strtok_r(NULL, "|", &strtok_state);
	    if (!strcmp(g_material_list, "NULL"))
	    {
                LOG_ERR(LOG_TYPE_DBM, "There is no material list in the dish file.\n");
                return return_value;
            }
            else
            {
                strcpy(g_material_list, material_list);
                printf("g_material_list: %s\n", g_material_list);
                //if (process_material_list(g_material_list))
                //{}
            }
            

	    URL = strtok_r(NULL, "|", &strtok_state);
	    if (!strcmp(g_URL, "NULL"))
	    {
                LOG_ERR(LOG_TYPE_DBM, "There is no URL in the dish file.\n");
                return return_value;
            }
            strcpy(g_URL, URL);
            printf("g_URL: %s\n", g_URL);

	    title = strtok_r(NULL, "|", &strtok_state);
	    if (!strcmp(g_title, "NULL"))
	    {
                LOG_ERR(LOG_TYPE_DBM, "There is no title in the dish file.\n");
                return return_value;
            }
            strcpy(g_title, title);
            printf("g_title: %s\n", g_title);

	    link_relative_list = strtok_r(NULL, "|", &strtok_state);
	    if (!strcmp(g_link_relative_list, "NULL"))
	    {
                LOG_ERR(LOG_TYPE_DBM, "There is no link relative list in the dish file.\n");
                return return_value;
            }
            strcpy(g_link_relative_list, link_relative_list);
            printf("g_link_relative_list: %s\n", g_link_relative_list);

            snatch = strtok_r(NULL, "|", &strtok_state);
	    if (!strcmp(g_snatch, "NULL"))
	    {
                LOG_ERR(LOG_TYPE_DBM, "There is no snatch in the dish file.\n");
                return return_value;
            }
            strcpy(g_snatch, snatch);
            printf("g_snatch: %s\n", g_snatch);
	}
    }
}


int process_psuedo_dish(char *g_material_id_list)
{
    int false = 0, true = 1;
    unsigned short int index;
    char sql[MAX_MID_FIELD_SIZE];
    int rc;
    int record_num;
    int insert_flag = true;
    char *dish_ids = NULL;
    char append_content[MAX_MATERIAL_NUM];

    memset(append_content, 0, sizeof(append_content));
    rc = safe_snprintf(append_content, sizeof(append_content),"|%c", g_dish_id + 0x30);

    if(operate_database("BEGIN"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "BEGIN OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    if(operate_database("SELECT standard_material_list FROM pseudo_dish ORDER BY pseudo_dish_id"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "SELECT OK!\n");
        #endif
        record_num = PQntuples(g_PGres);
        for(index=0; index<record_num; index++)
        {
            if (!strcmp(g_material_id_list, PQgetvalue(g_PGres, index, 0)))
            {
                LOG_INFO(LOG_TYPE_DBM, "There is a same pseudo dish in the db!\n");
                insert_flag = false;
                g_pseudo_id = index + 1;
                break;
            }
        }
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    if (true == insert_flag)
    {
        LOG_INFO(LOG_TYPE_DBM, "There is not the same pseudo dish in the db!\n");

        g_pseudo_id = record_num + 1;

        memset(sql, 0, sizeof(sql));
        rc = safe_snprintf(sql, sizeof(sql),
             "INSERT INTO pseudo_dish VALUES ('%d', '%s', '%d', '%d')", g_pseudo_id, g_material_id_list, g_dish_id, FAKE_ID);
        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_DBM, "INSERT OK!\n");
                #endif
                PQclear(g_PGres);
            }
            else
            {
                LOG_INFO(LOG_TYPE_DBM, "INSERT ERROR!\n");
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
        memset(sql, 0, sizeof(sql));
        rc = safe_snprintf(sql, sizeof(sql),
             "SELECT dish_ids FROM pseudo_dish WHERE pseudo_dish_id = %d", g_pseudo_id);
        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_DBM, "SELECT OK!\n");
                #endif
                dish_ids = (char *)malloc((sizeof(PQgetvalue(g_PGres, 0, 0))+2) * sizeof(char));
                memset(dish_ids, 0, sizeof(dish_ids));
                strcpy(dish_ids, PQgetvalue(g_PGres, 0, 0));
                PQclear(g_PGres);
            }
            else
            {
                LOG_ERR(LOG_TYPE_DBM, "SELECT ERROR!\n");
                return false;
            }
        }
        else
        {
            return false;
        }

        if (!is_contained(dish_ids, g_dish_id))
        {
            dish_ids = strcat(dish_ids, append_content);

            memset(sql, 0, sizeof(sql));
            rc = safe_snprintf(sql, sizeof(sql),
                 "UPDATE pseudo_dish SET dish_ids = '%s' WHERE pseudo_dish_id = %d", dish_ids, g_pseudo_id);
            if(rc>0 && rc<=sizeof(sql))
            {
                if(operate_database(sql))
                {
                    #ifdef DEBUG
                    LOG_INFO(LOG_TYPE_DBM, "UPDATE OK!\n");
                    #endif
                    PQclear(g_PGres);
                }
                else
                {
                    LOG_ERR(LOG_TYPE_DBM, "UPDATE ERROR!\n");
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
    }

    // update dish_info 'casue pseudo_id has created
    memset(sql, 0, sizeof(sql));
    rc = safe_snprintf(sql, sizeof(sql),
         "UPDATE dish_info SET pseudo_dish_id = %d WHERE dish_id = %d", g_pseudo_id, g_dish_id);
    if(rc>0 && rc<=sizeof(sql))
    {
        if(operate_database(sql))
        {
            #ifdef DEBUG
            LOG_INFO(LOG_TYPE_DBM, "UPDATE OK!\n");
            #endif
            PQclear(g_PGres);
        }
        else
        {
            LOG_ERR(LOG_TYPE_DBM, "UPDATE ERROR!\n");
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
        LOG_INFO(LOG_TYPE_DBM, "END OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }
    return true;
}

int process_dish_info(char *dish_name)
{
    int false = 0, true = 1;
    unsigned short int index;
    char sql[MAX_MID_FIELD_SIZE];
    int rc;
    int record_num;
    int insert_flag = true;

    if(operate_database("BEGIN"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "BEGIN OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    if(operate_database("SELECT dish_name FROM dish_info ORDER BY dish_id"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "SELECT OK!\n");
        #endif
        record_num = PQntuples(g_PGres);
        printf("record_num = %d\n", record_num);
        for(index=0; index<record_num; index++)
        {
            printf("res[%d] = %s\n", index, PQgetvalue(g_PGres, index, 0));
            if (0 == strcmp(dish_name, PQgetvalue(g_PGres, index, 0)))
            {
                LOG_INFO(LOG_TYPE_DBM, "There is a same dish in the db!\n");
                insert_flag = false;
                g_dish_id = index + 1;
                printf("here: %d\n", g_dish_id);
                break;
            }
        }
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    if (true == insert_flag)
    {
        LOG_INFO(LOG_TYPE_DBM, "There is not the same dish in the db!\n");

        g_dish_id = record_num + 1;

        memset(sql, 0, sizeof(sql));
        rc = safe_snprintf(sql, sizeof(sql),
             "INSERT INTO dish_info VALUES ('%d', '%s', '%d', '%d', '%d')", record_num+1, dish_name, g_recipe_id, g_url_id, FAKE_ID);
        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_DBM, "INSERT OK!\n");
                #endif
                PQclear(g_PGres);
            }
            else
            {
                LOG_INFO(LOG_TYPE_DBM, "INSERT ERROR!\n");
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
        // do nothing
    }

    if(operate_database("END"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "END OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }
    return true;
}


int process_url_info(char *url, char *title, char *snatch)
{
    int false = 0, true = 1;
    unsigned short int index;
    char sql[MAX_MID_FIELD_SIZE];
    int rc;
    int record_num;
    int insert_flag = true;

    if(operate_database("BEGIN"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "BEGIN OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    if(operate_database("SELECT url FROM url_info ORDER BY url_id"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "SELECT OK!\n");
        #endif
        record_num = PQntuples(g_PGres);
        for(index=0; index<record_num; index++)
        {
            if (!strcmp(url, PQgetvalue(g_PGres, index, 0)))
            {
                LOG_INFO(LOG_TYPE_DBM, "There is a same url in the db!\n");
                insert_flag = false;
                g_url_id = index + 1;
                break;
            }
        }
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    if (true == insert_flag)
    {
        LOG_INFO(LOG_TYPE_DBM, "There is not the same url in the db!\n");

        g_url_id = record_num + 1;

        memset(sql, 0, sizeof(sql));
        rc = safe_snprintf(sql, sizeof(sql),
             "INSERT INTO url_info VALUES ('%d', '%s', '%s', '%s')", record_num+1, url, title, snatch);
        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_DBM, "INSERT OK!\n");
                #endif
                PQclear(g_PGres);
            }
            else
            {
                LOG_INFO(LOG_TYPE_DBM, "INSERT ERROR!\n");
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
        // do nothing
    }

    if(operate_database("END"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "END OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }
    return true;
}

int process_material_list(char *material_list)
{
    enum RETURN_VALUE return_value = FAILED;
    char **materials;
    char *tmp_m = NULL;
    unsigned short material_counter = 0;
    char *strtok_state;
    int  material_ids[MAX_SHORT_FIELD_SIZE], index;
    char *material_id_list;

    materials = (char**)malloc(MAX_MATERIAL_NUM * sizeof(char*));
    memset(materials, 0, MAX_MATERIAL_NUM * sizeof(char*));

    if (NULL == material_list)
    {
        LOG_ERR(LOG_TYPE_DBM, "The material list is empty!\n");
        return return_value;
    }
    else
    {
        tmp_m = strtok_r(material_list, "^", &strtok_state);
        while (tmp_m != NULL)
        {
            materials[material_counter] = (char *)malloc(MAX_SHORT_FIELD_SIZE * sizeof(char));
            strcpy(materials[material_counter], tmp_m);
            LOG_INFO(LOG_TYPE_DBM, "Material: %s\n", materials[material_counter]);
            material_counter++;
            tmp_m = NULL;
            tmp_m = strtok_r(NULL, "^", &strtok_state);
        }

        //write these data into database
        // 1. organize these materials by its id
        if (get_material_ids(materials, material_counter, material_ids))
        {
            #ifdef DEBUG
            for(index=0; index!=material_counter; index++)
                printf("material_id_list[%d] = %d\n", index, material_ids[index]);
            LOG_INFO(LOG_TYPE_DBM, "Get Material ID List Successfully!\n");
            #endif

            material_id_list = (char *)malloc(2*material_counter * sizeof(char));
            memset(material_id_list, 0, sizeof(material_id_list));

            convert_ids_to_list(material_ids, material_id_list, material_counter);
            strcpy(g_material_id_list, material_id_list); // this is not good for the whole structure
            // 2. add it into db the table is dish_recipe_info
            if (add_material_list_to_recipe(material_id_list)) // this step means the list has stored in db.
            {
                LOG_INFO(LOG_TYPE_DBM, "Add the material list into recipe_info table Successfully!\n");
                if (update_material_info(material_ids, material_counter))
                {
                    LOG_INFO(LOG_TYPE_DBM, "Update rmaterial_info table Successfully!\n");
                }
                else
                {
                    LOG_ERR(LOG_TYPE_DBM, "Update rmaterial_info table ERROR!!\n");
                }

                //check_and_add_pseudo_dish(material_id_list);
            }
            else
            {
                LOG_ERR(LOG_TYPE_DBM, "Add the material list into recipe_info table ERROR!!\n");
            }



        }





    }
}


int get_material_ids(char **materials, unsigned short material_num, int *ids)
{
    enum RETURN_VALUE return_value = FAILED;
    unsigned short int index;
    char sql[MAX_MID_FIELD_SIZE];
    int rc;

    if(operate_database("BEGIN"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "BEGIN OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return return_value;
    }
    
    for (index=0; index!=material_num; index++)
    {
        memset(sql, 0, sizeof(sql));
        rc = safe_snprintf(sql, sizeof(sql),
             "SELECT material_id FROM material_alias WHERE material_name = '%s'", materials[index]);
        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_DBM, "SELECT OK!\n");
                #endif
                ids[index] = atoi(PQgetvalue(g_PGres, 0, 0));
                PQclear(g_PGres);
            }
            else
            {
                LOG_INFO(LOG_TYPE_DBM, "SELECT ERROR!\n");
                return return_value;
            }
        }
        else
        {
            return return_value;
        }
    }

    if(operate_database("END"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "END OK!\n");
        #endif
        PQclear(g_PGres);
        return_value = SUCCESS;
    }
    else
    {
        return return_value;
    }

    return return_value;
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

int add_material_list_to_recipe(char *list)
{
    int false = 0, true = 1;
    unsigned short int index;
    char sql[MAX_MID_FIELD_SIZE];
    int rc;
    int record_num;
    int insert_flag = true;

    if(operate_database("BEGIN"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "BEGIN OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }
    
    if(operate_database("SELECT material_list FROM dish_recipe_info ORDER BY dish_recipe_id"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "SELECT OK!\n");
        #endif
        record_num = PQntuples(g_PGres);
        for(index=0; index<record_num; index++)
        {
            if (!strcmp(list, PQgetvalue(g_PGres, index, 0)))
            {
                LOG_INFO(LOG_TYPE_DBM, "There is a same material list in the db!\n");
                insert_flag = false;
                g_recipe_id = index + 1;
                break;
            }
        }
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    if (true == insert_flag)
    {
        LOG_INFO(LOG_TYPE_DBM, "There is not the same material list in the db!\n");

        g_recipe_id = record_num + 1;

        memset(sql, 0, sizeof(sql));
        rc = safe_snprintf(sql, sizeof(sql),
             "INSERT INTO dish_recipe_info VALUES ('%d', '%s')", record_num+1, list);
        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_DBM, "INSERT OK!\n");
                #endif
                PQclear(g_PGres);
            }
            else
            {
                LOG_INFO(LOG_TYPE_DBM, "INSERT ERROR!\n");
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
        // do nothing
    }

    if(operate_database("END"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "END OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }
    return true;
}


int update_material_info(int *materials, unsigned short material_num)
{
    int false = 0, true = 1;
    unsigned short int material_id;
    char sql[MAX_MID_FIELD_SIZE];
    char append_content[MAX_MATERIAL_NUM];
    int rc;
    char *material_belong_to = NULL;
    size_t res_length = 0;

    memset(append_content, 0, sizeof(append_content));
    rc = safe_snprintf(append_content, sizeof(append_content),"|%c", g_recipe_id + 0x30);

    if(operate_database("BEGIN"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "BEGIN OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    for (material_id=0; material_id<material_num; material_id++)
    {
        memset(sql, 0, sizeof(sql));
        rc = safe_snprintf(sql, sizeof(sql),
             "SELECT belong_to_recipe_list FROM material_info WHERE material_id = %d", materials[material_id]);

        if(rc>0 && rc<=sizeof(sql))
        {
            if(operate_database(sql))
            {
                #ifdef DEBUG
                LOG_INFO(LOG_TYPE_DBM, "SELECT OK!\n");
                #endif

                res_length = sizeof(PQgetvalue(g_PGres, 0, 0));
                material_belong_to = (char *)malloc((res_length + 2)*sizeof(char));
                memset(material_belong_to, 0, sizeof(material_belong_to));
                strcpy(material_belong_to, PQgetvalue(g_PGres, 0, 0));
                if (!is_contained(material_belong_to, g_recipe_id))
                {
                    material_belong_to = strcat(material_belong_to, append_content);
                    PQclear(g_PGres);

                    memset(sql, 0, sizeof(sql));
                    rc = safe_snprintf(sql, sizeof(sql),
                         "UPDATE material_info SET belong_to_recipe_list = '%s' WHERE material_id = %d", material_belong_to, materials[material_id]);
                    if(rc>0 && rc<=sizeof(sql))
                    {
                        if(operate_database(sql))
                        {
                            #ifdef DEBUG
                            LOG_INFO(LOG_TYPE_DBM, "UPDATE OK!\n");
                            #endif
                            PQclear(g_PGres);
                        }
                        else
                        {
                            LOG_ERR(LOG_TYPE_DBM, "UPDATE ERROR!\n");
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
                    continue;
                }
                
            }
            else
            {
                LOG_ERR(LOG_TYPE_DBM, "SELECT ERROR!\n");
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    if(operate_database("END"))
    {
        #ifdef DEBUG
        LOG_INFO(LOG_TYPE_DBM, "END OK!\n");
        #endif
        PQclear(g_PGres);
    }
    else
    {
        return false;
    }

    return true;
}

// to check whether the list (1|2|3|4|5) has the member
int is_contained(char *list, int member)
{
    int true = 1, false = 0;
    char *ptr = list;

    while(NULL != ptr)
    {
        if (*ptr == member+0x30)
        {
            return true;
        }
        ptr++;
        
        if ('|' == *ptr)
        {
            ptr++;
        }
        else
        {
            return false;
        }
    }
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
    char *loglevel = "info", *dish_file = NULL, *db_connect_file_path = NULL;
    char *logfile = "log_dbm.txt";
    char *progname, *p;

    progname = ((p = strrchr(argv[0], '/')) ? ++p : argv[0]);

    for (;;)
    {
        int option = getopt(argc, argv, "?hl:o:c:d:v" );
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

    while (FAILED == init_connect_database(db_connect_file_path))
    {
        LOG_ERR(LOG_TYPE_DBM, "Login database ERROR!\n");
        sleep(10);
        continue;
    }

    if (NULL == dish_file)
    {
        LOG_ERR(LOG_TYPE_DBM, "NO Dish File ...\n");
    }
    else
    {
        if (abstract_classifed_info_from(dish_file))
        {
            LOG_INFO(LOG_TYPE_DBM, "Abstract classified info from dish file OK!\n");

            process_material_list(g_material_list);

            process_url_info(g_URL, g_title, g_snatch);

            process_dish_info(g_dish_name); // without psuedo_dish
            printf("dish_name = %s\n", g_dish_name);
            printf("dish_id = %d\n", g_dish_id);

            process_psuedo_dish(g_material_id_list); // without relative link info

        }
        else
        {
            LOG_ERR(LOG_TYPE_DBM, "Abstract classified info from dish file ERROR!\n");
        }
    }


    PQfinish(g_conn);

    return (EXIT_SUCCESS);
}


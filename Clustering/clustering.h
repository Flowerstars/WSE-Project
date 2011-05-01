/* 
 * File:   clustering.h
 * Author: songhan
 *
 * Created on May 1, 2011, 2:00 PM
 */

#ifndef CLUSTERING_H
#define	CLUSTERING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <postgresql/libpq-fe.h>
#include "log_manager.h"

#define STR_VERSION     "1.0"
#define MAX_CONN_INFO_SIZE      128
#define MAX_SHORT_FIELD_SIZE    256
#define MAX_MID_FIELD_SIZE      1024
#define MAX_LONG_FIELD_SIZE     2048
#define MAX_BUF_SIZE            4096
#define MAX_MATERIAL_NUM        64
#define FAKE_ID                 0
#define SIMILAR_RATE            0

enum RETURN_VALUE
{
    FAILED = 0,
    SUCCESS,
}RETURN_VALUE;


int abstract_link_page_info(char *dish_file);
int process_link_pages(char **dish_names, int link_num);
int similar_dish_clustering(int similar_rate);
int is_similar(int *master_ids, int *other_ids, int master_material_num, int other_material_num, int similar_rate);

void convert_ids_to_list(int *ids, char *id_list, unsigned short id_num);
void convert_list_to_ids(int *ids, char *id_list, unsigned short id_num);
void counting_sort_ids(int *ids, int id_num);
int get_max_id(int *ids, int id_num);

#ifdef	__cplusplus
extern "C" {
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* CLUSTERING_H */


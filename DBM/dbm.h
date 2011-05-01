/* 
 * File:   dbm.h
 * Author: songhan
 *
 * Created on April 28, 2011, 2:16 PM
 */

#ifndef DBM_H
#define	DBM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <postgresql/libpq-fe.h>
#include "log_manager.h"


enum RETURN_VALUE
{
    FAILED = 0,
    SUCCESS,
}RETURN_VALUE;


int get_material_ids(char **materials, unsigned short material_num, int *ids);
void convert_ids_to_list(int *ids, char *id_list, unsigned short id_num);
int add_material_list_to_recipe(char *list);
int update_material_info(int *materials, unsigned short material_num);
int process_url_info(char *url, char *title, char *snatch);
int process_dish_info(char *dish_name);
int process_psuedo_dish(char *material_id_list);

int is_contained(char *material_belong_to, int g_recipe_id);

#ifdef	__cplusplus
extern "C" {
#endif




#ifdef	__cplusplus
}
#endif

#endif	/* DBM_H */


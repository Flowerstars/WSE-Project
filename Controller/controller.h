/* 
 * File:   controller.h
 * Author: songhan
 *
 * Created on May 1, 2011, 10:51 PM
 */

#ifndef CONTROLLER_H
#define	CONTROLLER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "log_manager.h"


#define STR_VERSION             "1.0"
#define MAX_CONN_INFO_SIZE      128
#define MAX_SHORT_FIELD_SIZE    256
#define MAX_MID_FIELD_SIZE      1024
#define MAX_LONG_FIELD_SIZE     2048
#define MAX_BUF_SIZE            4096
#define MAX_MATERIAL_NUM        64
#define FAKE_ID                 0
#define SIMILAR_RATE            0




int read_config_file(char *configfile);
void construct_commands();


#ifdef	__cplusplus
extern "C" {
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* CONTROLLER_H */


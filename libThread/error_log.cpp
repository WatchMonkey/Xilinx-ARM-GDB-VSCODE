/**
 * @file error_log.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "error_log.h"


/**
 * @struct error information
 * 
 */
static struct _err_info{
    int err_code;
    char err_string[ERR_STRING_LENGTH_MAX];
}err_info;



void set_err_info(int id,char* format,...)
{
    err_info.err_code = id;
    memset(&err_info.err_string,0x0,sizeof(err_info.err_string));
    if(NULL != format)
	{
		va_list temp_va;
		va_start(temp_va,format);
		vsprintf(err_info.err_string,format,temp_va);
        va_end(temp_va);
    }
}



char* get_err_string()
{
    return err_info.err_string;
}


int get_err_code()
{
    return err_info.err_code;
}
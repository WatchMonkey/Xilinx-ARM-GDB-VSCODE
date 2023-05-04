/*
 * sysfs_op.c
 *
 *  Created on: 2022年12月27日
 *      Author: Administrator
 */

#ifndef SRC_SYSFS_OP_C_
#define SRC_SYSFS_OP_C_

#include "sysfs_op.h"
#include "LogHeader.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>


#define RT_ERROR ((int)__LINE__)
#define DATA_ERROR ((int)-1)
typedef int status_t;
typedef unsigned int uint;


/*!
 * @details		以 符号, 分割分次写数据到文件中
 * @return		status
 */
status_t writeData(int file,char* data,int* length,bool log)
{
	if(log){
		DLog("write data origin:%s",data);
	}
	status_t temp_status = 0x0;
	int temp_writeAll = 0x0;
	char* temp_data = data;
	char* temp_str = NULL;
	char* temp_out = NULL;
	*length = 0x0;

	while(1){
		//split
		temp_str = strtok_r(temp_data,",",&temp_out);
		if(NULL == temp_str){
			break;
		}
		//write
		int temp_write = write(file,temp_str,strlen(temp_str) + 0x1);
		if(temp_write <= 0x0){
			return RT_ERROR;
		}else{
			if(log){
				DLog("write <%s>[%d] success",temp_str,temp_write);
				ILog("write <%s>[%d] success",temp_str,temp_write);
			}
			temp_writeAll += temp_write;
		}
		//sleep
		usleep(500);
		temp_data = NULL;
	}

	*length = temp_writeAll;
	return temp_status;
}



int sysctrl_write(sys_info_p info)
{
	int temp_status = 0x0;
	if(NULL == info){
		ELog("input parameter invalid");
		return RT_ERROR;
	}else if(NULL == info->sysfile_path || NULL == info->write_string){
		ELog("input parameter invalid");
		return RT_ERROR;
	}

	//detect file exist
	int temp_exist = access(info->sysfile_path,R_OK);
	if(temp_exist){
		ELog("file:%s not exist",info->sysfile_path);
		return RT_ERROR;
	}
	//
	int temp_write_ok = access(info->sysfile_path,W_OK);
	if(temp_write_ok)
	{
		ELog("file path:%s can't write",info->sysfile_path);
		return RT_ERROR;
	}

	//
	//open file
	int temp_file = open(info->sysfile_path,O_WRONLY);
	if(temp_file < 0x0){
		ELog("%s open failed:%s",info->sysfile_path,strerror(errno));
		return RT_ERROR;
	}
	//write data
	int temp_write = 0x0;
	temp_status = writeData(temp_file,info->write_string,&temp_write,true);
	if(temp_status){
		ELog("write data into %s failed:%s",info->sysfile_path,strerror(errno));
		temp_status = RT_ERROR;
	}else{
		ILog("writeData into:%s Byte:%d",info->sysfile_path,temp_write);
	}
	//close file
	close(temp_file);

	return temp_status;
}



#endif /* SRC_SYSFS_OP_C_ */

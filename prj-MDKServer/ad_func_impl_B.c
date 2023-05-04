/*
 * impl_A.c
 *
 *  Created on: 2021年10月28日
 *      Author: 14452
 */

#include "LogHeader.h"

#include <sys/types.h>
#include <dirent.h>
#include <libgen.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "ad_func_impl.h"
#include "ad_func_internal_header.h"
#include "ad_func_internal_macro.h"

status_t AD_getFrequency(int id,CHANNEL_DIR_t dir,int64_t* value,bool read_dev)
{
	if(NULL == value){
		return RT_ERROR;
	}
	if(false == read_dev)
		return RT_ERROR;

	if(false == __check_ad_ok(id))
		return RT_ERROR;
	//
	char full_path[0x400] = {0x0};
	char* temp_home = search_ad_home(id);
	if(NULL == temp_home){
		ELog("can't search target ID:%d",id);
		return RT_ERROR;
	}
	switch(dir){
		case D_TX:
			sprintf(full_path,"%s/%s",temp_home,TX_FREQUENCY);
			break;
		case D_RX:
			sprintf(full_path,"%s/%s",temp_home,RX_FREQUENCY);
			break;
		default:
			ELog("direction parameter invalid");
			return RT_ERROR;
	}
	//detect file exist
	int temp_exist = access(full_path,R_OK);
	if(temp_exist){
		ELog("file path:%s not exist",full_path);
		return RT_ERROR;
	}
	//open file
	int temp_adfile = open(full_path,O_RDONLY);
	if(temp_adfile < 0x0){
		ELog("%s open failed:%s",full_path,strerror(errno));
		return RT_ERROR;
	}

	char temp_value[0x100] = {0x0};
	int temp_adread = read(temp_adfile,temp_value,0x100);
	if(0x0 >= temp_adread){
		ELog("%s read data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}else{
		int64_t temp_int = strtoull(temp_value,NULL,10);
		if(NULL != value){
			*value = temp_int;
		}
	}

	//close file
	close(temp_adfile);
	GET_INFO_WAIT_MS(200);
	return 0x0;
}

status_t AD_getBandwidth(int id,CHANNEL_DIR_t dir,uint* value,bool read_dev)
{
	if(NULL == value){
		return RT_ERROR;
	}
	if(false == read_dev){
		return RT_ERROR;
	}

	if(false == __check_ad_ok(id))
		return RT_ERROR;
	//
	char full_path[0x400] = {0x0};
	char* temp_home = search_ad_home(id);
	if(NULL == temp_home){
		ELog("can't search target ID:%d",id);
		return RT_ERROR;
	}
	switch(dir){
		case D_TX:
			sprintf(full_path,"%s/%s",temp_home,TX_BANDWIDTH);
			break;
		case D_RX:
			sprintf(full_path,"%s/%s",temp_home,RX_BANDWIDTH);
			break;
		default:
			ELog("direction parameter invalid");
			return RT_ERROR;
	}
	//detect file exist
	int temp_exist = access(full_path,R_OK);
	if(temp_exist){
		ELog("file path:%s not exist",full_path);
		return RT_ERROR;
	}
	//open file
	int temp_adfile = open(full_path,O_RDWR);
	if(temp_adfile < 0x0){
		ELog("%s open failed:%s",full_path,strerror(errno));
		return RT_ERROR;
	}

	char temp_value[0x100] = {0x0};
	int temp_adread = read(temp_adfile,temp_value,0x100);
	if(0x0 >= temp_adread){
		ELog("%s read data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}else{
		uint temp_int = strtoul(temp_value,NULL,10);
		if(NULL != value){
			*value = temp_int;
		}
	}

	//close file
	close(temp_adfile);
	GET_INFO_WAIT_MS(200);
	return 0x0;
}


status_t AD_getRSSIValue(int id,CHANNEL_DIR_t dir,uint* value)
{
	if(NULL == value){
		return RT_ERROR;
	}

	if(false == __check_ad_ok(id))
		return RT_ERROR;
	//
	char full_path[0x400] = {0x0};
	char* temp_home = search_ad_home(id);
	if(NULL == temp_home){
		ELog("can't search target ID:%d",id);
		return RT_ERROR;
	}
	switch(dir){
		case D_TX:
			sprintf(full_path,"%s/%s",temp_home,TX_RSSI);
			break;
		case D_RX:
			sprintf(full_path,"%s/%s",temp_home,RX_RSSI);
			break;
		default:
			ELog("direction parameter invalid");
			return RT_ERROR;
	}
	//detect file exist
	int temp_exist = access(full_path,R_OK);
	if(temp_exist){
		ELog("file path:%s not exist",full_path);
		return RT_ERROR;
	}
	//open file
	int temp_adfile = open(full_path,O_RDWR);
	if(temp_adfile < 0x0){
		ELog("%s open failed:%s",full_path,strerror(errno));
		return RT_ERROR;
	}

	char temp_value[0x100] = {0x0};
	int temp_adread = read(temp_adfile,temp_value,0x100);
	if(0x0 >= temp_adread){
		ELog("%s read data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}
	//
	float temp_float = 0x0;
	int temp_sacnf = sscanf(temp_value,"%f",&temp_float);
	if(EOF != temp_sacnf){
		*value = temp_float * 100000;
	}else{
		*value = 0x0;
		close(temp_adfile);
		return RT_ERROR;
	}

	//close file
	close(temp_adfile);
	GET_INFO_WAIT_MS(100);
	return 0x0;
}

status_t AD_getGCMode(int id,GC_MODE_t* mode)
{
	if(NULL == mode){
		ELog("input parameter invalid");
		return RT_ERROR;
	}

	if(false == __check_ad_ok(id))
		return RT_ERROR;
	//
	char full_path[0x400] = {0x0};
	char* temp_home = search_ad_debugfs(id);
	if(NULL == temp_home){
		ELog("can't search target ID:%d",id);
		return RT_ERROR;
	}
	sprintf(full_path,"%s/%s",temp_home,FILENAME_REG_ACCESS);

	//detect file exist
	int temp_exist = access(full_path,R_OK);
	if(temp_exist){
		ELog("file path:%s not exist",full_path);
		return RT_ERROR;
	}
	//open file
	int temp_adfile = open(full_path,O_RDWR);
	if(temp_adfile < 0x0){
		ELog("%s open failed:%s",full_path,strerror(errno));
		return RT_ERROR;
	}

	char temp_valuebuffer[0x100] = {0x0};
	strcat(temp_valuebuffer,"0xFA");
	//write data
	int temp_adwrite = 0x0;
	int temp_rt = writeData(temp_adfile,temp_valuebuffer,&temp_adwrite,false);
	if(temp_rt){
		ELog("%s write data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}
	//
	char readBuf[0x100] = {0x0};
	int temp_adread = read(temp_adfile,readBuf,0x100);
	if(0x0 > temp_adread){
		ELog("%s read data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}
	//check value
	uint temp_readValue = strtoul(readBuf,NULL,0);
	GC_MODE_t temp_gcmode = M_UNKNOW;
	switch(temp_readValue)
	{
	case 0xEA:
		temp_gcmode = M_AUTO;
		break;
	case 0xE0:
		temp_gcmode = M_HANDLE;
		break;
	default:
		ELog("mode parameter invalid");
		close(temp_adfile);
		return RT_ERROR;
	}
	*mode = temp_gcmode;

	//close file
	close(temp_adfile);
	GET_INFO_WAIT_MS(200);
	return 0x0;
}

status_t AD_getGCValue(int id,CHANNEL_DIR_t dir,uint* value)
{
	if(NULL == value){
		ELog("input parameter invalid");
		return RT_ERROR;
	}

	if(false == __check_ad_ok(id))
		return RT_ERROR;
	//
	char full_path[0x400] = {0x0};
	char* temp_home = search_ad_debugfs(id);
	if(NULL == temp_home){
		ELog("can't search target ID:%d",id);
		return RT_ERROR;
	}
	sprintf(full_path,"%s/%s",temp_home,FILENAME_REG_ACCESS);
	//detect file exist
	int temp_exist = access(full_path,R_OK);
	if(temp_exist){
		ELog("file path:%s not exist",full_path);
		return RT_ERROR;
	}
	//open file
	int temp_adfile = open(full_path,O_RDWR);
	if(temp_adfile < 0x0){
		ELog("%s open failed:%s",full_path,strerror(errno));
		return RT_ERROR;
	}

	char temp_valuebuffer[0x100] = {0x0};
	switch(dir)
	{
	case D_TX:
		ELog("target channel direction invalid");
		close(temp_adfile);
		return RT_ERROR;
	case D_RX:
		strcat(temp_valuebuffer,"0x109");
		break;
	default:
		ELog("direction parameter invalid");
		close(temp_adfile);
		return RT_ERROR;
	}
	//write data
	int temp_adwrite = 0x0;
	int temp_rt = writeData(temp_adfile,temp_valuebuffer,&temp_adwrite,false);
	if(temp_rt){
		ELog("%s write data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}
	//read data
	char temp_readBuf[0x100] = {0x0};
	uint temp_readValue = 0x0;
	int temp_adread = read(temp_adfile,temp_readBuf,0x100);
	if(0x0 > temp_adread){
		ELog("%s read data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}else{
		temp_readValue = strtoul(temp_readBuf,NULL,0);
		*value = temp_readValue;
	}

	//close file
	close(temp_adfile);
	GET_INFO_WAIT_MS(200);
	return 0x0;
}

/*!
 * @details		获取在手动增益下的发射衰减值
 * @return		status_t
 */
status_t AD_getATTValue(int id,CHANNEL_DIR_t dir,uint* value)
{
	if(NULL == value){
		ELog("input parameter invalid");
		return RT_ERROR;
	}

	if(false == __check_ad_ok(id))
		return RT_ERROR;
	//
	char full_path[0x400] = {0x0};
	char* temp_home = search_ad_debugfs(id);
	if(NULL == temp_home){
		ELog("can't search target ID:%d",id);
		return RT_ERROR;
	}
	sprintf(full_path,"%s/%s",temp_home,FILENAME_REG_ACCESS);
	//detect file exist
	int temp_exist = access(full_path,R_OK);
	if(temp_exist){
		ELog("file path:%s not exist",full_path);
		return RT_ERROR;
	}
	//open file
	int temp_adfile = open(full_path,O_RDWR);
	if(temp_adfile < 0x0){
		ELog("%s open failed:%s",full_path,strerror(errno));
		return RT_ERROR;
	}

	char temp_valuebuffer[0x100] = {0x0};
	switch(dir)
	{
	case D_TX:
		strcat(temp_valuebuffer,"0x73");
		break;
	case D_RX:
		ELog("target channel direction invalid");
		close(temp_adfile);
		return RT_ERROR;
	default:
		ELog("direction parameter invalid");
		close(temp_adfile);
		return RT_ERROR;
	}
	//write data
	int temp_adwrite = 0x0;
	int temp_rt = writeData(temp_adfile,temp_valuebuffer,&temp_adwrite,false);
	if(temp_rt){
		ELog("%s write data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}
	//read data
	int temp_attValue = 0x0;
	char temp_readBuf[0x100] = {0x0};
	uint temp_readValue = 0x0;
	int temp_adread = read(temp_adfile,temp_readBuf,0x100);
	if(0x0 > temp_adread){
		ELog("%s read data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}else{
		temp_readValue = strtoul(temp_readBuf,NULL,16);
		temp_attValue += (0xFF & temp_readValue);
	}
	//prepare data
	memset(temp_valuebuffer,0x0,0x100);
	strcat(temp_valuebuffer,"0x74");
	//write data
	temp_adwrite = 0x0;
	temp_rt = writeData(temp_adfile,temp_valuebuffer,&temp_adwrite,false);
	if(temp_rt){
		ELog("%s write data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}
	//read data
	memset(temp_readBuf,0x0,0x100);
	temp_readValue = 0x0;
	temp_adread = 0x0;
	temp_adread = read(temp_adfile,temp_readBuf,0x100);
	if(0x0 > temp_adread){
		ELog("%s read data failed:%s",full_path,strerror(errno));
		close(temp_adfile);
		return RT_ERROR;
	}else{
		temp_readValue = strtoul(temp_readBuf,NULL,16);
		temp_attValue += ((0x0F & temp_readValue) << 0x8);
	}
	//save value
	*value = temp_attValue;

	//close file
	close(temp_adfile);
	GET_INFO_WAIT_MS(200);
	return 0x0;
}


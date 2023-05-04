/*
 * impl_A.c
 *
 *  Created on: 2021Äê10ÔÂ28ÈÕ
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

int register_ad_byname(int id,char* name)
{
	int temp_rt = 0x0;

	char* temp_str = NULL;
	char temp_home[0x400] = {0x0};
	temp_str = find_ad_home(name,NULL,true,false);
	if(NULL == temp_str){
		ELog("search target device:%s home path failed",name);
		return RT_ERROR;
	}else{
		sprintf(temp_home,"%s",temp_str);
		free(temp_str);
		ILog("AD:%d,home full path:%s",id,temp_home);
		DLog("AD:%d,home full path:%s",id,temp_home);
	}
	temp_str = NULL;
	char temp_debugfs[0x400] = {0x0};
	//char* temp_debugfs = find_ad_debugfs(name,NULL,true,false);
	temp_str = find_ad_debugfs(name,temp_home);
	if(NULL == temp_str){
		ELog("search target device:%s debugfs path failed",name);
		return RT_ERROR;
	}else{
		sprintf(temp_debugfs,"%s",temp_str);
		free(temp_str);
		ILog("AD:%d,debug fs full path:%s",id,temp_debugfs);
		DLog("AD:%d,debug fs full path:%s",id,temp_debugfs);
	}
	//
	temp_rt = register_ad_home(id,NULL,temp_home,temp_debugfs);
	if(temp_rt){
		ELog("register ad device failed");
		return RT_ERROR;
	}

	return temp_rt;
}

int register_ad_bypath(int id,char* home,char* debugfs)
{
	int temp_rt = 0x0;

	temp_rt = register_ad_home(id,NULL,home,debugfs);
	if(temp_rt){
		ELog("register ad device failed");
		return RT_ERROR;
	}

	return temp_rt;
}

char* find_ad_home(char* tag,char* search_dir,bool dir,bool file)
{
	int temp_rt = 0x0;
	if(NULL == tag)
		return NULL;
	if(NULL == search_dir)
		search_dir = AD_HOME_ROOT_DIR;

    DIR *temp_dir;
    struct dirent *dp;
    if (NULL == (temp_dir = opendir (search_dir)))
    {
        ELog("Cannot open %s",search_dir);
        return NULL;
    }
    //
    char* mid_path = NULL;
    char full_path[0x400] = {0x0};
    strcat(full_path,search_dir);
    char temp_path[0x400] = {0x0};
    //
    while ((dp = readdir (temp_dir)) != NULL)
    {
    	if(strcmp(dp->d_name,".") == 0x0 || strcmp(dp->d_name,"..") == 0x0)
    		continue;
    	//directory
    	if(dir && dp->d_type == 0xa)
    	{
    		memset(temp_path,0x0,sizeof(temp_path));
    		strcat(temp_path,full_path);
    		strcat(temp_path,"/");
    		strcat(temp_path,dp->d_name);
    		//
    		mid_path = find_ad_home(tag,temp_path,false,true);
    		if(NULL == mid_path)
    			continue;
    		else
    			return strdup(mid_path);//strdup(full_path);
    	}
    	//file
    	if(file && dp->d_type == 0x8)
		{
    		if(strcmp(dp->d_name,"name") != 0x0)
    			continue;
    		//
    		memset(temp_path,0x0,sizeof(temp_path));
    		strcat(temp_path,full_path);
    		strcat(temp_path,"/");
			strcat(temp_path,dp->d_name);
			//
			int temp_fp = open(temp_path,O_RDONLY);
			if(temp_fp < 0x0){
				ELog("%s open failed:%s",temp_path,strerror(errno));
				return NULL;
			}
			//
			char temp_readBuf[0x100] = {0x0};
			int temp_read = read(temp_fp,temp_readBuf,0x100);
			if(0x0 >= temp_read){
				ELog("%s read failed:%s",temp_path,strerror(errno));
				close(temp_fp);
				return NULL;
			}
			//remove last char \n
			char* temp_pos = strstr(temp_readBuf,"\n");
			if(NULL != temp_pos){
				*temp_pos = '\0';
			}
			if(strcmp(temp_readBuf,tag) == 0x0){
				ILog("search target device,tag:%s",tag);
				close(temp_fp);
				return search_dir;
			}
			//
			close(temp_fp);
		}
    }
    //
    closedir(temp_dir);
	return NULL;
}

char* find_ad_debugfs(char* tag,char* home_path)
{
	int temp_rt = 0x0;
	if(NULL == tag)
		return NULL;
	if(NULL == home_path)
		return NULL;

	//
	char full_path[0x400] = {0x0};
	strcat(full_path,AD_DEBUGFS_ROOT_DIR);
	strcat(full_path,"/");
	//
	char temp_str[0x100] = {0x0};
	char* temp_mid = strrchr(home_path,'/');
	if(NULL == temp_mid){
		ELog("can't search target char");
		return NULL;
	}
	sprintf(temp_str,"%s",(char*)(temp_mid + 1));
	strcat(full_path,temp_str);
	return strdup(full_path);
}

char* get_ad_home(int id)
{
	return search_ad_home(id);
}

char* get_ad_debugfs(int id)
{
	return search_ad_debugfs(id);
}

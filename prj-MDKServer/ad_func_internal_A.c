/*
 * internal_A.c
 *
 *  Created on: 2021Äê10ÔÂ28ÈÕ
 *      Author: 14452
 */

#include "LogHeader.h"

#include <pthread.h>
#include <math.h>
#include <string.h>

#include "ad_func_impl.h"

#define AD_ARRAY_SIZE 0x20
#define RT_ERROR ((int)__LINE__)
static int AD_ID[AD_ARRAY_SIZE] = {0x0};
static char* AD_NAME[AD_ARRAY_SIZE] = {NULL};
static char* AD_HOME[AD_ARRAY_SIZE] = {NULL};
static char* AD_DEBUGFS_HOME[AD_ARRAY_SIZE] = {NULL};
static pthread_mutex_t array_mutex = PTHREAD_MUTEX_INITIALIZER;


static pthread_mutex_t ok_mutex = PTHREAD_MUTEX_INITIALIZER;
static bool AD_OK[AD_ARRAY_SIZE] = {false};


/**
 * @brief		check ad status
 * @return		bool
 * @retval		true	ad can use
 * 				false	ad can't use
 */
bool __check_ad_ok(int ID)
{
	if(0x0 >= ID)
		return NULL;
	//
	for(int i = 0x0;i < AD_ARRAY_SIZE;++i)
	{
		if(AD_ID[i] != ID)
			continue;
		//
		return AD_OK[i];
	}
	return false;
}


/**
 * @brief		set ad status
 * @return		int
 * @retval		0x0	success
 * 				other	failed
 */
int __set_ad_ok(int ID,bool ok)
{
	if(0x0 >= ID)
		return RT_ERROR;
	//
	for(int i = 0x0;i < AD_ARRAY_SIZE;++i)
	{
		if(AD_ID[i] != ID)
			continue;
		//
		pthread_mutex_lock(&ok_mutex);
		AD_OK[i] = ok;
		pthread_mutex_unlock(&ok_mutex);
		return 0x0;
	}
	return RT_ERROR;
}


/**
 * @brief		wait ad can use
 * @return		bool
 * @retval		true	ad device can use
 * 				false	ad can't use
 */
bool __wait_ad_ok(int ID,int ms)
{
	if(0x0 >= ID)
		return false;
	if(0x0 > ms)
		return false;
	//
	for(int i = 0x0;i < AD_ARRAY_SIZE;++i)
	{
		if(AD_ID[i] != ID)
			continue;
		//
		do{
			if(true == AD_OK[i])
				return true;
			if(!ms--)
				return false;
			//
			usleep(1000);
		}while(1);
	}
	return false;
}


/**
 * @brief		save id and home path
 * @return		status_t
 * @retval		0x0		success
 * 				other	failed
 */
status_t register_ad_home(int ID,char* name,char* home,char* debugfs)
{
	if(NULL == home || 0x0 >= ID)
		return RT_ERROR;
	//
	for(int i = 0x0;i < AD_ARRAY_SIZE;++i)
	{
		if(0x0 >= AD_ID[i])
			continue;
		//replace
		if(ID == AD_ID[i])
		{
			pthread_mutex_lock(&array_mutex);
			//home
			if(NULL != AD_HOME[i])
				free(AD_HOME[i]);
			AD_HOME[i] = strdup(home);
			//debugfs
			if(NULL != AD_DEBUGFS_HOME[i])
				free(AD_DEBUGFS_HOME[i]);
			AD_DEBUGFS_HOME[i] = strdup(debugfs);
			//name
			if(NULL != name){
				if(NULL != AD_NAME[i])
					free(AD_NAME[i]);
				AD_NAME[i] = strdup(name);
			}
			pthread_mutex_unlock(&array_mutex);
			pthread_mutex_lock(&ok_mutex);
			//ok
			AD_OK[i] = true;
			pthread_mutex_unlock(&ok_mutex);
			return 0x0;
		}
	}
	//
	for(int i = 0x0;i < AD_ARRAY_SIZE;++i)
	{
		//add
		if(0x0 >= AD_ID[i])
		{
			pthread_mutex_lock(&array_mutex);
			//id
			AD_ID[i] = ID;
			//home
			if(NULL != AD_HOME[i])
				free(AD_HOME[i]);
			AD_HOME[i] = strdup(home);
			//debugfs
			if(NULL != AD_DEBUGFS_HOME[i])
				free(AD_DEBUGFS_HOME[i]);
			AD_DEBUGFS_HOME[i] = strdup(debugfs);
			//name
			if(NULL != name){
				if(NULL != AD_NAME[i])
					free(AD_NAME[i]);
				AD_NAME[i] = strdup(name);
			}
			pthread_mutex_unlock(&array_mutex);
			pthread_mutex_lock(&ok_mutex);
			//ok
			AD_OK[i] = true;
			pthread_mutex_unlock(&ok_mutex);
			return 0x0;
		}
	}
	return RT_ERROR;
}

/**
 * @brief		delete id and home path
 * @return		void
 */
void delete_ad_home(int ID)
{
	for(int i = 0x0;i < AD_ARRAY_SIZE;++i)
	{
		if(AD_ID[i] != ID)
			continue;
		pthread_mutex_lock(&array_mutex);
		//id
		AD_ID[i] = 0x0;
		//home
		if(NULL != AD_HOME[i]){
			free(AD_HOME[i]);
			AD_HOME[i] = NULL;
		}
		//debugfs
		if(NULL != AD_DEBUGFS_HOME[i]){
			free(AD_DEBUGFS_HOME[i]);
			AD_DEBUGFS_HOME[i] = NULL;
		}
		//name
		if(NULL != AD_NAME[i]){
			free(AD_NAME[i]);
			AD_NAME[i] = NULL;
		}
		pthread_mutex_unlock(&array_mutex);
		pthread_mutex_lock(&ok_mutex);
		//ok
		AD_OK[i] = false;
		pthread_mutex_unlock(&ok_mutex);
	}
}

/*!
 * @brief		search target ID,home path
 * @param		ID
 * @return		char*	home path
 */
char* search_ad_home(int ID)
{
	if(0x0 >= ID)
		return NULL;
	//
	for(int i = 0x0;i < AD_ARRAY_SIZE;++i)
	{
		if(AD_ID[i] != ID)
			continue;
		//
		return AD_HOME[i];
	}
	return NULL;
}

/*!
 * @brief		search target ID,debufs home path
 * @param		ID
 * @return		char*	debugfs home path
 */
char* search_ad_debugfs(int ID)
{
	if(0x0 >= ID)
		return NULL;
	//
	for(int i = 0x0;i < AD_ARRAY_SIZE;++i)
	{
		if(AD_ID[i] != ID)
			continue;
		//
		return AD_DEBUGFS_HOME[i];
	}
	return NULL;
}

/*!
 * @brief		search target ID,device name
 * @param		ID
 * @return		char*	device name
 */
char* search_ad_name(int ID)
{
	if(0x0 >= ID)
		return NULL;
	//
	for(int i = 0x0;i < AD_ARRAY_SIZE;++i)
	{
		if(AD_ID[i] != ID)
			continue;
		//
		return AD_NAME[i];
	}
	return NULL;
}

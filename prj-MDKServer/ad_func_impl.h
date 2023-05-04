/*
 * internal.h
 *
 *  Created on: 2021年10月28日
 *      Author: 14452
 */

#ifndef SRC_SYSFS_OP_IMPL_H_
#define SRC_SYSFS_OP_IMPL_H_
#ifdef __cplusplus
extern "C"{
#endif

#include <stdbool.h>
#include <stdint.h>

typedef int status_t;
typedef unsigned int uint;
#define RT_ERROR ((int)__LINE__)
#define DATA_ERROR ((int)-1)

//! AD芯片GC增益模式
typedef enum GC_MODE{M_AUTO = 0x1,M_HANDLE = 0x2,M_UNKNOW = 0x0} GC_MODE_t;
//!	AD芯片通道方向
typedef enum CHANNEL_DIR{D_TX = 0x1,D_RX = 0x2}CHANNEL_DIR_t;


//*************************Init*************************//

/*!
 * @brief		register ad device
 * @param		id
 * @param		name		ad device tag name
 * @return		status_t
 * @retval		0x0		success
 * 				other	failed
 */
int register_ad_byname(int id,char* name);

/*!
 * @brief		register ad device
 * @param		id
 * @param		home		the full director path
 * @param		debugfs		the full director path
 * @return		status_t
 * @retval		0x0		success
 * 				other	failed
 */
int register_ad_bypath(int id,char* home,char* debugfs);


/**
 * @brief		search target ad full path by device tag name
 * @param		tag
 * @param		search_dir
 * @param		dir
 * @param		file
 * @return		char*
 * 				NULL	failed
 */
char* find_ad_home(char* tag,char* search_dir,bool dir,bool file);

/**
 * @brief		search target ad full path by device tag name
 * @param		tag
 * @param		search_dir
 * @param		dir
 * @param		file
 * @return		char*
 * 				NULL	failed
 */
char* find_ad_debugfs(char* tag,char* home_path);
//char* find_ad_debugfs(char* tag,char* search_dir,bool dir,bool file);


/**
 * @brief		get target ad full path by device tag name
 * @param		tag
 * @param		search_dir
 * @param		dir
 * @param		file
 * @return		char*
 * 				NULL	failed
 */
char* get_ad_home(int id);

/**
 * @brief		get target ad full path by device tag name
 * @param		tag
 * @param		search_dir
 * @param		dir
 * @param		file
 * @return		char*
 * 				NULL	failed
 */
char* get_ad_debugfs(int id);

/**
 * @brief		check ad use status
 * @param		ID
 * @return		false	can't use
 */
bool check_ad_ok(int ID);


//*************************Get Value*************************//

/*!
 * @brief		获取AD频率
 * @param		int:AD芯片编号
 * @param		CHANNEL_DIR_t:信号通道
 * @param		int64_t*:频率值
 * @param		bool:读取设备
 * @return		status_t
 */
status_t AD_getFrequency(int,CHANNEL_DIR_t,int64_t*,bool);

/*!
 * @brief		获取AD带宽
 * @param		int:AD芯片编号
 * @param		CHANNEL_DIR_t:信号通道
 * @param		uint*:频率值
 * @param		bool:读取设备
 * @return		status_t
 */
status_t AD_getBandwidth(int,CHANNEL_DIR_t,uint*,bool);

/*!
 * @brief		读取GC模式
 * @param 		int:AD芯片ID
 * @param		GC_MODE_t*:模式
 * @return		status_t
 * @retval		0x0--success
 * @retval		other--failed
 */
status_t AD_getGCMode(int,GC_MODE_t*);

/*!
 * @brief		获取手动GC值
 * @param		int:AD芯片ID
 * @param		CHANNEL_DIR_t:信号通道
 * @param		uint*:GC值
 * @return		status_t
 */
status_t AD_getGCValue(int,CHANNEL_DIR_t,uint*);

/*!
 * @brief		获取衰减值
 * @param		int:AD芯片ID
 * @param		CHANNEL_DIR_t:信号通道
 * @param		uint*:衰减值
 * @return		status_t
 */
status_t AD_getATTValue(int,CHANNEL_DIR_t,uint*);

/*!
 * @brief		获取AD的RSSI的值,也就是信号强度的值
 * @param		int:AD芯片编号
 * @param		CHANNEL_DIR_t:信号通道
 * @param		uint:rssi值
 * @return		status_t
 * @attention	读出的值是float,但是在返回时:rssi值  * 100000
 */
status_t AD_getRSSIValue(int,CHANNEL_DIR_t,uint*);

#ifdef __cplusplus
}
#endif
#endif /* SRC_SYSFS_OP_INTERNAL_H_ */

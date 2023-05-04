/*
 * internal.h
 *
 *  Created on: 2021��10��28��
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

//! ADоƬGC����ģʽ
typedef enum GC_MODE{M_AUTO = 0x1,M_HANDLE = 0x2,M_UNKNOW = 0x0} GC_MODE_t;
//!	ADоƬͨ������
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
 * @brief		��ȡADƵ��
 * @param		int:ADоƬ���
 * @param		CHANNEL_DIR_t:�ź�ͨ��
 * @param		int64_t*:Ƶ��ֵ
 * @param		bool:��ȡ�豸
 * @return		status_t
 */
status_t AD_getFrequency(int,CHANNEL_DIR_t,int64_t*,bool);

/*!
 * @brief		��ȡAD����
 * @param		int:ADоƬ���
 * @param		CHANNEL_DIR_t:�ź�ͨ��
 * @param		uint*:Ƶ��ֵ
 * @param		bool:��ȡ�豸
 * @return		status_t
 */
status_t AD_getBandwidth(int,CHANNEL_DIR_t,uint*,bool);

/*!
 * @brief		��ȡGCģʽ
 * @param 		int:ADоƬID
 * @param		GC_MODE_t*:ģʽ
 * @return		status_t
 * @retval		0x0--success
 * @retval		other--failed
 */
status_t AD_getGCMode(int,GC_MODE_t*);

/*!
 * @brief		��ȡ�ֶ�GCֵ
 * @param		int:ADоƬID
 * @param		CHANNEL_DIR_t:�ź�ͨ��
 * @param		uint*:GCֵ
 * @return		status_t
 */
status_t AD_getGCValue(int,CHANNEL_DIR_t,uint*);

/*!
 * @brief		��ȡ˥��ֵ
 * @param		int:ADоƬID
 * @param		CHANNEL_DIR_t:�ź�ͨ��
 * @param		uint*:˥��ֵ
 * @return		status_t
 */
status_t AD_getATTValue(int,CHANNEL_DIR_t,uint*);

/*!
 * @brief		��ȡAD��RSSI��ֵ,Ҳ�����ź�ǿ�ȵ�ֵ
 * @param		int:ADоƬ���
 * @param		CHANNEL_DIR_t:�ź�ͨ��
 * @param		uint:rssiֵ
 * @return		status_t
 * @attention	������ֵ��float,�����ڷ���ʱ:rssiֵ  * 100000
 */
status_t AD_getRSSIValue(int,CHANNEL_DIR_t,uint*);

#ifdef __cplusplus
}
#endif
#endif /* SRC_SYSFS_OP_INTERNAL_H_ */

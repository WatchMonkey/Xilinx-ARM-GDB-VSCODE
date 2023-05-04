/*
 * internal.h
 *
 *  Created on: 2021Äê10ÔÂ28ÈÕ
 *      Author: 14452
 */

#ifndef SRC_SYSFS_OP_INTERNAL_H_
#define SRC_SYSFS_OP_INTERNAL_H_
#ifdef __cplusplus
extern "C"{
#endif

typedef int status_t;

#define RT_ERROR ((int)__LINE__)
#define DATA_ERROR ((int)-1)


#include <stdbool.h>


/**
 * @brief		save id and home path
 * @return		status_t
 * @retval		0x0		success
 * 				other	failed
 */
status_t register_ad_home(int ID,char* name,char* home,char* debugfs);

/**
 * @brief		delete id and home path
 * @return		void
 */
void delete_ad_home(int ID);

/*!
 * @brief		search target ID,home path
 * @param		ID
 * @return		char*	home path
 */
char* search_ad_home(int ID);

/*!
 * @brief		search target ID,debufs home path
 * @param		ID
 * @return		char*	debugfs home path
 */
char* search_ad_debugfs(int ID);

/*!
 * @brief		search target ID,home path
 * @param		ID
 * @return		char*	device name
 */
char* search_ad_name(int ID);

/**
 * @brief		check ad status
 * @return		bool
 * @retval		true	ad can use
 * 				false	ad can't use
 */
bool __check_ad_ok(int ID);

/**
 * @brief		set ad status
 * @return		int
 * @retval		0x0	success
 * 				other	failed
 */
int __set_ad_ok(int ID,bool ok);

/**
 * @brief		wait ad can use
 * @return		bool
 * @retval		true	ad device can use
 * 				false	ad can't use
 */
bool __wait_ad_ok(int ID,int ms);

#ifdef __cplusplus
}
#endif
#endif /* SRC_SYSFS_OP_INTERNAL_H_ */

/*
 * sysfs_op.h
 *
 *  Created on: 2022Äê12ÔÂ27ÈÕ
 *      Author: Administrator
 */

#ifndef SRC_SYSFS_OP_H_
#define SRC_SYSFS_OP_H_

/*!
 * @brief		read string handler function
 */
typedef int (*func)(const char* str,int byte);

/*!
 * @struct		sysfs control reference information
 */
typedef struct sysctrl_info{
	const char* sysfile_path;
	const char* write_string;
	func read_func;
}sys_info_s,*sys_info_p;


/*!
 * @brief		write some information through sysfs
 * @param		info	information
 * @return		int
 * @retval		0x0		success
 * 				other	failed
 */
int sysctrl_write(sys_info_p info);

#endif /* SRC_SYSFS_OP_H_ */

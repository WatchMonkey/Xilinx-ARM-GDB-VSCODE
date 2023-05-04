/*
 * network_op.h
 *
 *  Created on: 2022Äê11ÔÂ23ÈÕ
 *      Author: Administrator
 */

#ifndef SRC_NETWORK_OP_H_
#define SRC_NETWORK_OP_H_

/**
 * @brief		initialize network
 * @param		out
 * 					0x0		success
 * 					other	failed
 */
void network_init(const char* ip,int port,const char* protocol,int max_conn,int* out);


void network_delete();

/**
 * @biref		read network data
 * @param		ptr		memory point
 * @return		int		read usr data size,byte
 * @retval		-1		read failed
 */
void network_read(void** ptr,int* out);

/**
 * @brief		write network data
 * @param		data	memory point
 * @param		size_byte	data size
 * @return		int		write user data size
 * @retval		-1		write failed
 */
void network_write(void* data,int size_byte,int* out);


void gentest_frm();

#endif /* SRC_NETWORK_OP_H_ */

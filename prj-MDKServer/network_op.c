/*
 * network_op.c
 *
 *  Created on: 2022年11月23日
 *      Author: Administrator
 */


#include "network_op.h"
#include <stdbool.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include "LogHeader.h"
#include "device_ctrl.h"
#include "ad_func_impl.h"

#define RT_ERROR ((int)__LINE__)
#define DATA_ERROR ((int)-1)
#define NET_FRM_HEAD 0xEB90
#define NET_FRM_END 0x0D0A
#define NET_REMAIN_DEFAULT 0xFFFF
#define FRM_BUFFER_SIZE 0x8000
#define NET_READ_MUST_SUCCESS false
#define SIMULATE_CMD 0x0
#define SERVICE_CMD 0x1

typedef struct client_addr_info{
	int client_sock;
	struct sockaddr_in client_addr;
}*client_addr_info_ptr;
typedef enum {control_net_frm = 0x1,data_net_frm = 0x2,back_control_net_frm = 0x3}net_frm_type_t;
typedef enum {
	reg_write_cmd = 0x1001,
	reg_read_cmd = 0x1002,
	reg_read_resp = 0x1100,
	//
	in_rf_freq = 0x3001,
	out_rf_freq = 0x3002,
	in_rf_bd = 0x3003,
	out_rf_bd = 0x3004,
	agc_rf_value = 0x3005,
	agc_rf_mode = 0x3006,
	agc_rf_att = 0x3007,
	//rf_flag_resp = 0x3100,
	rf_value_read = 0x3200,
	//
	fpga_bit_path = 0x5001,
	fpga_reconfig_resp = 0x5100,
	//
	simu_exe_start = 0x6001,
	simu_exe_stop = 0x6002,
	simu_exe_restart = 0x6003,
	simu_exe_resp = 0x6100
}ctrl_frm_cmd_t;
typedef struct net_resp_info{
	int cmd;
	int param_size;
	void* param_ptr;
}net_resp_info_s,*net_resp_info_p;

static int check_accpet_client(int listen_sock,int max_conn,client_addr_info_ptr addr_info,bool update);
static int gen_net_frm(void* data,int size_byte,int type);
static int extract_net_frm(void* data,int size_byte,int* type);
static int check_net_frm_valid(void* origin,int size_byte);
static int handler_control_frm(void* data,int size_byte,int type);
static void net_resp_msg(net_resp_info_p resp_info,client_addr_info_ptr client_info);

static bool init_flag = false;
static int listenfd = -1;//listenfd   socket描述符
static char* recv_buffer = NULL;
static int client_max_size = 0x0;
static client_addr_info_ptr client_info = NULL;
static char gen_temp_buffer[FRM_BUFFER_SIZE] = {0x0};
static char extract_temp_buffer[FRM_BUFFER_SIZE] = {0x0};
static char net_temp_buffer[FRM_BUFFER_SIZE] = {0x0};
//
static unsigned int gen_net_frm_data_index = 0x0;
static unsigned int gen_net_frm_control_index = 0x0;

void network_init(const char* ip,int port,const char* protocol,int max_conn,int* out)
{
	if(NULL == out)
		return;
	if(init_flag){
		WLog("network already initialize success");
		*out = 0x0;return;
	}

	gen_net_frm_data_index = 0x0;
	gen_net_frm_control_index = 0x0;
	//
	if(NULL == protocol){
		*out = RT_ERROR;return;
	}
	if(NULL == recv_buffer)
		recv_buffer = calloc(4096,sizeof(char));
	else
		memset(recv_buffer,0x0,4096);
	if(NULL == recv_buffer){
		*out = RT_ERROR;return;
	}

	//
	client_max_size = max_conn;
	if(NULL == client_info)
		client_info = calloc(max_conn,sizeof(struct client_addr_info));
	if(NULL == client_info){
		free(recv_buffer);
		recv_buffer = NULL;
		*out = RT_ERROR;return;
	}

	//
	if(!strcmp("tcp",protocol) || !strcmp("TCP",protocol) || !strcmp("Tcp",protocol))
	{
		//创建一个TCP的socket
		if( (listenfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
			ELog("create socket error: %s (errno :%d)",strerror(errno),errno);
			*out = RT_ERROR;return;
		}else{
			//addr refused
			int optval = 1;
			int nRet = setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,(char*)&optval,sizeof(int));
			if(-1 == nRet){
				ELog("set socket option error: %s (errno :%d)",strerror(errno),errno);
				*out = RT_ERROR;return;
			}
			//keep alive
			nRet = setsockopt(listenfd,SOL_SOCKET,SO_KEEPALIVE,(char*)&optval,sizeof(int));
			if(-1 == nRet){
				ELog("set socket option error: %s (errno :%d)",strerror(errno),errno);
				*out = RT_ERROR;return;
			}
		}
		struct sockaddr_in servaddr;
		 //先把地址清空，检测任意IP
		memset(&servaddr,0,sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		if (NULL == ip || 0 == strcmp("",ip)){
			servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		}else{
			servaddr.sin_addr.s_addr = inet_addr(ip);
		}
		servaddr.sin_port = htons(port);
		//地址绑定到listenfd
		if ( bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1) {
			ELog("bind socket error: %s (errno :%d)",strerror(errno),errno);
			*out = RT_ERROR;return;
		}
		//监听端口号listenfd  最多10个等待
		if( listen(listenfd,1) == -1) {
			ELog("listen socket error: %s (errno :%d)",strerror(errno),errno);
			*out = RT_ERROR;return;
		}
		ILog("====waiting for client's request=======");
		//accept 和recv,注意接收字符串添加结束符'\0'
		//获取文件的Mode值,设置成非阻塞模式
		int Mode = fcntl(listenfd, F_GETFL, 0);
		if( fcntl(listenfd, F_SETFL, Mode | O_NONBLOCK) == -1)
		{
			ELog("listen socket set none block failed: %s (errno :%d)",strerror(errno),errno);
			*out = RT_ERROR;return;
		}

		init_flag = true;
		*out = 0x0;return;
	}
	//
	if(!strcmp("udp",protocol) || !strcmp("UDP",protocol) || !strcmp("Udp",protocol))
	{
		//deal with

		//
		init_flag = false;
		*out = 0x0;return;
	}

	*out = RT_ERROR;return;
}


void network_delete()
{
	if(!init_flag)
		return;
	//关闭连接
	close(listenfd);
	for(int i  = 0x0;i < client_max_size;++i)
	{
		if(0x0 != client_info[i].client_sock)
		{
			close(client_info[i].client_sock);
			ILog("client %d closed",client_info[i].client_sock);
			memset(&client_info[i].client_addr,0x0,sizeof(struct sockaddr_in));
		}
	}
	free(client_info);
	//
	free(recv_buffer);
	recv_buffer = NULL;
	//
	client_max_size = 0x0;
	//
	init_flag = false;
}


void network_read(void** ptr,int* out)
{
	if(NULL == out)
		return;
	if(!init_flag){
		*out = DATA_ERROR;return;
	}
	if(NULL == ptr){
		*out = DATA_ERROR;return;
	}

	//check client size
	int client_size = check_accpet_client(listenfd,client_max_size,client_info,true);
	if(0x0 == client_size){
		*out = DATA_ERROR;return;
	}

	int read_byte = 0x0,user_byte = 0x0,frm_length = 0x0;
	int handler_flag = 0x0;
	int frm_type = 0x0;
	//no need to loop recv data
	do{
		for(int i = 0x0;i < client_max_size;++i)
		{
			if(0x0 >= client_info[i].client_sock)
				continue;
			//read data
			memset(net_temp_buffer,0x0,FRM_BUFFER_SIZE);
			read_byte = recv(client_info[i].client_sock,net_temp_buffer,FRM_BUFFER_SIZE,0);
			if(0x0 > read_byte){
				if(ETIMEDOUT == errno)
				{
					ELog("keepalive timeout");
					//
					close(client_info[i].client_sock);
					client_info[i].client_sock = -1;
					//
					struct in_addr in  = client_info[i].client_addr.sin_addr;
					char str[INET_ADDRSTRLEN];   //INET_ADDRSTRLEN这个宏系统默认定义 16
					//成功的话此时IP地址保存在str字符串中。
					inet_ntop(AF_INET,&in, str, sizeof(str));
					DLog("keepalive timeout,client ip:%s,port:%d disconnect",str,ntohs(client_info[i].client_addr.sin_port));
					if(0x0 >= --client_size){
						*out = DATA_ERROR;return;
					}
				}
				else if(EAGAIN == errno){
					//ELog(" client %d recv cant read any data",client_info[i].client_sock);
				}else if(EINTR == errno){
					//
				}else{
					ELog("client %d recv failed:%d,%s",client_info[i].client_sock,errno,strerror(errno));
				}
				continue;
			}else if(0x0 == read_byte){
				close(client_info[i].client_sock);
				client_info[i].client_sock = -1;
				//
				struct in_addr in  = client_info[i].client_addr.sin_addr;
				char str[INET_ADDRSTRLEN];   //INET_ADDRSTRLEN这个宏系统默认定义 16
				//成功的话此时IP地址保存在str字符串中。
				inet_ntop(AF_INET,&in, str, sizeof(str));
				DLog("normal client ip:%s,port:%d disconnect",str,ntohs(client_info[i].client_addr.sin_port));
				//ELog(" client %d recv failed:%d,%s",client_info[i].client_sock,errno,strerror(errno));
				if(0x0 >= --client_size){
					*out = DATA_ERROR;return;
				}
			}
			//
			frm_length = check_net_frm_valid(net_temp_buffer,read_byte);
			if(0x0 > frm_length){
				ELog("check net frame valid failed");
				continue;
			}else if(0x0 == frm_length){
				continue;
			}
			//
			user_byte = extract_net_frm(net_temp_buffer,frm_length,&frm_type);
			if(0x0 >= user_byte){
				ELog("extract net frame data failed,data from client:%d",client_info[i].client_sock);
				continue;
			}
			//
			if(data_net_frm == frm_type){
				*ptr = &net_temp_buffer[0];
				*out = user_byte;return;
			}else if(control_net_frm == frm_type){
				handler_flag = handler_control_frm(net_temp_buffer,user_byte,frm_type);
				if(handler_flag)
					ELog("control net frame,deal with occure error");
				continue;
			}else{
				ELog("invalid frame type:%d",frm_type);
				continue;
			}
		}
	}while(NET_READ_MUST_SUCCESS);
	//
	*out = DATA_ERROR;return;
}


void network_write(void* data,int size_byte,int* out)
{
	if (NULL == out)
		return;
	if (!init_flag) {
		*out = DATA_ERROR;
		return;
	}
	//
	if(0x0 >= size_byte){
		*out = DATA_ERROR;
		return;
	}

	//check client size
	int client_size = check_accpet_client(listenfd,client_max_size,client_info,false);
	if (0x0 == client_size) {
		*out = DATA_ERROR;
		return;
	}

	int data_byte = gen_net_frm(data,size_byte,data_net_frm);
	if (0x0 >= data_byte) {
		*out = DATA_ERROR;
		return;
	}

	int write_byte = 0x0;
	for (int i = 0x0;i < client_max_size;++i)
	{
		if (0x0 >= client_info[i].client_sock)
			continue;

		//send data
		write_byte = send(client_info[i].client_sock,data,data_byte,0);
		if (data_byte != write_byte)
			ELog("send msg to client[%d] failed.data %d byte",client_info[i].client_sock,write_byte);
		else {
			//DLog("send msg to client[%d] success.data %d byte",client_info[i].client_sock,write_byte);
		}
	}

	*out = write_byte;
	return;
}


void gentest_frm()
{
	void* data = NULL;
	int pos = 0x0;
	char temp_array[0xFF] = {0x0};


	for(int i = 0x0;i < 0x20;++i){
		temp_array[i] = i + 0x20;
	}
	printf("Data Network Frame\n");
	//
	int data_byte = gen_net_frm(&temp_array[0],0x20,data_net_frm);
	for(int i = 0x0;i < data_byte;++i)
	{
		printf(" %02x ",temp_array[i]);
	}
	printf("\n");


	//control parameter frame data
	memset(temp_array,0x0,0xFF);
	pos = 0x0;
	data = &temp_array[0];
	//
	*(unsigned int*)(data + pos) = 0x1001;
	pos += sizeof(int);
	*(unsigned int*)(data + pos) = 8;
	pos += sizeof(int);
	*(unsigned int*)(data + pos) = 0x300;
	pos += sizeof(int);
	*(unsigned int*)(data + pos) = 0x999;
	pos += sizeof(int);
	printf("Control Network Frame:\n");
	//
	data_byte = gen_net_frm(&temp_array[0],pos,control_net_frm);
	for(int i = 0x0;i < data_byte;++i)
	{
		printf(" %02x ",temp_array[i]);
	}
	printf("\n");



	memset(temp_array,0x0,0xFF);
	pos = 0x0;
	data = &temp_array[0];
	char* temp_temp_bit_path = "/home/root/simu.elf";
	//
	*(unsigned int*)(data + pos) = 0x6001;
	pos += sizeof(int);
	*(unsigned int*)(data + pos) = strlen(temp_temp_bit_path);
	pos += sizeof(int);
	strcpy((void*)(data + pos),temp_temp_bit_path);
	pos += strlen(temp_temp_bit_path);
	printf("Control Network Frame:\n");
	//
	data_byte = gen_net_frm(&temp_array[0],pos,control_net_frm);
	for(int i = 0x0;i < data_byte;++i)
	{
		printf(" %02x ",temp_array[i]);
	}
	printf("\n");
}


/**
 * @brief		check server listen socket,and accpet client connect
 * @param		listen_sock
 * @param		max_conn	能接收的最大的客户端的连接数量
 * @param		addr_info
 * @param		update		是否需要检测客户端的连接
 * @return		int
 * @retval		0x0	success
 * 				other	failed
 */
static int check_accpet_client(int listen_sock,int max_conn,client_addr_info_ptr addr_info,bool update)
{
	int client_size = 0x0;
	if(0x0 >= listen_sock)
		return client_size;
	if(NULL == addr_info)
		return client_size;


	bool force_close = false;
	//check client size
	int empty_item = 0x0;
	int empty_index = -1;
	for(int i = 0x0;i < max_conn;++i)
	{
		if(0x0 >= addr_info[i].client_sock)
		{
			empty_item++;
			if(0x0 > empty_index)
				empty_index = i;
		}
	}
	if(!empty_item)
		force_close = true;
	client_size = max_conn - empty_item;
	if(!update)
		return client_size;


	struct sockaddr_in client_addr;
	int client_addr_size = sizeof(struct sockaddr_in);
	int connfd = 0x0;
	//接受请求连接
	connfd = accept(listenfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addr_size);
	//connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
	if(-1 == connfd) {
		//ELog("accpt socket error: %s (errno :%d)",strerror(errno),errno);
		return client_size;
	}else{
		DLog("connfd = %d",connfd);
		struct in_addr in  = client_addr.sin_addr;
		char str[INET_ADDRSTRLEN];   //INET_ADDRSTRLEN这个宏系统默认定义 16
		//成功的话此时IP地址保存在str字符串中。
		inet_ntop(AF_INET,&in, str, sizeof(str));
		DLog("client ip:%s,port:%d",str,ntohs(client_addr.sin_port));
	}
	//强制关闭,由于已经达到最大连接数量
	if(force_close)
	{
		close(connfd);
		ILog("force close client %d,because client connect bigger than max number",connfd);
		return client_size;
	}

	int nRet = 0x0;
	//
	int sendBufferSize = 32 * 1024; // 设置发送缓冲区大小
	nRet = setsockopt(connfd, SOL_SOCKET, SO_SNDBUF, (void *)&sendBufferSize, sizeof(sendBufferSize));
	if(-1 == nRet){
		ELog("set socket optian error: %s (errno :%d)",strerror(errno),errno);
		close(connfd);
		return client_size;
	}
	int recvBufferSize = 32 * 1024; // 设置接收缓冲区大小
	nRet = setsockopt(connfd, SOL_SOCKET, SO_RCVBUF, (void *)&recvBufferSize, sizeof(recvBufferSize));
	if(-1 == nRet){
		ELog("set socket optian error: %s (errno :%d)",strerror(errno),errno);
		close(connfd);
		return client_size;
	}

	//
	struct timeval recv_timeout; //	设置接收数据超时时间
	recv_timeout.tv_sec = 1;
	recv_timeout.tv_usec = 0;
	nRet = setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&recv_timeout, sizeof(recv_timeout));
	if(-1 == nRet){
		ELog("set socket optian error: %s (errno :%d)",strerror(errno),errno);
		close(connfd);
		return client_size;
	}
	struct timeval send_timeout; //	设置发送数据超时时间
	send_timeout.tv_sec = 3;
	send_timeout.tv_usec = 0;
	nRet = setsockopt(connfd, SOL_SOCKET, SO_SNDTIMEO, (void *)&send_timeout, sizeof(send_timeout));
	if(-1 == nRet){
		ELog("set socket optian error: %s (errno :%d)",strerror(errno),errno);
		close(connfd);
		return client_size;
	}

	//
	int keepAlive = 1; // 开启keepalive属性
	nRet = setsockopt(connfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
	if(-1 == nRet){
		ELog("set socket optian error: %s (errno :%d)",strerror(errno),errno);
		close(connfd);
		return client_size;
	}
	int keepIdle = 30; // 如该连接在60秒内没有任何数据往来,则进行探测
	nRet = setsockopt(connfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepIdle, sizeof(keepIdle));
	if(-1 == nRet){
		ELog("set socket optian error: %s (errno :%d)",strerror(errno),errno);
		close(connfd);
		return client_size;
	}
	int keepInterval = 5; // 探测时发包的时间间隔为5 秒
	nRet = setsockopt(connfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepInterval, sizeof(keepInterval));
	if(-1 == nRet){
		ELog("set socket optian error: %s (errno :%d)",strerror(errno),errno);
		close(connfd);
		return client_size;
	}
	int keepCount = 10; // 探测尝试的次数.如果第1次探测包就收到响应了,则后2次的不再发.
	nRet = setsockopt(connfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepCount, sizeof(keepCount));
	if(-1 == nRet){
		ELog("set socket optian error: %s (errno :%d)",strerror(errno),errno);
		close(connfd);
		return client_size;
	}

	addr_info[empty_index].client_sock = connfd;
	memcpy(&addr_info[empty_index].client_addr,&client_addr,sizeof(client_addr));
	client_size += 0x1;
	ILog("client success connect %d,array index:%d",connfd,empty_index);

	return client_size;
}

/**
 * @brief		generate network frame
 * @param		data
 * @param		size_byte
 * @param		type
 * @return		int		data size,byte
 */
static int gen_net_frm(void* data,int size_byte,int type)
{
	int pos = 0x0;
	memcpy(gen_temp_buffer,data,size_byte);
	//head-2Byte
	*(short*)(data + pos) = NET_FRM_HEAD;pos += sizeof(short);
	//payload type-2Byte
	*(short*)(data + pos) = (short)type;pos += sizeof(short);
	//length-4Byte,only payload data size
	*(int*)(data + pos) = size_byte;pos += sizeof(int);
	//index-4Byte
	if(data_net_frm == type){
		*(unsigned int*)(data + pos) = ++gen_net_frm_data_index;pos += sizeof(int);
	}else if(control_net_frm == type){
		*(unsigned int*)(data + pos) = ++gen_net_frm_control_index;pos += sizeof(int);
	}else
		return DATA_ERROR;
	//remain-4Byte
	*(int*)(data + pos) = 0xFFFFFFFF;pos += sizeof(int);
	//payload
	if(FRM_BUFFER_SIZE < (size_byte + pos + sizeof(short) * 2))
		return 0x0;
	else{
		memcpy((char*)(data + pos),gen_temp_buffer,size_byte);pos += size_byte;
	}
	//crc
	*(short*)(data + pos) = NET_REMAIN_DEFAULT;pos += sizeof(short);
	//end
	*(short*)(data + pos) = NET_FRM_END;pos += sizeof(short);

	return pos;
}

#define SAVE_FRM_BUFFER_SIZE (int)(FRM_BUFFER_SIZE * 0x4)
static char net_save_buffer[SAVE_FRM_BUFFER_SIZE] = {0x0};
static unsigned int save_buffer_data_size = 0x0;
static unsigned int save_buffer_ptr_write_pos = 0x0;
static unsigned int save_buffer_ptr_read_pos = 0x0;
/**
 * @brief		write/read buffer
 */
static int write_tmp_buffer(void* data,int size_byte)
{
	if(NULL == data || 0x0 > size_byte)
		return DATA_ERROR;
	//
	if(size_byte > (SAVE_FRM_BUFFER_SIZE - save_buffer_data_size)){
		ELog("no enough memory size to write data");
		return DATA_ERROR;
	}
	//
	int remain = SAVE_FRM_BUFFER_SIZE - save_buffer_ptr_write_pos;
	if(remain >= size_byte)
	{
		save_buffer_data_size += size_byte;
		//
		memcpy((void*)(net_save_buffer + save_buffer_ptr_write_pos),data,size_byte);
		save_buffer_ptr_write_pos += size_byte;
		save_buffer_ptr_write_pos %= SAVE_FRM_BUFFER_SIZE;
	}else{
		save_buffer_data_size += size_byte;
		//
		memcpy((void*)(net_save_buffer + save_buffer_ptr_write_pos),data,remain);
		save_buffer_ptr_write_pos += remain;
		save_buffer_ptr_write_pos %= SAVE_FRM_BUFFER_SIZE;
		memcpy((void*)(net_save_buffer + save_buffer_ptr_write_pos),(void*)(data + remain),(size_byte - remain));
		save_buffer_ptr_write_pos += (size_byte - remain);
		save_buffer_ptr_write_pos %= SAVE_FRM_BUFFER_SIZE;
	}

	return size_byte;
}
static int read_tmp_buffer(void* buffer,int size_byte)
{
	if(NULL == buffer)
		return DATA_ERROR;
	if(size_byte > save_buffer_data_size)
		return 0x0;

	int remain = SAVE_FRM_BUFFER_SIZE - save_buffer_ptr_read_pos;
	if(remain >= size_byte)
	{
		save_buffer_data_size -= size_byte;
		//
		memcpy(buffer,(void*)(net_save_buffer + save_buffer_ptr_read_pos),size_byte);
		save_buffer_ptr_read_pos += size_byte;
		save_buffer_ptr_read_pos %= SAVE_FRM_BUFFER_SIZE;
	}else{
		save_buffer_data_size -= size_byte;
		//
		memcpy(buffer,(void*)(net_save_buffer + save_buffer_ptr_read_pos),remain);
		save_buffer_ptr_read_pos += remain;
		save_buffer_ptr_read_pos %= SAVE_FRM_BUFFER_SIZE;
		memcpy((void*)(buffer + remain),(void*)(net_save_buffer + save_buffer_ptr_read_pos),(size_byte - remain));
		save_buffer_ptr_read_pos += (size_byte - remain);
		save_buffer_ptr_read_pos %= SAVE_FRM_BUFFER_SIZE;
	}

	return size_byte;
}
static inline unsigned int data_size_tmp(){
	return save_buffer_data_size;
}
static inline unsigned int ptr_read_pos_tmp(){
	return save_buffer_ptr_read_pos;
}
static inline unsigned int ptr_write_pos_tmp(){
	return save_buffer_ptr_write_pos;
}
static inline unsigned int get_frm_payload_length()
{
	int payload_length = 0x0;

	char temp_array[4] = {0x0};
	void* data = (void*)&net_save_buffer[0];
	void* target = NULL;
	int temp_pos = ptr_read_pos_tmp();
	//head + type
	temp_pos += sizeof(short) + sizeof(short);
	temp_pos %= SAVE_FRM_BUFFER_SIZE;
	//
	int temp_remain = SAVE_FRM_BUFFER_SIZE - temp_pos;
	if(sizeof(int) <= temp_remain){
		target = data + temp_pos;
		payload_length = *(int*)(target);
	}else{
		target = data + temp_pos;
		memcpy(temp_array,target,temp_remain);
		//
		temp_pos += temp_remain;
		temp_pos %= SAVE_FRM_BUFFER_SIZE;
		//
		target = data + temp_pos;
		memcpy((void*)(&temp_array[temp_remain]),target,sizeof(int) - temp_remain);
		//
		temp_pos += temp_remain;
		temp_pos %= SAVE_FRM_BUFFER_SIZE;
		//
		payload_length = *(int*)(&temp_array[0]);
	}

	return payload_length;
}

/**
 * @brief		check network frame valid status
 * @param		origin		原始数据,并且用户负载数据在解析过后也通过此变量返回
 * @param		size_byte
 * @return		int		user payload data size,byte
 */
static int check_net_frm_valid(void* origin,int size_byte)
{
	if(NULL == origin)
		return DATA_ERROR;
	if(0x0 >= size_byte || FRM_BUFFER_SIZE < size_byte)
		return DATA_ERROR;

	int temp_rt = 0x0;
	//write data
	write_tmp_buffer(origin,size_byte);

	//
	int pos = 0x0;
	//head
	pos += sizeof(short);
	//frame type
	pos += sizeof(short);
	//payload length
	int payload_length = get_frm_payload_length();
	pos += sizeof(int);
	if(pos > data_size_tmp())
		return DATA_ERROR;
	//index
	pos += sizeof(int);
	//remain
	pos += sizeof(int);
	//payload
	pos += payload_length;
	//crc
	pos += sizeof(short);
	//end
	pos += sizeof(short);

	//read data
	if(pos <= data_size_tmp()){
		temp_rt = read_tmp_buffer(origin,pos);
		if(pos != temp_rt)
			return DATA_ERROR;
		else
			return pos;
	}else{
		return 0x0;
	}

	return DATA_ERROR;
}

/**
 * @brief		extract network frame
 * @param		origin		原始数据,并且用户负载数据在解析过后也通过此变量返回
 * @param		size_byte
 * @param		type	frame type
 * @return		int		user payload data size,byte
 */
static int extract_net_frm(void* origin,int size_byte,int* type)
{
	if(NULL == origin || NULL == type)
		return DATA_ERROR;
	if(0x0 >= size_byte || FRM_BUFFER_SIZE < size_byte)
		return DATA_ERROR;
	memset(extract_temp_buffer,0x0,FRM_BUFFER_SIZE);
	memcpy(extract_temp_buffer,origin,size_byte);

	void* data = &extract_temp_buffer[0];
	int pos = 0x0;
	//head
	unsigned short frm_head = *(unsigned short*)(data + pos);
	pos += sizeof(short);
	if(NET_FRM_HEAD != frm_head)
		return DATA_ERROR;
	//frame type
	*type = *(short*)(data + pos);
	pos += sizeof(short);
	//payload length
	int payload_length = *(int*)(data + pos);
	pos += sizeof(int);
	//index
	pos += sizeof(int);
	//remain
	pos += sizeof(int);
	//payload
	memcpy(origin,(char*)(data + pos),payload_length);
	pos += payload_length;
	//crc
	unsigned short crc_value = *(unsigned short*)(data + pos);
	pos += sizeof(short);
	if(NET_REMAIN_DEFAULT != crc_value)
		return DATA_ERROR;
	//end
	unsigned short frm_end = *(unsigned short*)(data + pos);
	pos += sizeof(short);
	if(NET_FRM_END != frm_end)
		return DATA_ERROR;
	//
	if(pos != size_byte){
		ELog("frame data invalid");
		return DATA_ERROR;
	}

	return payload_length;
}

/**
 * @brief		parse command network frame
 * @param		data
 * @param		size_byte
 * @param		type	frame type
 * @return		int
 * @retval		0x0	success
 * 				other	failed
 */
static int handler_control_frm(void* data,int size_byte,int type)
{
	if(NULL == data)
		return RT_ERROR;
	if(0x0 >= size_byte)
		return RT_ERROR;
	if(control_net_frm != type)
		return RT_ERROR;

	int flag = 0x0;
	int pos = 0x0;
	unsigned int cmd = 0x0,var_size = 0x0;
	char temp_buffer[1024] = {0x0};
	int loop_rt = 0x0;
	bool loop_error = false;
	static char local_resp_data[FRM_BUFFER_SIZE] = {0x0};
	static net_resp_info_s resp_msg;
	int fpga_reconfig_step = 0x0;
	int temp_ad_id = 0x0;
	int temp_op_cmd_value = 0x0;
	//
	do{
		flag = 0x0;
		//cmd code
		cmd = *(unsigned int*)(data + pos);
		pos += sizeof(unsigned int);
		//cmd var length
		var_size = *(unsigned int*)(data + pos);
		pos += sizeof(unsigned int);
		//
		memset(temp_buffer,0x0,1024);
		switch(cmd)
		{
		case reg_write_cmd:
#if SIMULATE_CMD
			{
				//reg_id
				int reg_id = *(int*)(data + pos);
				//reg value
				int reg_value = *(int*)(data + pos + sizeof(int));
				DLog("cmd:%d,reg_id:%d,reg_value:%d",cmd,reg_id,reg_value);
				reg_write(reg_id,reg_value);
			}
#endif
			break;
		case reg_read_cmd:
#if SIMULATE_CMD
			{
				//reg_id
				int reg_id = *(int*)(data + pos);
				//reg value
				int reg_value = 0x0;
				int reg_out = 0x0;
				reg_read(reg_id,&reg_value,&reg_out);
				if(!reg_out){
					WLog("read reg id:%d value failed",reg_id);
					break;
				}
				//
				resp_msg.cmd = reg_read_resp;
				int pos = 0x0;
				*(unsigned int*)(&local_resp_data[0] + pos) = reg_id;
				pos += sizeof(int);
				*(int*)(&local_resp_data[0] + pos) = reg_value;
				pos += sizeof(int);
				resp_msg.param_size = pos;
				resp_msg.param_ptr = &local_resp_data[0];
				//
				net_resp_msg(&resp_msg,client_info);
			}
#endif
			break;
		case in_rf_freq:
#if SERVICE_CMD
			temp_ad_id = *(int*)(data + pos);
			DLog("cmd:%d,arg:%lld,ad_id:%d",cmd,*(int64_t*)(data + pos + sizeof(int)),temp_ad_id);
			{
				loop_rt = ad9361_set_freq(temp_ad_id,ad_rx,*(int64_t*)(data + pos + sizeof(int)));
				if(loop_rt){
					//loop_error = true;
					flag = RT_ERROR;
					break;
				}
			}
#endif
			break;
		case out_rf_freq:
#if SERVICE_CMD
			temp_ad_id = *(int*)(data + pos);
			DLog("cmd:%d,arg:%lld,ad_id:%d",cmd,*(int64_t*)(data + pos + sizeof(int)),temp_ad_id);
			{
				loop_rt = ad9361_set_freq(temp_ad_id,ad_tx,*(int64_t*)(data + pos + sizeof(int)));
				if(loop_rt){
					//loop_error = true;
					flag = RT_ERROR;
					break;
				}
			}
#endif
			break;
		case in_rf_bd:
#if SERVICE_CMD
			temp_ad_id = *(int*)(data + pos);
			DLog("cmd:%d,arg:%lld,ad_id:%d",cmd,*(int64_t*)(data + pos + sizeof(int)),temp_ad_id);
			{
				loop_rt = ad9361_set_bd(temp_ad_id,ad_rx,*(int64_t*)(data + pos + sizeof(int)));
				if(loop_rt){
					//loop_error = true;
					flag = RT_ERROR;
					break;
				}
			}
#endif
			break;
		case out_rf_bd:
#if SERVICE_CMD
			temp_ad_id = *(int*)(data + pos);
			DLog("cmd:%d,arg:%lld,ad_id:%d",cmd,*(int64_t*)(data + pos + sizeof(int)),temp_ad_id);
			{
				loop_rt = ad9361_set_bd(temp_ad_id,ad_tx,*(int64_t*)(data + pos + sizeof(int)));
				if(loop_rt){
					//loop_error = true;
					flag = RT_ERROR;
					break;
				}
			}
#endif
			break;
		case agc_rf_value:
#if SERVICE_CMD
			temp_ad_id = *(int*)(data + pos);
			DLog("cmd:%d,arg:%d,ad_id:%d",cmd,*(int*)(data + pos + sizeof(int)),temp_ad_id);
			//
			{
				loop_rt = ad9361_set_rxgain(temp_ad_id,*(int*)(data + pos + sizeof(int)));
				if(loop_rt){
					//loop_error = true;
					flag = RT_ERROR;
					break;
				}
			}
#endif
			break;
		case agc_rf_mode:
#if SERVICE_CMD
			temp_ad_id = *(int*)(data + pos);
			DLog("cmd:%d,arg:%d,ad_id:%d",cmd,*(int*)(data + pos + sizeof(int)),temp_ad_id);
			//
			{
				loop_rt = ad9361_set_rxgain_mod(temp_ad_id,*(int*)(data + pos + sizeof(int)));
				if(loop_rt){
					//loop_error = true;
					flag = RT_ERROR;
					break;
				}
			}
#endif
			break;
		case agc_rf_att:
#if SERVICE_CMD
			temp_ad_id = *(int*)(data + pos);
			DLog("cmd:%d,arg:%d,ad_id:%d",cmd,*(int*)(data + pos + sizeof(int)),temp_ad_id);
			//
			{
				loop_rt = ad9361_set_att(temp_ad_id,*(int*)(data + pos + sizeof(int)));
				if(loop_rt){
					//loop_error = true;
					flag = RT_ERROR;
					break;
				}
			}
#endif
			break;
		case rf_value_read:
#if SERVICE_CMD
			temp_ad_id = *(int*)(data + pos);
			pos += sizeof(int);
			temp_op_cmd_value = *(int*)(data + pos);
			DLog("cmd:%d,op_cmd:%d,ad_id:%d",cmd,temp_op_cmd_value,temp_ad_id);
			//
			{
				int temp_read_flag = 0x0;
				double temp_read_value = 0x0;
				//
				int temp_get_value = 0x0;
				int64_t temp_value_64 = 0x0;
				uint temp_value_uint = 0x0;
				int temp_value_int = 0x0;
				//read target ad data
				switch(temp_op_cmd_value)
				{
				case in_rf_freq:
					temp_get_value = AD_getFrequency(temp_ad_id,D_RX,&temp_value_64,true);
					if(temp_get_value){
						temp_read_flag = 0x2;
					}else{
						temp_read_flag = 0x1;
						temp_read_value = temp_value_64;
					}
					break;
				case out_rf_freq:
					temp_get_value = AD_getFrequency(temp_ad_id,D_TX,&temp_value_64,true);
					if(temp_get_value){
						temp_read_flag = 0x2;
					}else{
						temp_read_flag = 0x1;
						temp_read_value = temp_value_64;
					}
					break;
				case in_rf_bd:
					temp_get_value = AD_getBandwidth(temp_ad_id,D_RX,&temp_value_uint,true);
					if(temp_get_value){
						temp_read_flag = 0x2;
					}else{
						temp_read_flag = 0x1;
						temp_read_value = temp_value_uint;
					}
					break;
				case out_rf_bd:
					temp_get_value = AD_getBandwidth(temp_ad_id,D_TX,&temp_value_uint,true);
					if(temp_get_value){
						temp_read_flag = 0x2;
					}else{
						temp_read_flag = 0x1;
						temp_read_value = temp_value_uint;
					}
					break;
				case agc_rf_value:
					temp_get_value = AD_getGCValue(temp_ad_id,D_RX,&temp_value_uint);
					if(temp_get_value){
						temp_read_flag = 0x2;
					}else{
						temp_read_flag = 0x1;
						temp_read_value = temp_value_uint;
					}
					break;
				case agc_rf_mode:
					temp_get_value = AD_getGCMode(temp_ad_id,&temp_value_int);
					if(temp_get_value){
						temp_read_flag = 0x2;
					}else{
						temp_read_flag = 0x1;
						temp_read_value = temp_value_int;
					}
					break;
				case agc_rf_att:
					temp_get_value = AD_getATTValue(temp_ad_id,D_TX,&temp_value_uint);
					if(temp_get_value){
						temp_read_flag = 0x2;
					}else{
						temp_read_flag = 0x1;
						temp_read_value = temp_value_uint;
					}
					break;
				default:
					break;
				}
				//resp value data
				int pos = 0x0;
				*(unsigned int*)(&local_resp_data[0] + pos) = temp_ad_id;
				pos += sizeof(int);
				*(int*)(&local_resp_data[0] + pos) = temp_op_cmd_value;
				pos += sizeof(int);
				*(int*)(&local_resp_data[0] + pos) = temp_read_flag;
				pos += sizeof(int);
				*(double*)(&local_resp_data[0] + pos) = temp_read_value;
				pos += sizeof(double);
				//
				resp_msg.cmd = reg_read_resp;
				resp_msg.param_size = pos;
				resp_msg.param_ptr = &local_resp_data[0];
				//
				net_resp_msg(&resp_msg,client_info);
			}
#endif
			break;
		case fpga_bit_path:
#if SERVICE_CMD
			memset(temp_buffer,0x0,sizeof(temp_buffer));
			memcpy(temp_buffer,(char*)(data + pos),var_size);
			DLog("cmd:%d,arg:%s",cmd,temp_buffer);
			{
				int temp_status = 0x0;
				//
				if(simu_exe_running == simu_exe_flag()){
					WLog("simulate application is running");
					flag = RT_ERROR;
					temp_status = RT_ERROR;
				}else{
					temp_status = ++fpga_reconfig_step;
				}
				//
				resp_msg.cmd = fpga_reconfig_resp;
				int pos = 0x0;
				*(int*)(&local_resp_data[0] + pos) = temp_status;
				pos += sizeof(int);
				resp_msg.param_size = pos;
				resp_msg.param_ptr = &local_resp_data[0];
				//
				net_resp_msg(&resp_msg,client_info);
			}
			if(flag)
				break;
			{
				//
				int temp_status = 0x0;
				loop_rt = fpga_reconfigure(temp_buffer);
				if(loop_rt){
					loop_error = true;
					flag = RT_ERROR;
					temp_status = RT_ERROR;
				}else{
					temp_status = ++fpga_reconfig_step;
					ILog("reconfigure fpga into bit:%s success",temp_buffer);
				}
				//
				resp_msg.cmd = fpga_reconfig_resp;
				int pos = 0x0;
				*(int*)(&local_resp_data[0] + pos) = temp_status;
				pos += sizeof(int);
				resp_msg.param_size = pos;
				resp_msg.param_ptr = &local_resp_data[0];
				//
				net_resp_msg(&resp_msg,client_info);
			}
			if(flag)
				break;
			{
				usleep(100000);
				//
				int temp_status = 0x0;
				loop_rt = ad_reset();
				if(loop_rt){
					loop_error = true;
					flag = RT_ERROR;
					temp_status = RT_ERROR;
				}else{
					temp_status = ++fpga_reconfig_step;
					ILog("ad reset success");
				}
				//
				resp_msg.cmd = fpga_reconfig_resp;
				int pos = 0x0;
				*(int*)(&local_resp_data[0] + pos) = temp_status;
				pos += sizeof(int);
				resp_msg.param_size = pos;
				resp_msg.param_ptr = &local_resp_data[0];
				//
				net_resp_msg(&resp_msg,client_info);
			}
			if(flag)
				break;
			{
				//
				resp_msg.cmd = fpga_reconfig_resp;
				int pos = 0x0;
				*(int*)(&local_resp_data[0] + pos) = 0xFFFF;
				pos += sizeof(int);
				resp_msg.param_size = pos;
				resp_msg.param_ptr = &local_resp_data[0];
				//
				net_resp_msg(&resp_msg,client_info);
			}
#endif
			break;
		case simu_exe_start:
#if SERVICE_CMD
			memset(temp_buffer,0x0,sizeof(temp_buffer));
			memcpy(temp_buffer,(char*)(data + pos),var_size);
			DLog("cmd:%d,arg:%s",cmd,temp_buffer);
			{
				int temp_status = 0x0;
				//
				int temp_rt = start_simu_exe(temp_buffer);
				if(temp_rt){
					temp_status = simu_resp_child_process_failed;
				}else{
					temp_status = simu_resp_operate_success;
				}
				//
				resp_msg.cmd = simu_exe_resp;
				int pos = 0x0;
				*(int*)(&local_resp_data[0] + pos) = temp_status;
				pos += sizeof(int);
				resp_msg.param_size = pos;
				resp_msg.param_ptr = &local_resp_data[0];
				//
				net_resp_msg(&resp_msg,client_info);
			}
#endif
			break;
		case simu_exe_stop:
#if SERVICE_CMD
			memset(temp_buffer,0x0,sizeof(temp_buffer));
			memcpy(temp_buffer,(char*)(data + pos),var_size);
			DLog("cmd:%d,arg:%s",cmd,temp_buffer);
			{
				int temp_status = 0x0;
				//
				int temp_rt = stop_simu_exe(temp_buffer);
				if(temp_rt){
					temp_status = simu_resp_child_process_failed;
				}else{
					temp_status = simu_resp_operate_success;
				}
				//
				resp_msg.cmd = simu_exe_resp;
				int pos = 0x0;
				*(int*)(&local_resp_data[0] + pos) = temp_status;
				pos += sizeof(int);
				resp_msg.param_size = pos;
				resp_msg.param_ptr = &local_resp_data[0];
				//
				net_resp_msg(&resp_msg,client_info);
			}
#endif
			break;
		case simu_exe_restart:
#if SERVICE_CMD
			memset(temp_buffer,0x0,sizeof(temp_buffer));
			memcpy(temp_buffer,(char*)(data + pos),var_size);
			DLog("cmd:%d,arg:%s",cmd,temp_buffer);
			{
				int temp_status = 0x0;
				//
				int temp_rt = restart_simu_exe(temp_buffer);
				if(temp_rt){
					temp_status = simu_resp_child_process_failed;
				}else{
					temp_status = simu_resp_operate_success;
				}
				//
				resp_msg.cmd = simu_exe_resp;
				int pos = 0x0;
				*(int*)(&local_resp_data[0] + pos) = temp_status;
				pos += sizeof(int);
				resp_msg.param_size = pos;
				resp_msg.param_ptr = &local_resp_data[0];
				//
				net_resp_msg(&resp_msg,client_info);
			}
#endif
			break;
		default:
			ELog("invalid cmd code:%x",cmd);
			break;
		}
		//
		pos += var_size;
	}while(pos != size_byte && !loop_error);

	return flag;
}

/**
 * @brief		parse command network frame
 * @param		data
 * @param		size_byte
 * @param		type	frame type
 * @return		int
 * @retval		0x0	success
 * 				other	failed
 */
static void net_resp_msg(net_resp_info_p resp_info,client_addr_info_ptr client_info)
{
	if(NULL == resp_info || NULL == client_info){
		return;
	}

	int pos = 0x0;
	static char local_resp_data[FRM_BUFFER_SIZE] = {0x0};

	//control payload
	*(unsigned int*)(&local_resp_data[0] + pos) = resp_info->cmd;
	pos += sizeof(int);
	*(unsigned int*)(&local_resp_data[0] + pos) = resp_info->param_size;
	pos += sizeof(int);
	memcpy((void*)(&local_resp_data[0] + pos),resp_info->param_ptr,resp_info->param_size);
	pos += resp_info->param_size;

	//
	static unsigned int resp_msg_index = 0x0;
	int size_byte = pos;
	static char resp_data[FRM_BUFFER_SIZE] = {0x0};
	void* data = &resp_data[0];
	pos = 0x0;
	//head-2Byte
	*(short*)(data + pos) = NET_FRM_HEAD;pos += sizeof(short);
	//payload type-2Byte
	*(short*)(data + pos) = (short)back_control_net_frm;pos += sizeof(short);
	//length-4Byte,only payload data size
	*(int*)(data + pos) = size_byte;pos += sizeof(int);
	//index-4Byte
	*(unsigned int*)(data + pos) = ++resp_msg_index;pos += sizeof(int);
	//remain-4Byte
	*(int*)(data + pos) = 0xFFFFFFFF;pos += sizeof(int);
	//payload
	if(FRM_BUFFER_SIZE < (size_byte + pos + sizeof(short) * 2))
		return;
	else{
		memcpy((char*)(data + pos),local_resp_data,size_byte);pos += size_byte;
	}
	//crc
	*(short*)(data + pos) = NET_REMAIN_DEFAULT;pos += sizeof(short);
	//end
	*(short*)(data + pos) = NET_FRM_END;pos += sizeof(short);

	//send resp data
	int write_byte = 0x0;
	for (int i = 0x0;i < client_max_size;++i)
	{
		if (0x0 >= client_info[i].client_sock)
			continue;

		//send data
		write_byte = send(client_info[i].client_sock,resp_data,pos,0);
		if (pos != write_byte){
			ELog("send response msg to client[%d] failed.data %d byte",client_info[i].client_sock,write_byte);
		}
	}
}

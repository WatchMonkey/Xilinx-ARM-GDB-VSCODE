/**
 * @file LogImpl.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "LogHeader.h"

#include <stdlib.h>
#include <unistd.h>
//#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
//#include <errno.h>
//#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

//**************************************宏定义**************************************//
#define RT_ERROR ((int)__LINE__)
#define NET_DF_PORT 9527
#define NET_DF_ADDR "127.0.0.1"
#define NET_LOG_MAX_LENGTH 2048
#define NET_LOG_VAR_LENGTH_MAX (NET_LOG_MAX_LENGTH - 256)
#define DEFAULT_LOG_DIR "/home/root/LogFile"

//**************************************类型声明**************************************//
/*!
 * @struct		功能配置信息
 */
typedef struct setInfo{
	Info_Level_t level;//!<级别信息
	bool debug_enable;//!<打开调试信息记录
	bool net_enable;//!<打开网络日志发送
	bool debug_stdout;//!<调试信息输出到stdout
	bool stdout_enable;//!<所有信息使能输出到stdout
	bool contain_date;//!<日志信息包含日期
	bool contain_position;//!<日志信息包含位置信息
	bool force_flush;//!<日志信息每次都进行强制刷新
}setInfo_t;

/*!
 * @struct		日志初始化状态信息
 */
/*
typedef struct initInfo{
	pthread_mutex_t init_mutex;
	int init_flag;
}initInfo_t;
*/

//**************************************默认设置**************************************//
//! 是否将所有的日志信息同步输出到标准输出中
static bool Send_stdout = false;
//!	是否记录调试信息
static bool Debug_Enable = false;
//!	是否使用网络发送日志信息
static bool Net_Enable = false;
//!	是否显示调试信息到标准输出中
static bool Debug_Show = true;
//!	是否强制flush日志文件数据流
static bool ForceFlush = false;
//!	日志信息是否包含日期信息
static bool Contain_Date = false;
//!	日志信息是否包含位置信息
static bool Contain_Position = false;
//! 最低日志记录/显示级别
static Info_Level_t Log_level = f_Info;

//**************************************全局参数**************************************//
//! 日志文件指针
static FILE* Log_File = NULL;
//! 互斥量
static pthread_mutex_t Log_localmutex;
static pthread_mutex_t Log_datamutex;
//!	初始化结果
static unsigned int Flag_init_success = 0x0;
static pthread_mutex_t Log_initmutex = PTHREAD_MUTEX_INITIALIZER;
//!	初始化状态
static bool Flag_init_status = false;
//!	数据buffer,unused
static void* Net_Log_Buffer = NULL;
//!	网络地址
static char Net_Addr[0x40] = {0x0};
//! 网络端口
static int Net_Port = NET_DF_PORT;
//!	socket端口
static int Net_UDPSocket = -1;
//!	远端地址信息
static struct sockaddr_in Net_Remote;
//!	初始化状态信息
/*
static initInfo_t Flag_init_Info = {
		.init_flag = 0x0,
		.init_mutex = PTHREAD_MUTEX_INITIALIZER
};
*/

//**************************************函数接口**************************************//
/*!
    @brief  设置是否所有的日志数据全部都输出到标准输出流中
    @param	value
    @retval void
*/
void LogShowAll(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Send_stdout = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  设置日志信息的过滤级别
    @param	Info_Level_t,信息输出级别
    @retval void
*/
void LogFilterLevel(Info_Level_t value)
{
    pthread_mutex_lock(&Log_localmutex);
    Log_level = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  设置日志信息是否包含日期信息
    @param	bool,状态
    @retval void
*/
void LogContainDate(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Contain_Date = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  设置日志信息是否包含位置信息
    @param	bool,状态
    @retval void
*/
void LogContainPosition(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Contain_Position = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  设置日志是否记录debug信息
    @param	bool,状态
    @retval void
*/
void LogEnableDebug(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Debug_Enable = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  设置日志是否在标准输出中显示debug信息
    @param	bool,状态
    @retval void
*/
void LogShowDebug(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Debug_Show = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  设置日志是否强制刷新日志文件数据流
    @param	bool,状态
    @retval void
*/
void LogForceFlush(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    ForceFlush = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  设置日志是否启动网络功能
    @param	bool,状态
    @retval void
*/
void LogEnableNet(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Net_Enable = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  设置日志是否启动网络功能
    @param	char*,网络地址
    @param	int:网络端口
    @retval void
*/
void LogSetNet(char* ip,int port)
{
	if(NULL == ip)
		return;
	if(0x0 >= port || 65535 <= port)
		return;
    pthread_mutex_lock(&Log_localmutex);
    memset(&Net_Addr,0x0,sizeof(Net_Addr));
    sprintf(Net_Addr,"%s",ip);
    Net_Port = port;
    pthread_mutex_unlock(&Log_localmutex);
    //set remote ipaddr
	memset(&Net_Remote,0x0,sizeof(Net_Remote));
	Net_Remote.sin_family = AF_INET;
	Net_Remote.sin_port = htons(Net_Port);
	Net_Remote.sin_addr.s_addr = inet_addr(Net_Addr);
}

/*!
 * *************************************内部功能函数**************************************
 */

/*!
 * @brief		获取进程名称
 * @return		char*	进程名称
 * @retval		null	获取失败
 */
static char* detect_process_name()
{
	char *temp_proc_name = NULL;
	char temp_proc_fullname[0x100] = {0x0};
	if(readlink("/proc/self/exe",temp_proc_fullname,0x100) <= 0x0)
		return temp_proc_name;
	//get name
	temp_proc_name = strrchr(temp_proc_fullname,'/');
	if(NULL == temp_proc_name){
		return temp_proc_name;
	}else{
		return ++temp_proc_name;
	}
}

/*!
 * *************************************功能接口**************************************
 */

/*!
    @brief  刷新缓冲区
    @retval void
*/
void LogFFlush()
{
	//刷新缓冲区
	if(NULL != Log_File){
		fflush(Log_File);
		fsync(fileno(Log_File));
	}
}

/*!
 * *************************************初始化/释放函数**************************************
 */

/*!
    @brief  初始化日志模块
    @param	char*,日志文件标识信息
    @retval 0x0-success
    		other-failed
	@return int
*/
int InitializeLog(char* file_label)
{
	int temp_status = 0x0;
	int temp_logfp = 0x0;
	char *temp_procname = detect_process_name();
	char temp_filename[400] = {0x0};

	//detect initialize status
	pthread_mutex_lock(&Log_initmutex);
	if(Flag_init_status){
		pthread_mutex_unlock(&Log_initmutex);
		return temp_status;
	}

	//init mutex
    if(0x0 == pthread_mutex_init(&Log_localmutex,NULL) || 0x0 == pthread_mutex_init(&Log_datamutex,NULL)){
        Flag_init_success |= 0x1;
    }else{
    	Flag_init_status = false;
    	temp_status = RT_ERROR;
		goto TAG_A;
    }

    //alloc memory
    Net_Log_Buffer = calloc(NET_LOG_MAX_LENGTH,0x1);
    if(Net_Log_Buffer != NULL){
        Flag_init_success |= 0x2;
    }else{
    	Flag_init_status = false;
    	temp_status = RT_ERROR;
		goto TAG_A;
    }

    //detect log folder
    if(0x0 != access(DEFAULT_LOG_DIR,W_OK)){
    	//create folder
    	if(0x0 != mkdir(DEFAULT_LOG_DIR,0755))
    	{
    		if(EEXIST == errno){
    			//nothing
    			fprintf(stderr,"create %s folder failed.already exist.\n",DEFAULT_LOG_DIR);
    			Flag_init_success |= 0x4;
    		}else{
    			//
				fprintf(stderr,"create %s folder failed:%d,%s\n",DEFAULT_LOG_DIR,errno,strerror(errno));
				temp_status = RT_ERROR;
				goto TAG_A;
    		}
    	}else{
    		Flag_init_success |= 0x4;
    	}
    }
    //generate file path
    if(NULL == temp_procname){
    	if(NULL == file_label){
    		sprintf(temp_filename,"/null-%u-null.log",getpid());
    	}else{
    		sprintf(temp_filename,"/null-%u-%s.log",getpid(),file_label);
    	}
    }else{
    	if(NULL == file_label){
    		sprintf(temp_filename,"/%s-%u-null.log",temp_procname,getpid());
    	}else{
    		sprintf(temp_filename,"/%s-%u-%s.log",temp_procname,getpid(),file_label);
    	}
    }
    char temp_logfilepath[512] = {0x0};
    strcat(temp_logfilepath,DEFAULT_LOG_DIR);
    strcat(temp_logfilepath,temp_filename);
    //open file
    Log_File = fopen(temp_logfilepath,"w+");
    if(Log_File != NULL){
    	Flag_init_success |= 0x8;
    }else{
    	Flag_init_status = false;
    	temp_status = RT_ERROR;
		goto TAG_A;
    }

    //set remote ipaddr
	memset(&Net_Remote,0x0,sizeof(Net_Remote));
	Net_Remote.sin_family = AF_INET;
	Net_Remote.sin_port = htons(NET_DF_PORT);
	Net_Remote.sin_addr.s_addr = inet_addr(NET_DF_ADDR);
	//create socket
	Net_UDPSocket = socket(AF_INET,SOCK_DGRAM,0x0);
	if(-1 == Net_UDPSocket){
		Flag_init_status = false;
		temp_status = RT_ERROR;
		goto TAG_A;
	}else{
		Flag_init_success |= 0x10;
	}

    //default set
    Send_stdout = false;
    Debug_Enable= false;
    Debug_Show = true;
    ForceFlush = false;
    Contain_Date = false;
    Contain_Position = true;
    Log_level = f_Info;
    Net_Enable = false;
    //flag
    Flag_init_status = true;

TAG_A:
	pthread_mutex_unlock(&Log_initmutex);
    return temp_status;
}

/*!
 * 	@brief	释放日志模块
 * 	@retval	void
 */
void ReleaseLog()
{
	if(Flag_init_success == 0x0){
		return;
	}

	//close socket
	if(Flag_init_success & 0x10){
		close(Net_UDPSocket);
		Net_UDPSocket = -1;
		memset(&Net_Remote,0x0,sizeof(Net_Remote));
		Flag_init_success &= 0xFFFFFFEF;
	}

	//close file
	if(Flag_init_success & 0x8){
		fflush(Log_File);
		fsync(fileno(Log_File));
		fclose(Log_File);
		Log_File = NULL;
		Flag_init_success &= 0xFFFFFFF7;
	}

	//free memory
	if(Flag_init_success & 0x2){
		free(Net_Log_Buffer);
		Flag_init_success &= 0xFFFFFFFD;
	}

	//release mutex
	if(Flag_init_success & 0x1){
		pthread_mutex_destroy(&Log_localmutex);
		pthread_mutex_destroy(&Log_datamutex);
		Flag_init_success &= 0xFFFFFFFE;
	}
}

/*!
 * *************************************日志输出函数**************************************
 */

/*!
 * @brief		检测是否需要将数据输出到标准输出Stdout中
 * @param		bool:是否输出所有数据到Stdout中
 * @return		bool:输出权限
 * @retval		true--输出到stdout
 * @retval		false--不输出到stdout
 */
static inline bool detectStdout(setInfo_t info,Info_Level_t level)
{
	/*!
	 * @note		如果设置所有信息输出到stdout,那么无条件返回true
	 */
	if(true == info.stdout_enable){
		return true;
	}
	/*!
	 * @note		如果当前信息级别是Error,Runing那么无条件进行信息输出
	 */
	if(f_Error == level || f_Running == level){
		return true;
	}
	/*!
	 * @note		判断当前是否是调试信息级别,并依据调试信息输出权限进行判断
	 */
	if(f_Debug == level && true == info.debug_stdout){
		return true;
	}

	return false;
}

/*!
 * @brief		将数据写入到缓存中
 * @param		char*:缓存
 * @param		int:偏移
 * @param		char*:格式
 * @param		char*:数据
 * @return		int:偏址值
 */
static inline int saveStr(char* ptr,int off,char* format,char* data)
{
	int temp_length = sprintf((char*)(ptr + off),format,data);
	if(0x0 > temp_length){
		return off;
	}else{
		return (off + temp_length);
	}
}
#define macro_saveStr(ptr,off,format,...) {\
		int temp_length = sprintf((char*)(ptr + off),format,##__VA_ARGS__);\
		(0x0 >= temp_length) ? off : (off += temp_length);\
}

/*!
    @brief  记录/输出日志信息
    @param	Info_Level_t,日志级别
    @param	int,日志信息的行数
    @param	const char*,日志信息所在的函数
    @param	const char*,日志信息所在的文件
    @param	const char*,可变参数的格式字符串
    @param	...,可变长参数
    @retval	void
*/
void LogPrint(Info_Level_t level,int line,const char* function,const char* file,const char* format,...)
{
	char* temp_ptr = (char*)Net_Log_Buffer;
	int temp_offset = 0x0;
	int temp_byte = 0x0;
	//判断日志初始化状态
	if(Flag_init_status == false){
		return;
	}

	//判断是否日志信息级别以及调试使能状态
	pthread_mutex_lock(&Log_localmutex);
	Info_Level_t temp_level = Log_level;
	bool temp_debug = Debug_Enable;
	pthread_mutex_unlock(&Log_localmutex);
	//判断日志信息级别是否需要进行记录与显示
	if((int)level < (int)temp_level){
		return;
	}
	//判断是否是调试信息,如果没有使能调试信息,那么不记录/显示调试信息
	if(f_Debug == level && false == temp_debug){
		return;
	}
	//功能设置状态
	pthread_mutex_lock(&Log_localmutex);
	bool temp_stdout = Send_stdout;
	bool temp_date = Contain_Date;
	bool temp_position = Contain_Position;
	bool temp_debugshow = Debug_Show;
	bool temp_forceflush = ForceFlush;
	bool temp_net = Net_Enable;
	pthread_mutex_unlock(&Log_localmutex);
	//功能设置状态统计
	setInfo_t temp_setInfo = {0x0};
	temp_setInfo.level = temp_level;
	temp_setInfo.debug_enable = temp_debug;
	temp_setInfo.net_enable = temp_net;
	temp_setInfo.debug_stdout = temp_debugshow;
	temp_setInfo.contain_date = temp_date;
	temp_setInfo.contain_position = temp_position;
	temp_setInfo.stdout_enable = temp_stdout;
	temp_setInfo.force_flush = temp_forceflush;
	//判断信息是否需要同步输出到stdout中
	bool temp_bool = detectStdout(temp_setInfo,level);

	//计算时间
	struct tm today;
	time_t now;
	now = time(NULL);
	//now = now + 28880; //閸ョ姳璐熼幋鎴滄粦閸︼拷+8閺冭泛灏� 閸氬酣娼扮憰浣烘暏gmtime閸掑棜袙閺冨爼妫� 韫囧懘銆忛幍瀣紣娣囶喗顒滄稉锟芥稉锟� 鐏忚鲸妲�60缁夛拷*60閸掑棝鎸�*8鐏忓繑妞� = 28880
	gmtime_r(&now, &today);
	int y,m,d,h,n,s;
	y = today.tm_year+1900;
	m = today.tm_mon+1;
	d = today.tm_mday;
	h = today.tm_hour;
	n = today.tm_min;
	s = today.tm_sec;
	///互斥锁,输出日志信息
	pthread_mutex_lock(&Log_datamutex);
	//判断信息的输出级别,并且输出级别信息
	switch(level)
	{
	case f_Info:
		fprintf(Log_File,"[%-8s]","Info");
		if(temp_bool)fprintf(stdout,"[%-8s]","Info");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Info");
		break;
	case f_Warning:
		fprintf(Log_File,"[%-8s]","Warn");
		if(temp_bool)fprintf(stdout,"[%-8s]","Warn");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Warn");
		break;
	case f_Error:
		fprintf(Log_File,"[%-8s]","Error");
		fprintf(stdout,"[%-8s]","Error");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Error");
		break;
	case f_Running:
		fprintf(Log_File,"[%-8s]","***Run");
		fprintf(stdout,"[%-8s]","***Run");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","***Run");
		break;
	case f_Debug:
		fprintf(Log_File,"[%-8s]","Debug");
		if(temp_bool)fprintf(stdout,"[%-8s]","Debug");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Debug");
		break;
	default:
		fprintf(Log_File,"[%-8s]","Unknown");
		if(temp_bool)fprintf(stdout,"[%-8s]","Unknown");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Unknown");
		break;
	}
	//输出时间信息
	fprintf(Log_File,"[%02d/%02d/%02d]",h,n,s);
	if(temp_bool){
		fprintf(stdout,"[%02d/%02d/%02d]",h,n,s);
	}
	macro_saveStr(temp_ptr,temp_offset,"[%02d/%02d/%02d]",h,n,s);
	//输出可变长度参数中的数据信息
	if(NULL != format)
	{
		va_list temp_va;
		va_start(temp_va,format);
		vfprintf(Log_File,format,temp_va);
		if(temp_bool){
			vfprintf(stdout,format,temp_va);
		}
		temp_byte = vsprintf((char*)(temp_ptr + temp_offset),format,temp_va);
		if(0x0 >= temp_byte)
			temp_byte = 0x0;
		if(NET_LOG_VAR_LENGTH_MAX < temp_byte)
			temp_byte = NET_LOG_VAR_LENGTH_MAX;
		temp_offset += temp_byte;
		va_end(temp_va);
	}
	//输出日期信息
	if(temp_date)
	{
		fprintf(Log_File,"(%d|%02d|%02d)",y,m,d);
		if(temp_bool){
			fprintf(stdout,"(%d|%02d|%02d)",y,m,d);
		}
		macro_saveStr(temp_ptr,temp_offset,"(%d|%02d|%02d)",y,m,d);
	}
	//输出日志位置信息
	if(temp_position){
		fprintf(Log_File," <pid:%d,%s,%d,%s>",getpid(),function,line,file);
		if(temp_bool){
			fprintf(stdout," <pid:%d,%s,%d,%s>",getpid(),function,line,file);
		}
		macro_saveStr(temp_ptr,temp_offset," <pid:%d,%s,%d,%s>",getpid(),function,line,file);
	}
	//输出進程信息
	/*
	if(temp_position){
		fprintf(Log_File,"[PID:%-8d]",getpid());
		if(temp_bool){
			fprintf(stdout,"[PID:%-8d]",getpid());
		}
		macro_saveStr(temp_ptr,temp_offset,"[PID:%-8d]",getpid());
	}*/
	//输出换行符
	fprintf(Log_File,"\n");
	if(temp_bool){
		fprintf(stdout,"\n");
		fflush(stdout);
	}
	macro_saveStr(temp_ptr,temp_offset,"\n");

	//net send
	if(temp_net){
		sendto(Net_UDPSocket,Net_Log_Buffer,(temp_offset + 0x1),0x0,(sockaddr*)(&Net_Remote),sizeof(Net_Remote));
	}
	//flush
	if(temp_forceflush){
		fflush(Log_File);
	}
	//互斥解锁
	pthread_mutex_unlock(&Log_datamutex);
}

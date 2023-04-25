/**
 * @file LogHeader.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef SRC_LOGHEADER_H_
#define SRC_LOGHEADER_H_
#ifdef __cplusplus
extern "C"{
#endif

//#ifndef GENERATE_LIB
//#define LOG_API extern
//#else
//#define LOG_API
//#endif

#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

//! 日志信息级别类型
enum Info_Level{f_Info = 0x10,f_Warning = 0x12,f_Error = 0x14,f_Running = 0x18,f_Debug = 0x80};
typedef enum Info_Level Info_Level_t;

//初始化,只需要传入文件标识信息
int InitializeLog(char* file_label);
//释放
void ReleaseLog();
//设置是否将所有日志信息输出到标准输出流中
void LogShowAll(bool value);
//设置最低日志记录/显示级别
void LogFilterLevel(Info_Level_t value);
//设置子日志信息是否包含日期信息
void LogContainDate(bool value);
//设置子日志信息是否包含位置信息
void LogContainPosition(bool value);
//设置是否记录调试日志
void LogEnableDebug(bool value);
//设置是否显示调试日志到标准输出中
void LogShowDebug(bool value);
//设置是否强制刷新日志文件信息流
void LogForceFlush(bool value);
//设置是否启动网络
void LogEnableNet(bool value);
//设置网络的目标地址与端口
void LogSetNet(char* ip,int port);
//刷新缓冲区
void LogFFlush();
//日志记录/显示
void LogPrint(Info_Level_t level,int line,const char* function,const char* file,const char* format,...);

//! Log Macro
#define ILog(format,...) LogPrint(f_Info,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
#define WLog(format,...) LogPrint(f_Warning,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
#define ELog(format,...) LogPrint(f_Error,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
//!	Running Info
#define RLog(format,...) LogPrint(f_Running,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
//!	Debug Macro
#define DLog(format,...) LogPrint(f_Debug,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
//! Usual Macro,Show Position,Can't Show Information
#define Log(...) LogPrint(f_Debug,__LINE__,__FUNCTION__,__FILE__,NULL,##__VA_ARGS__)


//! Time Log
#define ITLog(ms,format,...) \
do{\
	static int run_cnt = 0x1;\
	static struct timespec now_t,start_t;\
	if(run_cnt){run_cnt--;clock_gettime(CLOCK_MONOTONIC,&start_t);}\
	clock_gettime(CLOCK_MONOTONIC,&now_t);\
	int tmp_ms = (now_t.tv_sec * 1000 + now_t.tv_nsec / 1000000) - (start_t.tv_sec * 1000 + start_t.tv_nsec / 1000000);\
	if(ms > tmp_ms) break;\
	LogPrint(f_Info,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__);\
	start_t = now_t;\
}while(0)

/*
#define IVTLog(ms,ptr,format,...) \
do{\
	static int run_cnt = 0x1;\
	static struct timespec now_t,start_t;\
	if(run_cnt){run_cnt--;clock_gettime(CLOCK_MONOTONIC,&start_t);}\
	clock_gettime(CLOCK_MONOTONIC,&now_t);\
	int tmp_ms = (now_t.tv_sec * 1000 + now_t.tv_nsec / 1000000) - (start_t.tv_sec * 1000 + start_t.tv_nsec / 1000000);\
	if(ms > tmp_ms) break;\
	LogPrint(f_Info,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__);\
	start_t = now_t;\
	if(NULL != ptr){*ptr = 0x1;}\
}while(0)*/
//! Time Log
#define WTLog(ms,format,...) \
do{\
	static int run_cnt = 0x1;\
	static struct timespec now_t,start_t;\
	if(run_cnt){run_cnt--;clock_gettime(CLOCK_MONOTONIC,&start_t);}\
	clock_gettime(CLOCK_MONOTONIC,&now_t);\
	int tmp_ms = (now_t.tv_sec * 1000 + now_t.tv_nsec / 1000000) - (start_t.tv_sec * 1000 + start_t.tv_nsec / 1000000);\
	if(ms > tmp_ms) break;\
	LogPrint(f_Warning,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__);\
	start_t = now_t;\
}while(0)
/*
#define WVTLog(ms,ptr,format,...) \
do{\
	static int run_cnt = 0x1;\
	static struct timespec now_t,start_t;\
	if(run_cnt){run_cnt--;clock_gettime(CLOCK_MONOTONIC,&start_t);}\
	clock_gettime(CLOCK_MONOTONIC,&now_t);\
	int tmp_ms = (now_t.tv_sec * 1000 + now_t.tv_nsec / 1000000) - (start_t.tv_sec * 1000 + start_t.tv_nsec / 1000000);\
	if(ms > tmp_ms) break;\
	LogPrint(f_Warning,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__);\
	start_t = now_t;\
	if(NULL != ptr){*ptr = 0x1;}\
}while(0)*/
//! Time Log
#define ETLog(ms,format,...) \
do{\
	static int run_cnt = 0x1;\
	static struct timespec now_t,start_t;\
	if(run_cnt){run_cnt--;clock_gettime(CLOCK_MONOTONIC,&start_t);}\
	clock_gettime(CLOCK_MONOTONIC,&now_t);\
	int tmp_ms = (now_t.tv_sec * 1000 + now_t.tv_nsec / 1000000) - (start_t.tv_sec * 1000 + start_t.tv_nsec / 1000000);\
	if(ms > tmp_ms) break;\
	LogPrint(f_Error,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__);\
	start_t = now_t;\
}while(0)
/*
#define EVTLog(ms,ptr,format,...) \
do{\
	static int run_cnt = 0x1;\
	static struct timespec now_t,start_t;\
	if(run_cnt){run_cnt--;clock_gettime(CLOCK_MONOTONIC,&start_t);}\
	clock_gettime(CLOCK_MONOTONIC,&now_t);\
	int tmp_ms = (now_t.tv_sec * 1000 + now_t.tv_nsec / 1000000) - (start_t.tv_sec * 1000 + start_t.tv_nsec / 1000000);\
	if(ms > tmp_ms) break;\
	LogPrint(f_Error,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__);\
	start_t = now_t;\
	if(NULL != ptr){*ptr = 0x1;}\
}while(0)*/
//! Time Log
#define DTLog(ms,format,...) \
do{\
	static int run_cnt = 0x1;\
	static struct timespec now_t,start_t;\
	if(run_cnt){run_cnt--;clock_gettime(CLOCK_MONOTONIC,&start_t);}\
	clock_gettime(CLOCK_MONOTONIC,&now_t);\
	int tmp_ms = (now_t.tv_sec * 1000 + now_t.tv_nsec / 1000000) - (start_t.tv_sec * 1000 + start_t.tv_nsec / 1000000);\
	if(ms > tmp_ms) break;\
	LogPrint(f_Debug,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__);\
	start_t = now_t;\
}while(0)
/*
#define DVTLog(ms,ptr,format,...) \
do{\
	static int run_cnt = 0x1;\
	static struct timespec now_t,start_t;\
	if(run_cnt){run_cnt--;clock_gettime(CLOCK_MONOTONIC,&start_t);}\
	clock_gettime(CLOCK_MONOTONIC,&now_t);\
	int tmp_ms = (now_t.tv_sec * 1000 + now_t.tv_nsec / 1000000) - (start_t.tv_sec * 1000 + start_t.tv_nsec / 1000000);\
	if(ms > tmp_ms) break;\
	LogPrint(f_Debug,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__);\
	start_t = now_t;\
	if(NULL != ptr){*ptr = 0x1;}\
}while(0)*/


#ifdef __cplusplus
}
#endif
#endif /* SRC_LOGHEADER_H_ */

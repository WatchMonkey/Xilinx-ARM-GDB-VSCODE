/*
 * LogHeader.h
 *
 *  Created on: 2021年11月2日
 *      Author: 14452
 */

#ifndef SRC_LOGHEADER_H_
#define SRC_LOGHEADER_H_
#ifdef __cplusplus
extern "C"{
#endif

#ifdef INTERNAL_USING
#define LOG_API
#else
#ifndef GENERATE_LIB
#define LOG_API extern
#else
#define LOG_API
#endif
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>

//! 日志信息级别类型
enum Info_Level{f_Info = 0x10,f_Warning = 0x12,f_Error = 0x14,f_Running = 0x18,f_Debug = 0x80};
typedef enum Info_Level Info_Level_t;

//初始化,只需要传入文件名称
LOG_API int InitializeLog(char* filepath);
//释放
LOG_API void ReleaseLog();
//设置是否将所有日志信息输出到标准输出流中
LOG_API void LogShowAll(bool value);
//设置最低日志记录/显示级别
LOG_API void LogFilterLevel(Info_Level_t value);
//设置子日志信息是否包含日期信息
LOG_API void LogContainDate(bool value);
//设置子日志信息是否包含位置信息
LOG_API void LogContainPosition(bool value);
//设置是否记录调试日志
LOG_API void LogEnableDebug(bool value);
//设置是否显示调试日志到标准输出中
LOG_API void LogShowDebug(bool value);
//设置是否强制刷新日志文件信息流
LOG_API void LogForceFlush(bool value);
//设置是否启动网络
LOG_API void LogEnableNet(bool value);
//设置网络的目标地址与端口
LOG_API void LogSetNet(char* ip,int port);
//刷新缓冲区
LOG_API void LogFFlush();
//日志记录/显示
LOG_API void LogPrint(bool write_file,Info_Level_t level,int line,const char* function,const char* file,const char* format,...);

//! Log Macro
#define ILog(format,...) LogPrint(true,f_Info,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
#define WLog(format,...) LogPrint(true,f_Warning,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
#define ELog(format,...) LogPrint(true,f_Error,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
//!	Running Info
#define RLog(format,...) LogPrint(true,f_Running,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
//!	Debug Macro
#define DLog(format,...) LogPrint(true,f_Debug,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)
#define Log(...) LogPrint(true,f_Debug,__LINE__,__FUNCTION__,__FILE__,NULL,##__VA_ARGS__)

//! Net Macro
#define NILog(format,...) LogPrint(false,f_Info,__LINE__,__FUNCTION__,__FILE__,format,##__VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif /* SRC_LOGHEADER_H_ */

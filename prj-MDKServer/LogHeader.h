/*
 * LogHeader.h
 *
 *  Created on: 2021��11��2��
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

//! ��־��Ϣ��������
enum Info_Level{f_Info = 0x10,f_Warning = 0x12,f_Error = 0x14,f_Running = 0x18,f_Debug = 0x80};
typedef enum Info_Level Info_Level_t;

//��ʼ��,ֻ��Ҫ�����ļ�����
LOG_API int InitializeLog(char* filepath);
//�ͷ�
LOG_API void ReleaseLog();
//�����Ƿ�������־��Ϣ�������׼�������
LOG_API void LogShowAll(bool value);
//���������־��¼/��ʾ����
LOG_API void LogFilterLevel(Info_Level_t value);
//��������־��Ϣ�Ƿ����������Ϣ
LOG_API void LogContainDate(bool value);
//��������־��Ϣ�Ƿ����λ����Ϣ
LOG_API void LogContainPosition(bool value);
//�����Ƿ��¼������־
LOG_API void LogEnableDebug(bool value);
//�����Ƿ���ʾ������־����׼�����
LOG_API void LogShowDebug(bool value);
//�����Ƿ�ǿ��ˢ����־�ļ���Ϣ��
LOG_API void LogForceFlush(bool value);
//�����Ƿ���������
LOG_API void LogEnableNet(bool value);
//���������Ŀ���ַ��˿�
LOG_API void LogSetNet(char* ip,int port);
//ˢ�»�����
LOG_API void LogFFlush();
//��־��¼/��ʾ
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

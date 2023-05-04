/*
 * LogHeader.c
 *
 *  Created on: 2021骞�11鏈�2鏃�
 *      Author: 14452
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

//**************************************瀹忓畾涔�**************************************//
#define RT_ERROR ((int)__LINE__)
#define NET_DF_PORT 9527
#define NET_DF_ADDR "127.0.0.1"
#define NET_LOG_MAX_LENGTH 2048
#define NET_LOG_VAR_LENGTH_MAX (NET_LOG_MAX_LENGTH - 256)

//**************************************绫诲瀷澹版槑**************************************//
/*!
 * @struct		鍔熻兘閰嶇疆淇℃伅
 */
typedef struct setInfo{
	Info_Level_t level;//!<绾у埆淇℃伅
	bool debug_enable;//!<鎵撳紑璋冭瘯淇℃伅璁板綍
	bool net_enable;//!<鎵撳紑缃戠粶鏃ュ織鍙戦��
	bool debug_stdout;//!<璋冭瘯淇℃伅杈撳嚭鍒皊tdout
	bool stdout_enable;//!<鎵�鏈変俊鎭娇鑳借緭鍑哄埌stdout
	bool contain_date;//!<鏃ュ織淇℃伅鍖呭惈鏃ユ湡
	bool contain_position;//!<鏃ュ織淇℃伅鍖呭惈浣嶇疆淇℃伅
	bool force_flush;//!<鏃ュ織淇℃伅姣忔閮借繘琛屽己鍒跺埛鏂�
	//
	bool need_write_logfile;//!<鏃ュ織淇℃伅鏄惁闇�瑕佸啓鍏ュ埌鏃ュ織鏂囦欢
}setInfo_t;

//**************************************榛樿璁剧疆**************************************//
//! 鏄惁灏嗘墍鏈夌殑鏃ュ織淇℃伅鍚屾杈撳嚭鍒版爣鍑嗚緭鍑轰腑
static bool Send_stdout = false;
//!	鏄惁璁板綍璋冭瘯淇℃伅
static bool Debug_Enable = false;
//!	鏄惁浣跨敤缃戠粶鍙戦�佹棩蹇椾俊鎭�
static bool Net_Enable = false;
//!	鏄惁鏄剧ず璋冭瘯淇℃伅鍒版爣鍑嗚緭鍑轰腑
static bool Debug_Show = true;
//!	鏄惁寮哄埗flush鏃ュ織鏂囦欢鏁版嵁娴�
static bool ForceFlush = false;
//!	鏃ュ織淇℃伅鏄惁鍖呭惈鏃ユ湡淇℃伅
static bool Contain_Date = false;
//!	鏃ュ織淇℃伅鏄惁鍖呭惈浣嶇疆淇℃伅
static bool Contain_Position = false;
//! 鏈�浣庢棩蹇楄褰�/鏄剧ず绾у埆
static Info_Level_t Log_level = f_Info;

//**************************************鍏ㄥ眬鍙傛暟**************************************//
//! 鏃ュ織鏂囦欢鎸囬拡
static FILE* Log_File = NULL;
//! 浜掓枼閲�
static pthread_mutex_t Log_localmutex;
static pthread_mutex_t Log_datamutex;
//!	鍒濆鍖栫粨鏋�
static unsigned int Flag_init_success = 0x0;
//!	鍒濆鍖栫姸鎬�
static bool Flag_init_status = false;
//!	鏁版嵁buffer,unused
static void* Net_Log_Buffer = NULL;
//!	缃戠粶鍦板潃
static char Net_Addr[0x40] = {0x0};
//! 缃戠粶绔彛
static int Net_Port = NET_DF_PORT;
//!	socket绔彛
static int Net_UDPSocket = -1;
//!	杩滅鍦板潃淇℃伅
static struct sockaddr_in Net_Remote;

//**************************************鍑芥暟鎺ュ彛**************************************//
/*!
    @brief  璁剧疆鏄惁鎵�鏈夌殑鏃ュ織鏁版嵁鍏ㄩ儴閮借緭鍑哄埌鏍囧噯杈撳嚭娴佷腑
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
    @brief  璁剧疆鏃ュ織淇℃伅鐨勮繃婊ょ骇鍒�
    @param	Info_Level_t,淇℃伅杈撳嚭绾у埆
    @retval void
*/
void LogFilterLevel(Info_Level_t value)
{
    pthread_mutex_lock(&Log_localmutex);
    Log_level = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  璁剧疆鏃ュ織淇℃伅鏄惁鍖呭惈鏃ユ湡淇℃伅
    @param	bool,鐘舵��
    @retval void
*/
void LogContainDate(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Contain_Date = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  璁剧疆鏃ュ織淇℃伅鏄惁鍖呭惈浣嶇疆淇℃伅
    @param	bool,鐘舵��
    @retval void
*/
void LogContainPosition(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Contain_Position = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  璁剧疆鏃ュ織鏄惁璁板綍debug淇℃伅
    @param	bool,鐘舵��
    @retval void
*/
void LogEnableDebug(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Debug_Enable = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  璁剧疆鏃ュ織鏄惁鍦ㄦ爣鍑嗚緭鍑轰腑鏄剧ずdebug淇℃伅
    @param	bool,鐘舵��
    @retval void
*/
void LogShowDebug(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Debug_Show = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  璁剧疆鏃ュ織鏄惁寮哄埗鍒锋柊鏃ュ織鏂囦欢鏁版嵁娴�
    @param	bool,鐘舵��
    @retval void
*/
void LogForceFlush(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    ForceFlush = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  璁剧疆鏃ュ織鏄惁鍚姩缃戠粶鍔熻兘
    @param	bool,鐘舵��
    @retval void
*/
void LogEnableNet(bool value)
{
    pthread_mutex_lock(&Log_localmutex);
    Net_Enable = value;
    pthread_mutex_unlock(&Log_localmutex);
}

/*!
    @brief  璁剧疆鏃ュ織鏄惁鍚姩缃戠粶鍔熻兘
    @param	char*,缃戠粶鍦板潃
    @param	int:缃戠粶绔彛
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
 * *************************************鍔熻兘鎺ュ彛**************************************
 */

/*!
    @brief  鍒锋柊缂撳啿鍖�
    @retval void
*/
void LogFFlush()
{
	//鍒锋柊缂撳啿鍖�
	if(NULL != Log_File){
		fflush(Log_File);
		fsync(fileno(Log_File));
	}
}

/*!
 * *************************************鍒濆鍖�/閲婃斁鍑芥暟**************************************
 */

/*!
    @brief  鍒濆鍖栨棩蹇楁ā鍧�
    @param	char*,鏃ュ織鏂囦欢鍚嶇О
    @retval 0x0-success
    		other-failed
	@return int
*/
int InitializeLog(char* filepath)
{
	int temp_logfp = 0x0;

	//init mutex
    if(0x0 == pthread_mutex_init(&Log_localmutex,NULL) || 0x0 == pthread_mutex_init(&Log_datamutex,NULL)){
        Flag_init_success |= 0x1;
    }else{
    	Flag_init_status = false;
    	return RT_ERROR;
    }

    //alloc memory
    Net_Log_Buffer = calloc(NET_LOG_MAX_LENGTH,0x1);
    if(Net_Log_Buffer != NULL){
        Flag_init_success |= 0x2;
    }else{
    	Flag_init_status = false;
    	return RT_ERROR;
    }

    //detect log folder
    if(0x0 != access("/home/root/LogFile",W_OK)){
    	//create folder
    	if(0x0 != mkdir("/home/root/LogFile",0755))
    	{
    		printf("create /home/root/LogFile folder failed\n");
    		return RT_ERROR;
    	}else{
    		Flag_init_success |= 0x4;
    	}
    }
    //generate file path
    char temp_logfilepath[512] = {0x0};
    strcat(temp_logfilepath,"/home/root/LogFile");
    strcat(temp_logfilepath,"/");
    strcat(temp_logfilepath,filepath);
    strcat(temp_logfilepath,".log");
    //open file
    Log_File = fopen(temp_logfilepath,"w+");
    if(Log_File != NULL){
    	Flag_init_success |= 0x8;
    }else{
    	Flag_init_status = false;
    	return RT_ERROR;
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
		return RT_ERROR;
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
    return 0x0;
}

/*!
 * 	@brief	閲婃斁鏃ュ織妯″潡
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
 * *************************************鏃ュ織杈撳嚭鍑芥暟**************************************
 */

/*!
 * @brief		妫�娴嬫槸鍚﹂渶瑕佸皢鏁版嵁杈撳嚭鍒版爣鍑嗚緭鍑篠tdout涓�
 * @return		bool:杈撳嚭鏉冮檺
 * @retval		true--杈撳嚭鍒皊tdout
 * @retval		false--涓嶈緭鍑哄埌stdout
 */
static inline bool detectStdout(setInfo_t info,Info_Level_t level)
{
	/*!
	 * @note		濡傛灉璁剧疆鎵�鏈変俊鎭緭鍑哄埌stdout,閭ｄ箞鏃犳潯浠惰繑鍥瀟rue
	 */
	if(true == info.stdout_enable){
		return true;
	}
	/*!
	 * @note		濡傛灉褰撳墠淇℃伅绾у埆鏄疎rror,Runing閭ｄ箞鏃犳潯浠惰繘琛屼俊鎭緭鍑�
	 */
	if(f_Error == level || f_Running == level){
		return true;
	}
	/*!
	 * @note		鍒ゆ柇褰撳墠鏄惁鏄皟璇曚俊鎭骇鍒�,骞朵緷鎹皟璇曚俊鎭緭鍑烘潈闄愯繘琛屽垽鏂�
	 */
	if(f_Debug == level && true == info.debug_stdout){
		return true;
	}

	return false;
}

/*!
 * @brief		妫�娴嬫槸鍚﹂渶瑕佸皢鏁版嵁杈撳嚭鍒版棩蹇楁枃浠朵腑
 * @return		bool:杈撳嚭鏉冮檺
 * @retval		true--杈撳嚭鍒皊tdout
 * @retval		false--涓嶈緭鍑哄埌stdout
 */
static inline bool detectLogfile(setInfo_t info,Info_Level_t level)
{
	/*!
	 * @note		濡傛灉璁剧疆鎵�鏈変俊鎭緭鍑哄埌鏃ュ織鏂囦欢涓�,閭ｄ箞鏃犳潯浠惰繑鍥瀟rue
	 */
	if(true == info.need_write_logfile){
		return true;
	}
	/*!
	 * @note		濡傛灉褰撳墠淇℃伅绾у埆鏄疎rror,Runing閭ｄ箞鏃犳潯浠惰繘琛屼俊鎭緭鍑�
	 */
	if(f_Error == level || f_Warning == level || f_Running == level){
		return true;
	}

	return false;
}

/*!
 * @brief		灏嗘暟鎹啓鍏ュ埌缂撳瓨涓�
 * @param		char*:缂撳瓨
 * @param		int:鍋忕Щ
 * @param		char*:鏍煎紡
 * @param		char*:鏁版嵁
 * @return		int:鍋忓潃鍊�
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
    @brief  璁板綍/杈撳嚭鏃ュ織淇℃伅
    @param	write_file,鍐欏叆鏃ュ織鏂囦欢
    @param	Info_Level_t,鏃ュ織绾у埆
    @param	int,鏃ュ織淇℃伅鐨勮鏁�
    @param	const char*,鏃ュ織淇℃伅鎵�鍦ㄧ殑鍑芥暟
    @param	const char*,鏃ュ織淇℃伅鎵�鍦ㄧ殑鏂囦欢
    @param	const char*,鍙彉鍙傛暟鐨勬牸寮忓瓧绗︿覆
    @param	...,鍙彉闀垮弬鏁�
    @retval	void
*/
void LogPrint(bool write_file,Info_Level_t level,int line,const char* function,const char* file,const char* format,...)
{
	char* temp_ptr = (char*)Net_Log_Buffer;
	int temp_offset = 0x0;
	int temp_byte = 0x0;
	//鍒ゆ柇鏃ュ織鍒濆鍖栫姸鎬�
	if(Flag_init_status == false){
		return;
	}

	//鍒ゆ柇鏄惁鏃ュ織淇℃伅绾у埆浠ュ強璋冭瘯浣胯兘鐘舵��
	pthread_mutex_lock(&Log_localmutex);
	Info_Level_t temp_level = Log_level;
	bool temp_debug = Debug_Enable;
	pthread_mutex_unlock(&Log_localmutex);
	//鍒ゆ柇鏃ュ織淇℃伅绾у埆鏄惁闇�瑕佽繘琛岃褰曚笌鏄剧ず
	if((int)level < (int)temp_level){
		return;
	}
	//鍒ゆ柇鏄惁鏄皟璇曚俊鎭�,濡傛灉娌℃湁浣胯兘璋冭瘯淇℃伅,閭ｄ箞涓嶈褰�/鏄剧ず璋冭瘯淇℃伅
	if(f_Debug == level && false == temp_debug){
		return;
	}
	//鍔熻兘璁剧疆鐘舵��
	pthread_mutex_lock(&Log_localmutex);
	bool temp_stdout = Send_stdout;
	bool temp_date = Contain_Date;
	bool temp_position = Contain_Position;
	bool temp_debugshow = Debug_Show;
	bool temp_forceflush = ForceFlush;
	bool temp_net = Net_Enable;
	pthread_mutex_unlock(&Log_localmutex);
	//鍔熻兘璁剧疆鐘舵�佺粺璁�
	setInfo_t temp_setInfo = {0x0};
	temp_setInfo.level = temp_level;
	temp_setInfo.debug_enable = temp_debug;
	temp_setInfo.net_enable = temp_net;
	temp_setInfo.debug_stdout = temp_debugshow;
	temp_setInfo.contain_date = temp_date;
	temp_setInfo.contain_position = temp_position;
	temp_setInfo.stdout_enable = temp_stdout;
	temp_setInfo.force_flush = temp_forceflush;
	temp_setInfo.need_write_logfile = write_file;
	//鍒ゆ柇淇℃伅鏄惁闇�瑕佸悓姝ヨ緭鍑哄埌stdout涓�
	bool temp_bool = detectStdout(temp_setInfo,level);
	//鍒ゆ柇淇℃伅鏄惁闇�瑕佸啓鍏ュ埌鏃ュ織鏂囦欢涓�
	bool temp_write_logfile = detectLogfile(temp_setInfo,level);

	//璁＄畻鏃堕棿
	struct tm today;
	time_t now;
	now = time(NULL);
	//now = now + 28880; //闁搞儳濮崇拹鐔煎箣閹存粍绮﹂柛锔兼嫹+8闁哄啳娉涚亸锟� 闁告艾閰ｅ鎵啺娴ｇ儤鏆廹mtime闁告帒妫滆闁哄啫鐖煎Λ锟� 闊洤鎳橀妴蹇涘箥鐎ｎ亙绱ｅǎ鍥跺枟椤掓粍绋夐敓鑺ョ▔閿燂拷 閻忓繗椴稿Σ锟�60缂佸鎷�*60闁告帒妫濋幐锟�*8閻忓繐绻戝锟� = 28880
	gmtime_r(&now, &today);
	int y,m,d,h,n,s;
	y = today.tm_year+1900;
	m = today.tm_mon+1;
	d = today.tm_mday;
	h = today.tm_hour;
	n = today.tm_min;
	s = today.tm_sec;
	///浜掓枼閿�,杈撳嚭鏃ュ織淇℃伅
	pthread_mutex_lock(&Log_datamutex);
	//鍒ゆ柇淇℃伅鐨勮緭鍑虹骇鍒�,骞朵笖杈撳嚭绾у埆淇℃伅
	switch(level)
	{
	case f_Info:
		if(temp_write_logfile)fprintf(Log_File,"[%-8s]","Info");
		if(temp_bool)fprintf(stdout,"[%-8s]","Info");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Info");
		break;
	case f_Warning:
		if(temp_write_logfile)fprintf(Log_File,"[%-8s]","Warn");
		if(temp_bool)fprintf(stdout,"[%-8s]","Warn");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Warn");
		break;
	case f_Error:
		if(temp_write_logfile)fprintf(Log_File,"[%-8s]","Error");
		fprintf(stdout,"[%-8s]","Error");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Error");
		break;
	case f_Running:
		if(temp_write_logfile)fprintf(Log_File,"[%-8s]","***Run");
		fprintf(stdout,"[%-8s]","***Run");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","***Run");
		break;
	case f_Debug:
		if(temp_write_logfile)fprintf(Log_File,"[%-8s]","Debug");
		if(temp_bool)fprintf(stdout,"[%-8s]","Debug");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Debug");
		break;
	default:
		if(temp_write_logfile)fprintf(Log_File,"[%-8s]","Unknown");
		if(temp_bool)fprintf(stdout,"[%-8s]","Unknown");
		macro_saveStr(temp_ptr,temp_offset,"[%-8s]","Unknown");
		break;
	}
	//杈撳嚭鏃堕棿淇℃伅
	if(temp_write_logfile)
		fprintf(Log_File,"[%-8d]",getpid());
	if(temp_bool){
		fprintf(stdout,"[%-8d]",getpid());
	}
	macro_saveStr(temp_ptr,temp_offset,"[%-8d]",getpid());
	//杈撳嚭鏃堕棿淇℃伅
	if(temp_write_logfile)
		fprintf(Log_File,"[%02d/%02d/%02d]",h,n,s);
	if(temp_bool){
		fprintf(stdout,"[%02d/%02d/%02d]",h,n,s);
	}
	macro_saveStr(temp_ptr,temp_offset,"[%02d/%02d/%02d]",h,n,s);
	//
	if(temp_write_logfile)
		fprintf(Log_File," ");
	if(temp_bool){
		fprintf(stdout," ");
	}
	macro_saveStr(temp_ptr,temp_offset," ");
	//杈撳嚭鍙彉闀垮害鍙傛暟涓殑鏁版嵁淇℃伅
	if(NULL != format)
	{
		va_list temp_va;
		va_start(temp_va,format);
		//
		if(temp_write_logfile)
			vfprintf(Log_File,format,temp_va);
		if(temp_bool)
			vfprintf(stdout,format,temp_va);
		//
		temp_byte = vsprintf((char*)(temp_ptr + temp_offset),format,temp_va);
		if(0x0 >= temp_byte)
			temp_byte = 0x0;
		if(NET_LOG_VAR_LENGTH_MAX < temp_byte)
			temp_byte = NET_LOG_VAR_LENGTH_MAX;
		temp_offset += temp_byte;
		va_end(temp_va);
	}
	//杈撳嚭鏃ユ湡淇℃伅
	if(temp_date)
	{
		if(temp_write_logfile)
			fprintf(Log_File,"(%d|%02d|%02d)",y,m,d);
		if(temp_bool)
			fprintf(stdout,"(%d|%02d|%02d)",y,m,d);
		macro_saveStr(temp_ptr,temp_offset,"(%d|%02d|%02d)",y,m,d);
	}
	//杈撳嚭鏃ュ織浣嶇疆淇℃伅
	if(temp_position){
		if(temp_write_logfile)
			fprintf(Log_File," <%s,%d,%s>",function,line,file);
		if(temp_bool)
			fprintf(stdout," <%s,%d,%s>",function,line,file);
		macro_saveStr(temp_ptr,temp_offset," <%s,%d,%s>",function,line,file);
	}
	//杈撳嚭鎹㈣绗�
	if(temp_write_logfile)
		fprintf(Log_File,"\n");
	if(temp_bool){
		fprintf(stdout,"\n");
		fflush(stdout);
	}
	macro_saveStr(temp_ptr,temp_offset,"\n");

	//net send
	if(temp_net){
		sendto(Net_UDPSocket,Net_Log_Buffer,(temp_offset + 0x1),0x0,&Net_Remote,sizeof(Net_Remote));
	}
	//flush
	if(temp_forceflush){
		if(temp_write_logfile)
			fflush(Log_File);
	}
	//浜掓枼瑙ｉ攣
	pthread_mutex_unlock(&Log_datamutex);
}

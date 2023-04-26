/*!
 * @file Untitled-1
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "LogHeader.h"


#define RT_ERROR ((int)__LINE__)


int main(int argc,char** argv)
{
    fprintf(stderr,"Execute Demo\n");

    for(int i = 0x0;i < argc;++i)
    {
        fprintf(stderr,"\t%d:%s\n",i,argv[i]);
    }

    //
    for(int i = 0x0;i < 5;++i)
    {
        fprintf(stderr,"log index:%d\n",i);
        usleep(100000);
    }


	int temp_rt = 0x0;
	//initialize log
	temp_rt = InitializeLog("appDemo");
	if(temp_rt){
		fprintf(stderr,"Log Module initialize failed\n");
		return RT_ERROR;
	}else{
		//TODO:[调试]这你只用于调试,最终需要修改下面的逻辑
#if COMPILE_VERSION == 0x0
		LogEnableDebug(true);
#endif
		LogForceFlush(true);
#if LOGLIB_NET_ENABLE
		LogSetNet(temp_ipaddr_remote,temp_ipport_remote);
		LogEnableNet(true);
#endif
	}

    for(int i = 0x0;i < 10;++i)
    {
        DLog("loglib test code,debug");
        ILog("loglib test code,info");
        WLog("loglib test code,warning");
        ELog("loglib test code,error");    
    }


    int temp_loop = 0x1;
    int64_t temp_loop_index = 0x0;
    while(1)
    {
        if(!temp_loop){
            DLog("exit loop");
            break;
        }
        //
        temp_loop_index++;
        DTLog(500,"curent index:%lld",temp_loop_index);
        //lib func

    }


    DLog("exit test demo");
    ReleaseLog();

    //
    fprintf(stderr,"finish Demo\n");

    return EXIT_SUCCESS;
}

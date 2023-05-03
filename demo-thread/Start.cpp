/*!
 * @file Start.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "LogHeader.h"
#include "ThreadHeader.h"
#include <string.h>
#include <exception>



typedef struct _res_data{
	int dma_fp;
	FILE* other_fp;
	int cnt;
}res_data;


static int local_loop(void* param)
{
    int temp_exit = 0x0;
    DLog("execute loop work function");
    if(NULL == param){
        ELog("input parameter invalid");
        return RT_ERROR;
    }
    thread_param_ptr thread_param = (thread_param_ptr)param;
    if(NULL == thread_param){
        ELog("threac control parameter invalid");
        return RT_ERROR;
    }

    int loop_exit = 0x0;
	int64_t pause_log = 0x0;
	int temp_cnt = 0x0;
    while(1)
    {
        //exit
        if(thread_check_exit()){
			DLog("exit loop");
			break;
		}
        //pause
		if(thread_check_pause()){
			if(!pause_log){
				DLog("pause");
				pause_log++;
			}
			continue;
		}else{
			if(pause_log){
				DLog("resume");
				pause_log = 0x0;
			}
		}
		//
		DTLog(500,"this was an information:%d",++temp_cnt);
    }

	DLog("exit child thread work function");
    return temp_exit;
}


static void* work_thread(void* param)
{
    DLog("already start child thread");
    int temp_rt = 0x0;
	thread_param_ptr thread_param = (thread_param_ptr)param;
	thread_param->thread_id = pthread_self();
	int* temp_exit_code = &thread_param->exit_code;
	//
	try{
		//work
		temp_rt = local_loop(param);
		if((temp_rt & 0x0000FFFF)){
			ELog("Work Thread[%s],PID=%08x running error",thread_param->thread_name,thread_param->thread_id);
		}else{
			ILog("Work Thread[%s],PID=%08x success finished",thread_param->thread_name,thread_param->thread_id);
		}
	}catch(const char* exp){
		ELog("Work Thread[%s],PID=%08x running happened exception,%s",thread_param->thread_name,thread_param->thread_id,exp);
		*temp_exit_code = RT_ERROR;
		return temp_exit_code;
	}catch(...){
		ELog("Work Thread[%s],PID=%08x running happened exception",thread_param->thread_name,thread_param->thread_id);
		*temp_exit_code = RT_ERROR;
		return temp_exit_code;
	}
	//exit
	*temp_exit_code = temp_rt;
	return temp_exit_code;
}


#define ONCE_OP 0x1


/*!
 * @brief main function
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main()
{
    fprintf(stdout,"start test application\n");
    //
    int temp_rt = 0x0;
	//initialize log
	temp_rt = InitializeLog("appDemo");
	if(temp_rt){
		fprintf(stderr,"Log Module initialize failed\n");
		return RT_ERROR;
	}else{
		LogEnableDebug(true);
		LogForceFlush(true);
#if LOGLIB_NET_ENABLE
		LogSetNet(temp_ipaddr_remote,temp_ipport_remote);
		LogEnableNet(true);
#endif
	}

    DLog("try to load workthread library");

    int tag_id = 0xFF;
	int thread_status = 0x0;
	res_data local_res = {
		.dma_fp = 0xf,
		.other_fp = NULL,
		.cnt = 12
	};
	//
	thread_param local_param;
	memset(&local_param,0x0,sizeof(local_param));
	sprintf(local_param.thread_name,"testThread");
	local_param.resource_ptr = &local_res;
#if ONCE_OP
	thread_status = thread_ready_and_start(work_thread,&local_param,tag_id,SCHED_RR,0x20);
	if(thread_status){
		ELog("function execute failed:%d",thread_status);
		return RT_ERROR;
	}
#else
    thread_status = thread_ready_param(work_thread,&local_param,tag_id);
	if(thread_status){
		ELog("function execute failed:%d",thread_status);
		return RT_ERROR;
	}
	//
	pthread_attr_t local_attr;
    thread_status = thread_ready_attribute(work_thread,&local_attr,tag_id);
	if(thread_status){
		ELog("function execute failed:%d",thread_status);
		return RT_ERROR;
	}
	//
    thread_status = thread_create_and_start(tag_id);
	if(thread_status){
		ELog("function execute failed:%d",thread_status);
		return RT_ERROR;
	}
#endif

	DLog("running main function loop");
	int temp_pause = 0x0;
	int temp_stop = 0x0;
	int loop_status = 0x0;
	while(1)
	{
		if(temp_stop){
			loop_status = thread_release_and_destroy(tag_id);
			if(loop_status){
				ELog("function execute failed:%d",loop_status);
				break;
			}	
		}
		//
		if(0x1 == temp_pause){
			loop_status = thread_pause(tag_id);
			if(loop_status){
				ELog("function execute failed:%d",loop_status);
				break;
			}	
		}else if(0x2 == temp_pause){
			loop_status = thread_resume(tag_id);
			if(loop_status){
				ELog("function execute failed:%d",loop_status);
				break;
			}	
		}
	}


    ReleaseLog();
    //
    return EXIT_SUCCESS;
}

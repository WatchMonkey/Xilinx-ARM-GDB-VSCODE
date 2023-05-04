/*!
 * @file ThreadImpl_Init.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "ThreadHeader.h"
#include "thread_internal.h"
#include "mgr_header.h"
#include "mgr_data.h"
#include "error_log.h"
#include "LogHeader.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <exception>



/*!
 * @brief default child thrad main function
 * 
 * @param param 
 * @return void* 
 */
static void* work_thread(void* param)
{
    ILog("already execute child thread");
    DLog("already execute child thread");
	thread_param_ptr thread_param = (thread_param_ptr)param;
	thread_param->thread_id = pthread_self();
    thread_param->thread_pid = getpid();
	int* temp_exit_code = &thread_param->exit_code;
    //TODO:这里需要进行一些处理，使用其他更好的方式解决tid无法及时别写入到mgr_array中的问题
    usleep(1000000);
    //TODO:条件机制的使用，还未完成
    //while(false == thread_cond_check(pthread_self()));
	//
    status_t temp_rt = 0x0;
    if(NULL == thread_param->userfunc_ptr){
        ELog("Work Thread[%s],TID=%08x user func point was invalid,exit child thread",thread_param->thread_name,thread_param->thread_id);
        *temp_exit_code = RT_ERROR;
		return temp_exit_code;
    }
	try{
		//work
		temp_rt = thread_param->userfunc_ptr(param);
		if((temp_rt & 0x0000FFFF)){
			ELog("Work Thread[%s],TID=%08x running error",thread_param->thread_name,thread_param->thread_id);
		}else{
			ILog("Work Thread[%s],TID=%08x success finished",thread_param->thread_name,thread_param->thread_id);
		}
	}catch(const char* exp){
		ELog("Work Thread[%s],TID=%08x running happened exception,%s",thread_param->thread_name,thread_param->thread_id,exp);
		*temp_exit_code = RT_ERROR;
		return temp_exit_code;
	}catch(...){
		ELog("Work Thread[%s],TID=%08x running happened exception",thread_param->thread_name,thread_param->thread_id);
		*temp_exit_code = RT_ERROR;
		return temp_exit_code;
	}
	//exit
	*temp_exit_code = temp_rt;
	return temp_exit_code;
}
#ifdef EXMAPLE_CODE
/*!
 * @brief user work code function
 * 
 * @param param 
 * @return int 
 */
static int local_loop(void* param)
{
    thread_param_ptr thread_param = (thread_param_ptr)param;
    if(NULL == thread_param){
        ELog("threac control parameter invalid");
        return RT_ERROR;
    }
    while(1){
        //user work logic code
    }
    return 0x0;
}
/*!
 * @brief 用户模板线程入口函数的写法,直接copy，然后修改local_loop函数的逻辑
 * 
 * @param param 
 * @return void* 
 */
static void* exmaple_thread_main_func(void* param)
{
    ILog("already execute child thread");
    DLog("already execute child thread");
	thread_param_ptr thread_param = (thread_param_ptr)param;
	thread_param->thread_id = pthread_self();
    thread_param->thread_pid = getpid();
	int* temp_exit_code = &thread_param->exit_code;
    //TODO:这里需要进行一些处理，使用其他更好的方式解决tid无法及时别写入到mgr_array中的问题
    usleep(1000000);
    //TODO:条件机制的使用，还未完成
    //while(false == thread_cond_check(pthread_self()));
	//
	status_t temp_rt = 0x0;
	try{
		//work
		temp_rt = local_loop(param);
		if((temp_rt & 0x0000FFFF)){
			ELog("Work Thread[%s],TID=%08x running error",thread_param->thread_name,thread_param->thread_id);
		}else{
			ILog("Work Thread[%s],TID=%08x success finished",thread_param->thread_name,thread_param->thread_id);
		}
	}catch(const char* exp){
		ELog("Work Thread[%s],TID=%08x running happened exception,%s",thread_param->thread_name,thread_param->thread_id,exp);
		*temp_exit_code = RT_ERROR;
		return temp_exit_code;
	}catch(...){
		ELog("Work Thread[%s],TID=%08x running happened exception",thread_param->thread_name,thread_param->thread_id);
		*temp_exit_code = RT_ERROR;
		return temp_exit_code;
	}
	//exit
	*temp_exit_code = temp_rt;
	return temp_exit_code;
}
#endif


status_t thread_ready_param(thread_func func,thread_param_ptr param,int identify)
{
    if(NULL == param){
        ELog("input parameter invlaid");
        return RT_ERROR;
    }
    //
    frist_init_array();
    if(set_param_by_iden(identify,param)){
        ELog("%d.%d:%s",identify,P_PERROR_CODE,P_PERROR_STR);
        return RT_ERROR;
    }
    if(NULL != func)
    {
        if(set_func_by_iden(identify,func)){
            ELog("%d.%d:%s",identify,P_PERROR_CODE,P_PERROR_STR);
            return RT_ERROR;
        }
    }
    
    return 0x0;
}


status_t thread_ready_attribute(thread_func func,pthread_attr_t* attr_ptr,int identify)
{
    if(NULL == attr_ptr){
        ELog("input parameter invlaid");
        return RT_ERROR;
    }
    //
    frist_init_array();
    thread_attr temp_attr = {
        .attr = *attr_ptr
    };
    if(set_attr_by_iden(identify,&temp_attr)){
        ELog("%d.%d:%s",identify,P_PERROR_CODE,P_PERROR_STR);
        return RT_ERROR;
    }
    if(NULL != func)
    {
        if(set_func_by_iden(identify,func)){
            ELog("%d.%d:%s",identify,P_PERROR_CODE,P_PERROR_STR);
            return RT_ERROR;
        }
    }

    return 0x0;
}


status_t thread_ready_and_start(thread_func func,thread_param_ptr param,int identify,int sched_type,int priority_level)
{
    if(thread_ready_param(func,param,identify)){
        return RT_ERROR;
    }
    //
    pthread_attr_t temp_attr_var;
    if(pthread_attr_init(&temp_attr_var)){
        ELog("%d.pthread_attr_init failed,%d:%s",identify,errno,strerror(errno));
        return RT_ERROR;
    }
    if(pthread_attr_setinheritsched(&temp_attr_var,PTHREAD_EXPLICIT_SCHED)){
        ELog("%d.pthread_attr_set failed,%d:%s",identify,errno,strerror(errno));
        return RT_ERROR;
    }
    if( SCHED_RR == sched_type || SCHED_FIFO == sched_type){
        if(false == thread_check_runtime_support()){
            ELog("not support runtime sched policy");
            return RT_ERROR;
        }
    }
    if(pthread_attr_setschedpolicy(&temp_attr_var, sched_type)){
        ELog("%d.pthread_attr_set schedpolicy failed,%d:%s",identify,errno,strerror(errno));
        return RT_ERROR;
    }
    struct sched_param sched_var;
    if(pthread_attr_getschedparam(&temp_attr_var,&sched_var)){
        ELog("%d.pthread_attr get schedparam failed,%d:%s",identify,errno,strerror(errno));
        return RT_ERROR;
    }
    sched_var.sched_priority = priority_level;
    if(pthread_attr_setschedparam(&temp_attr_var,&sched_var)){
        ELog("%d.pthread_attr set schedparam failed,%d:%s",identify,errno,strerror(errno));
        return RT_ERROR;
    }
    if(pthread_attr_setdetachstate(&temp_attr_var,PTHREAD_CREATE_JOINABLE)){
        ELog("%d.pthread_attr set detachstate joinable failed,%d:%s",identify,errno,strerror(errno));
        return RT_ERROR;
    }
    //
    if(thread_ready_attribute(func,&temp_attr_var,identify)){
        return RT_ERROR;
    }
    //
    if(thread_create_and_start(identify)){
        return RT_ERROR;
    }

    return 0x0;
}


status_t thread_create_and_start(int identify)
{
    if(NULL == get_attr_by_iden(identify) || NULL == get_param_by_iden(identify)){
        ELog("can't get valid attr data or param data order by identify:%d",identify);
        return RT_ERROR;
    }
    //
    if(internal_thread_start(identify)){
        return RT_ERROR;
    }
    if(thread_pause(identify)){
        return RT_ERROR;
    }
    //
    pthread_t temp_tid;
    thread_param_ptr temp_param = get_param_by_iden(identify);
    //set control data point
    temp_param->remain_ptr = get_ctrl_by_iden(identify);
    if(NULL == get_func_by_iden(identify))
    {
        if(pthread_create(&temp_tid,&(get_attr_by_iden(identify)->attr),work_thread,temp_param)){
            ELog("create thread failed order by identify:%d",identify);
            return RT_ERROR;
        }
    }else{
        if(pthread_create(&temp_tid,&(get_attr_by_iden(identify)->attr),get_func_by_iden(identify),temp_param)){
            ELog("create thread failed order by identify:%d",identify);
            return RT_ERROR;
        }
    }
    if(set_tid_by_iden(identify,temp_tid)){
        ELog("save thread id reference identify");
        return RT_ERROR;
    }
    //TODO:条件机制的使用，还未完成
    //thread_cond_emit(identify);
    //
    if(thread_resume(identify)){
        return RT_ERROR;
    }

    return 0x0;
}


status_t thread_release_and_destroy(int identify)
{
    if(thread_resume(identify)){
        return RT_ERROR;
    }
    if(internal_thread_stop(identify)){
        return RT_ERROR;
    }
    //
    thread_param_ptr temp_param = get_param_by_iden(identify);
    if(NULL == temp_param){
        ELog("get thread parameter data from mgr was invalid");
        return RT_ERROR;
    }
    pthread_t temp_tid = temp_param->thread_id;
    void* temp_value = NULL;
    if(pthread_join(temp_tid,&temp_value)){
        ELog("thread join function occur error,%d:%s",errno,strerror(errno));
        return RT_ERROR;
    }
    DLog("child thread:%x.[%s] exit success,return value:%d",temp_tid,temp_param->thread_name,*(int*)temp_value);
    ILog("child thread:%x.[%s] exit success,return value:%d",temp_tid,temp_param->thread_name,*(int*)temp_value);
    //
    init_index_by_iden(identify);
    ILog("identify:%d destroy success",identify);

    return 0x0;
}

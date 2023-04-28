/**
 * @file ThreadImpl.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "ThreadHeader.h"
#include "thread_internal.h"
#include "error_log.h"
#include "LogHeader.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <exception>



static void* work_thread(void* param)
{
	thread_param_ptr thread_param = (thread_param_ptr)param;
	thread_param->thread_id = pthread_self();
    thread_param->thread_pid = getpid();
	int* temp_exit_code = &thread_param->exit_code;
	//
    status_t temp_rt = 0x0;
    if(NULL == thread_param->userfunc_ptr){
        ELog("Work Thread[%s],PID=%08x user func point was invalid,exit child thread",thread_param->thread_name,thread_param->thread_id);
        *temp_exit_code = RT_ERROR;
		return temp_exit_code;
    }
	try{
		//work
		temp_rt = thread_param->userfunc_ptr(param);
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


status_t thread_ready(thread_func func,thread_param_ptr param,int tag_id)
{
    if(NULL == param){
        ELog("input parameter invlaid");
        return RT_ERROR;
    }
    //
    frist_init_array();
    if(add_param(tag_id,param)){
        ELog("%d.%d:%s",tag_id,P_PERROR_CODE,P_PERROR_STR);
        return RT_ERROR;
    }
    if(NULL != func)
    {
        if(add_func(tag_id,func)){
            ELog("%d.%d:%s",tag_id,P_PERROR_CODE,P_PERROR_STR);
            return RT_ERROR;
        }
    }
    
    return 0x0;
}


status_t thread_set_thread_attribute(thread_func func,pthread_attr_t* attr_ptr,int tag_id)
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
    if(add_attr(tag_id,&temp_attr)){
        ELog("%d.%d:%s",tag_id,P_PERROR_CODE,P_PERROR_STR);
        return RT_ERROR;
    }
    if(NULL != func)
    {
        if(add_func(tag_id,func)){
            ELog("%d.%d:%s",tag_id,P_PERROR_CODE,P_PERROR_STR);
            return RT_ERROR;
        }
    }

    return 0x0;
}


status_t thread_ready_and_run(thread_func func,thread_param_ptr param,int tag_id,int sched_type,int priority_level)
{
    if(thread_ready(func,param,tag_id)){
        return RT_ERROR;
    }
    //
    pthread_attr_t temp_attr_var;
    if(pthread_attr_init(&temp_attr_var)){
        ELog("%d.pthread_attr_init failed,%d:%s",tag_id,errno,strerror(errno));
        return RT_ERROR;
    }
    if(pthread_attr_setinheritsched(&temp_attr_var,PTHREAD_EXPLICIT_SCHED)){
        ELog("%d.pthread_attr_set failed,%d:%s",tag_id,errno,strerror(errno));
        return RT_ERROR;
    }
    if( SCHED_RR == sched_type || SCHED_FIFO == sched_type){
        if(false == thread_check_runtime_support()){
            ELog("not support runtime sched policy");
            return RT_ERROR;
        }
    }
    if(pthread_attr_setschedpolicy(&temp_attr_var, sched_type)){
        ELog("%d.pthread_attr_set schedpolicy failed,%d:%s",tag_id,errno,strerror(errno));
        return RT_ERROR;
    }
    struct sched_param sched_var;
    if(pthread_attr_getschedparam(&temp_attr_var,&sched_var)){
        ELog("%d.pthread_attr get schedparam failed,%d:%s",tag_id,errno,strerror(errno));
        return RT_ERROR;
    }
    sched_var.sched_priority = priority_level;
    if(pthread_attr_setschedparam(&temp_attr_var,&sched_var)){
        ELog("%d.pthread_attr set schedparam failed,%d:%s",tag_id,errno,strerror(errno));
        return RT_ERROR;
    }
    //
    if(thread_set_thread_attribute(func,&temp_attr_var,tag_id)){
        return RT_ERROR;
    }
    //
    if(thread_create_and_start(tag_id)){
        return RT_ERROR;
    }

    return 0x0;
}


status_t thread_create_and_start(int tag_id)
{
    if(NULL == get_attr(tag_id) || NULL == get_param(tag_id)){
        ELog("can't get valid attr data or param data order by tag_id:%d",tag_id);
        return RT_ERROR;
    }
    //
    if(thread_pause(tag_id)){
        return RT_ERROR;
    }
    //
    pthread_t temp_pid;
    thread_param_ptr temp_param = get_param(tag_id);
    temp_param->remain_ptr = get_ctrl(tag_id);
    if(NULL == get_func(tag_id))
    {
        if(pthread_create(&temp_pid,&(get_attr(tag_id)->attr),work_thread,temp_param)){
            ELog("create thread failed order by tag_id:%d",tag_id);
            return RT_ERROR;
        }
    }else{
        if(pthread_create(&temp_pid,&(get_attr(tag_id)->attr),get_func(tag_id),temp_param)){
            ELog("create thread failed order by tag_id:%d",tag_id);
            return RT_ERROR;
        }
    }
    //
    if(thread_resume(tag_id)){
        return RT_ERROR;
    }

    return 0x0;
}


status_t thread_pause(int tag_id)
{
    if(NULL == get_ctrl(tag_id)){
        ELog("can't get valid ctrl data order by tag_id:%d",tag_id);
        return RT_ERROR;
    }
    thread_control_ptr temp_ctrl = get_ctrl(tag_id);
    pthread_mutex_lock(&temp_ctrl->mutex_pause);
    temp_ctrl->pause = true;
    pthread_mutex_unlock(&temp_ctrl->mutex_pause);

    return 0x0;
}


status_t thread_resume(int tag_id)
{
    if(NULL == get_ctrl(tag_id)){
        ELog("can't get valid ctrl data order by tag_id:%d",tag_id);
        return RT_ERROR;
    }
    thread_control_ptr temp_ctrl = get_ctrl(tag_id);
    pthread_mutex_lock(&temp_ctrl->mutex_pause);
    temp_ctrl->pause = false;
    pthread_mutex_unlock(&temp_ctrl->mutex_pause);

    return 0x0;
}


status_t thread_stop(int tag_id)
{
    if(NULL == get_ctrl(tag_id)){
        ELog("can't get valid ctrl data order by tag_id:%d",tag_id);
        return RT_ERROR;
    }
    thread_control_ptr temp_ctrl = get_ctrl(tag_id);
    pthread_mutex_lock(&temp_ctrl->mutex_stop);
    temp_ctrl->stop = true;
    pthread_mutex_unlock(&temp_ctrl->mutex_stop);

    return 0x0;
}


status_t thread_release_and_destroy(int tag_id)
{
    return 0x0;
}


thread_ctrl_id thread_current_status(int tag_id)
{
    return s_thread_unknow;
}


bool thread_check_runtime_support()
{
    bool rt;
#if defined(_POSIX_THREAD_PRIORITY_SCHEDULING)
    rt = true;
#else
    rt = false;
#endif
    return rt;
}

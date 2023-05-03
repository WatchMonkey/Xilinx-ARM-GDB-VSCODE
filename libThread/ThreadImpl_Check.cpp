/*!
 * @file ThreadImpl_Check.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "ThreadHeader.h"
#include "mgr_header.h"
#include "mgr_data.h"
#include "error_log.h"
#include "LogHeader.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <exception>



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


bool thread_check_exit(int identify)
{
    if(NULL == get_ctrl_by_iden(identify)){
        ELog("can't get valid ctrl data order by identify:%d",identify);
        return RT_ERROR;
    }
    bool temp_value = false;
    thread_control_ptr temp_ctrl = get_ctrl_by_iden(identify);
    pthread_mutex_lock(&temp_ctrl->mutex_stop);
    temp_value = temp_ctrl->stop;
    pthread_mutex_unlock(&temp_ctrl->mutex_stop);

    return temp_value;
}


bool thread_check_exit()
{
    int temp_index = find_index_by_tid(pthread_self());
    if(0x0 > temp_index){
        ELog("can't find current tid");
        return RT_ERROR;
    }
    //
    if(NULL == get_ctrl_by_iden(get_iden(temp_index))){
        ELog("can't get valid ctrl data order by identify:%d",get_iden(temp_index));
        return RT_ERROR;
    }
    bool temp_value = false;
    thread_control_ptr temp_ctrl = get_ctrl_by_iden(get_iden(temp_index));
    pthread_mutex_lock(&temp_ctrl->mutex_stop);
    temp_value = temp_ctrl->stop;
    pthread_mutex_unlock(&temp_ctrl->mutex_stop);

    return temp_value;
}


bool thread_check_pause(int identify)
{
    if(NULL == get_ctrl_by_iden(identify)){
        ELog("can't get valid ctrl data order by identify:%d",identify);
        return RT_ERROR;
    }
    bool temp_value = false;
    thread_control_ptr temp_ctrl = get_ctrl_by_iden(identify);
    pthread_mutex_lock(&temp_ctrl->mutex_pause);
    temp_value = temp_ctrl->pause;
    pthread_mutex_unlock(&temp_ctrl->mutex_pause);

    return temp_value;
}


bool thread_check_pause()
{
    int temp_index = find_index_by_tid(pthread_self());
    if(0x0 > temp_index){
        ELog("can't find current tid");
        return RT_ERROR;
    }
    //
    if(NULL == get_ctrl_by_iden(get_iden(temp_index))){
        ELog("can't get valid ctrl data order by identify:%d",get_iden(temp_index));
        return RT_ERROR;
    }
    bool temp_value = false;
    thread_control_ptr temp_ctrl = get_ctrl_by_iden(get_iden(temp_index));
    pthread_mutex_lock(&temp_ctrl->mutex_pause);
    temp_value = temp_ctrl->pause;
    pthread_mutex_unlock(&temp_ctrl->mutex_pause);

    return temp_value;
}

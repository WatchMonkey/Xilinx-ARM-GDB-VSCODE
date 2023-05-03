/*!
 * @file ThreadImpl_Ctrl.cpp
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
#include "error_log.h"
#include "LogHeader.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <exception>



thread_ctrl_id thread_current_status(int identify)
{
    return s_thread_unknow;
}


status_t thread_pause(int identify)
{
    if(NULL == get_ctrl_by_iden(identify)){
        ELog("can't get valid ctrl data order by identify:%d",identify);
        return RT_ERROR;
    }
    thread_control_ptr temp_ctrl = get_ctrl_by_iden(identify);
    pthread_mutex_lock(&temp_ctrl->mutex_pause);
    temp_ctrl->pause = true;
    pthread_mutex_unlock(&temp_ctrl->mutex_pause);

    return 0x0;
}


status_t thread_resume(int identify)
{
    if(NULL == get_ctrl_by_iden(identify)){
        ELog("can't get valid ctrl data order by identify:%d",identify);
        return RT_ERROR;
    }
    thread_control_ptr temp_ctrl = get_ctrl_by_iden(identify);
    pthread_mutex_lock(&temp_ctrl->mutex_pause);
    temp_ctrl->pause = false;
    pthread_mutex_unlock(&temp_ctrl->mutex_pause);

    return 0x0;
}


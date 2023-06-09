/*!
 * @file thread_internal.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "thread_internal.h"
#include "LogHeader.h"



status_t internal_thread_start(int identify)
{
    if(NULL == get_ctrl_by_iden(identify)){
        ELog("can't get valid ctrl data order by identify:%d",identify);
        return RT_ERROR;
    }
    thread_control_ptr temp_ctrl = get_ctrl_by_iden(identify);
    pthread_mutex_lock(&temp_ctrl->mutex_stop);
    temp_ctrl->stop = false;
    pthread_mutex_unlock(&temp_ctrl->mutex_stop);

    return 0x0;
}


status_t internal_thread_stop(int identify)
{
    if(NULL == get_ctrl_by_iden(identify)){
        ELog("can't get valid ctrl data order by identify:%d",identify);
        return RT_ERROR;
    }
    thread_control_ptr temp_ctrl = get_ctrl_by_iden(identify);
    pthread_mutex_lock(&temp_ctrl->mutex_stop);
    temp_ctrl->stop = true;
    pthread_mutex_unlock(&temp_ctrl->mutex_stop);

    return 0x0;
}

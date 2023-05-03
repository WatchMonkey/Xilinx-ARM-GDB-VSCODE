/*!
 * @file mgr_ref_tid.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "mgr_header.h"
#include "mgr_data.h"
#include "error_log.h"
#include <string.h>


int find_index_by_tid(pthread_t tid)
{
    int temp_index = -1;
    if(0x0 == tid){
        P_PERROR(RT_ERROR,"input identify:%d invalid",tid);
        return RT_ERROR;
    }
    //
    for(int i = 0x0;i < MGR_THREAD_MAX;++i)
    {
        if(MGR_TID_DEFAULT == get_tid(i))
            continue;
        if(tid == get_tid(i)){
            temp_index = i;
        }
    }

    return temp_index;
}
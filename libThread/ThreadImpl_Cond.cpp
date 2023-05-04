/*!
 * @file ThreadImpl_Cond.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
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


static struct _local_data{
    pthread_mutex_t mtx[MGR_THREAD_MAX];
    pthread_cond_t cond[MGR_THREAD_MAX];
    pthread_t tid[MGR_THREAD_MAX];
}local_cond = {0x0};

static pthread_mutex_t local = PTHREAD_MUTEX_INITIALIZER;



bool thread_cond_check(pthread_t tid)
{
    int temp_index = -1;
    int temp_frist = -1;
    pthread_mutex_lock(&local);
    for(int i = 0x0;i < MGR_THREAD_MAX;++i){
        if(0x0 == local_cond.tid[i]){
            temp_frist = i;
            continue;
        }
        if(tid == local_cond.tid[i]){
            temp_index = i;
            break;
        }
    }
    if(0x0 > temp_index){
        if(0x0 > temp_frist){
            pthread_mutex_unlock(&local);
            return false;
        }else{
            local_cond.tid[temp_frist] = tid;
            local_cond.mtx[temp_frist] = PTHREAD_MUTEX_INITIALIZER;
            local_cond.cond[temp_frist] = PTHREAD_COND_INITIALIZER;
        }
    }
    pthread_mutex_unlock(&local);
}


void thread_cond_emit(pthread_t tid)
{
    return;
}

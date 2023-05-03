/*!
 * @file mgr_data.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "mgr_data.h"
#include "error_log.h"
#include <string.h>


static thread_mgr mgr_array[MGR_THREAD_MAX];
static pthread_mutex_t mutex_mgr = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_only_one = PTHREAD_MUTEX_INITIALIZER;
static uint32_t only_one = 0x0;


void init_index(int index)
{
    if(0x0 > index || MGR_THREAD_MAX < index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return;
    }
    //
    pthread_mutex_lock(&mutex_mgr);
    memset(&mgr_array[index],0x0,sizeof(thread_mgr));
    mgr_array[index].ctrl.mutex = PTHREAD_MUTEX_INITIALIZER;
    mgr_array[index].ctrl.mutex_pause = PTHREAD_MUTEX_INITIALIZER;
    mgr_array[index].ctrl.mutex_stop = PTHREAD_MUTEX_INITIALIZER;
    mgr_array[index].ctrl.status = s_thread_unknow;
    mgr_array[index].iden = MGR_IDEN_DEFAULT;
    mgr_array[index].tid = MGR_TID_DEFAULT;
    pthread_mutex_unlock(&mutex_mgr);
    return;
}

void frist_init_array()
{
    pthread_mutex_lock(&mutex_only_one);
    if(0x0 >= only_one++)
    {
        for(int i = 0x0;i < MGR_THREAD_MAX;++i){
            init_index(i);
        }
    }
    pthread_mutex_unlock(&mutex_only_one);
}


void set_param(int index,thread_param_ptr param)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return;
    }
    //
    pthread_mutex_lock(&mutex_mgr);
    if(NULL == param){
        memset(&mgr_array[index].param,0x0,sizeof(mgr_array[index].param));
    }else{
        mgr_array[index].param = *param;
    }
    pthread_mutex_unlock(&mutex_mgr);
}
thread_param_ptr get_param(int index)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return NULL;
    }
    //
    return &mgr_array[index].param;
}


void set_attr(int index,thread_attr_ptr attr)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return;
    }
    //
    pthread_mutex_lock(&mutex_mgr);
    if(NULL == attr){
        memset(&mgr_array[index].attr,0x0,sizeof(mgr_array[index].attr));
    }else{
        mgr_array[index].attr = *attr;
    }
    pthread_mutex_unlock(&mutex_mgr);
}
thread_attr_ptr get_attr(int index)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return NULL;
    }
    //
    return &mgr_array[index].attr;
}


void set_ctrl(int index,thread_control_ptr ctrl)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return;
    }
    //
    pthread_mutex_lock(&mutex_mgr);
    if(NULL == ctrl){
        memset(&mgr_array[index].ctrl,0x0,sizeof(mgr_array[index].ctrl));
    }else{
        mgr_array[index].ctrl = *ctrl;
    }
    pthread_mutex_unlock(&mutex_mgr);
}
thread_control_ptr get_ctrl(int index)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return NULL;
    }
    //
    return &mgr_array[index].ctrl;
}


void set_func(int index,thread_func func)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return;
    }
    //
    pthread_mutex_lock(&mutex_mgr);
    if(NULL == func){
        memset(&mgr_array[index].func,0x0,sizeof(mgr_array[index].func));
    }else{
        mgr_array[index].func = func;
    }
    pthread_mutex_unlock(&mutex_mgr);
}
thread_func get_func(int index)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return NULL;
    }
    //
    return mgr_array[index].func;
}



void set_tid(int index,pthread_t tid)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return;
    }
    //
    pthread_mutex_lock(&mutex_mgr);
    mgr_array[index].tid = tid;
    pthread_mutex_unlock(&mutex_mgr);
}
pthread_t get_tid(int index)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return MGR_TID_DEFAULT;
    }
    return mgr_array[index].tid;
}



void set_iden(int index,int identify)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return;
    }
    //
    pthread_mutex_lock(&mutex_mgr);
    mgr_array[index].iden = identify;
    pthread_mutex_unlock(&mutex_mgr);
}
int get_iden(int index)
{
    if(0x0 > index || MGR_THREAD_MAX <= index){
        P_PERROR(RT_ERROR,"input index:%d invalid",index);
        return MGR_IDEN_DEFAULT;
    }
    return mgr_array[index].iden;
}

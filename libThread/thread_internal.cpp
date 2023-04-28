/**
 * @file thread_internal.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#include "thread_internal.h"
#include "error_log.h"
#include <string.h>


static thread_mgr mgr_array[MGR_THREAD_MAX];
static pthread_mutex_t mutex_mgr = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_only_one = PTHREAD_MUTEX_INITIALIZER;
static uint32_t only_one = 0x0;


static bool check_unique(int tag_id)
{
    int cnt = 0x0;
    for(int i = 0x0;i < MGR_THREAD_MAX;++i)
    {
        if(0x0 > mgr_array[i].tag_id){
            continue;
        }
        if(tag_id != mgr_array[i].tag_id){
            continue;
        }
        cnt++;
    }
    return !cnt;
}


static bool check_full(int& first)
{
    int jump = false;
    int cnt = 0x0;
    for(int i = 0x0;i < MGR_THREAD_MAX;++i)
    {
        if(0x0 <= mgr_array[i].tag_id){
            continue;
        }
        cnt++;
        if(!jump){
            first = i;jump = !jump;
        }
        break;
    }
    return !cnt;
}

static int find_array(int tag_id)
{
    if(false == check_unique(tag_id)){
        return FIND_MULTY;
    }
    //
    int rt = FIND_NOTHING;
    for(int i = 0x0;i < MGR_THREAD_MAX;++i)
    {
        if(0x0 > mgr_array[i].tag_id){
            continue;
        }
        if(tag_id != mgr_array[i].tag_id){
            continue;
        }
        rt = i;
    }
    return rt;
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


void init_index(int id)
{
    if(0x0 > id || MGR_THREAD_MAX < id){
        return;
    }
    
    pthread_mutex_lock(&mutex_mgr);
    memset(&mgr_array[id],0x0,sizeof(thread_mgr));
    //
    mgr_array[id].ctrl.mutex = PTHREAD_MUTEX_INITIALIZER;
    mgr_array[id].ctrl.mutex_pause = PTHREAD_MUTEX_INITIALIZER;
    mgr_array[id].ctrl.mutex_stop = PTHREAD_MUTEX_INITIALIZER;
    mgr_array[id].ctrl.status = s_thread_unknow;
    mgr_array[id].tag_id = -1;
    pthread_mutex_unlock(&mutex_mgr);
    return;
}


status_t add_param(int tag_id,thread_param_ptr param)
{
    if(NULL == param){
        P_PERROR(RT_ERROR,"input parameter param invalid");
        return RT_ERROR;
    }
    if(0x0 >= tag_id){
        P_PERROR(RT_ERROR,"input tag_id:%d invalid",tag_id);
        return RT_ERROR;
    }
    if(FIND_MULTY == find_array(tag_id)){
        P_PERROR(RT_ERROR,"input tag_id:%d invalid,it's exist multy item",tag_id);
        return RT_ERROR;
    }
    //
    int find_id = find_array(tag_id);
    if(FIND_NOTHING != find_id)
    {
        pthread_mutex_lock(&mutex_mgr);
        mgr_array[find_id].param = *param;
        pthread_mutex_unlock(&mutex_mgr);
        return 0x0;
    }
    if(true == check_full(find_id)){
        P_PERROR(RT_ERROR,"no empty array size can be used save parameter");
        return RT_ERROR;
    }
    pthread_mutex_lock(&mutex_mgr);
    mgr_array[find_id].param = *param;
    mgr_array[find_id].tag_id = tag_id;
    pthread_mutex_unlock(&mutex_mgr);
    return 0x0;
}

thread_param_ptr get_param(int tag_id)
{
    int find_id = find_array(tag_id);
    if(FIND_NOTHING == find_id || FIND_MULTY == find_id){
        P_PERROR(RT_ERROR,"search target tag_id:%d failed, from mgr array",tag_id);
        return NULL;
    }
    return &mgr_array[find_id].param;
}

status_t add_attr(int tag_id,thread_attr_ptr att)
{
    if(NULL == att){
        P_PERROR(RT_ERROR,"input parameter att invalid");
        return RT_ERROR;
    }
    if(0x0 >= tag_id){
        P_PERROR(RT_ERROR,"input tag_id:%d invalid",tag_id);
        return RT_ERROR;
    }
    if(FIND_MULTY == find_array(tag_id)){
        P_PERROR(RT_ERROR,"input tag_id:%d invalid,it's exist multy item",tag_id);
        return RT_ERROR;
    }
    //
    int find_id = find_array(tag_id);
    if(FIND_NOTHING != find_id)
    {
        pthread_mutex_lock(&mutex_mgr);
        mgr_array[find_id].attr = *att;
        pthread_mutex_unlock(&mutex_mgr);
        return 0x0;
    }
    if(true == check_full(find_id)){
        P_PERROR(RT_ERROR,"no empty array size can be used save parameter");
        return RT_ERROR;
    }
    pthread_mutex_lock(&mutex_mgr);
    mgr_array[find_id].attr = *att;
    mgr_array[find_id].tag_id = tag_id;
    pthread_mutex_unlock(&mutex_mgr);
    return 0x0;
}

thread_attr_ptr get_attr(int tag_id)
{
    int find_id = find_array(tag_id);
    if(FIND_NOTHING == find_id || FIND_MULTY == find_id){
        P_PERROR(RT_ERROR,"search target tag_id:%d failed, from mgr array",tag_id);
        return NULL;
    }
    return &mgr_array[find_id].attr;
}

thread_control_ptr get_ctrl(int tag_id)
{
    int find_id = find_array(tag_id);
    if(FIND_NOTHING == find_id || FIND_MULTY == find_id){
        P_PERROR(RT_ERROR,"search target tag_id:%d failed, from mgr array",tag_id);
        return NULL;
    }
    return &mgr_array[find_id].ctrl;
}


status_t add_func(int tag_id,thread_func func)
{
    if(NULL == func){
        P_PERROR(RT_ERROR,"input parameter func was NULL,tag_id:%d",tag_id);
    }
    if(0x0 >= tag_id){
        P_PERROR(RT_ERROR,"input tag_id:%d invalid",tag_id);
        return RT_ERROR;
    }
    if(FIND_MULTY == find_array(tag_id)){
        P_PERROR(RT_ERROR,"input tag_id:%d invalid,it's exist multy item",tag_id);
        return RT_ERROR;
    }
    //
    int find_id = find_array(tag_id);
    if(FIND_NOTHING != find_id)
    {
        pthread_mutex_lock(&mutex_mgr);
        mgr_array[find_id].func = func;
        pthread_mutex_unlock(&mutex_mgr);
        return 0x0;
    }
    if(true == check_full(find_id)){
        P_PERROR(RT_ERROR,"no empty array size can be used save parameter");
        return RT_ERROR;
    }
    pthread_mutex_lock(&mutex_mgr);
    mgr_array[find_id].func = func;
    mgr_array[find_id].tag_id = tag_id;
    pthread_mutex_unlock(&mutex_mgr);
    return 0x0;
}

thread_func get_func(int tag_id)
{
    int find_id = find_array(tag_id);
    if(FIND_NOTHING == find_id || FIND_MULTY == find_id){
        P_PERROR(RT_ERROR,"search target tag_id:%d failed, from mgr array",tag_id);
        return NULL;
    }
    return mgr_array[find_id].func;
}

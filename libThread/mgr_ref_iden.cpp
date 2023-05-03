/*!
 * @file mgr_ref_iden.cpp
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


status_t init_index_by_iden(int identify)
{
    int temp_index = find_array(identify);
    if(FIND_NOTHING == temp_index || FIND_MULTY == temp_index){
        P_PERROR(RT_ERROR,"search target identify:%d failed, from mgr array",identify);
        return RT_ERROR;
    }
    //
    init_index(temp_index);
    return 0x0;
}


status_t set_param_by_iden(int identify,thread_param_ptr param)
{
    if(NULL == param){
        P_PERROR(RT_ERROR,"input parameter param invalid");
        return RT_ERROR;
    }
    if(0x0 >= identify){
        P_PERROR(RT_ERROR,"input identify:%d invalid",identify);
        return RT_ERROR;
    }
    if(FIND_MULTY == find_array(identify)){
        P_PERROR(RT_ERROR,"input identify:%d invalid,it's exist multy item",identify);
        return RT_ERROR;
    }
    //
    int temp_index = find_array(identify);
    if(FIND_NOTHING != temp_index)
    {
        set_param(temp_index,param);
        return 0x0;
    }
    if(true == check_full(temp_index)){
        P_PERROR(RT_ERROR,"no empty array size can be used save parameter");
        return RT_ERROR;
    }
    set_param(temp_index,param);
    set_iden(temp_index,identify);
    return 0x0;
}


thread_param_ptr get_param_by_iden(int identify)
{
    int temp_index = find_array(identify);
    if(FIND_NOTHING == temp_index || FIND_MULTY == temp_index){
        P_PERROR(RT_ERROR,"search target identify:%d failed, from mgr array",identify);
        return NULL;
    }
    return get_param(temp_index);
}


status_t set_attr_by_iden(int identify,thread_attr_ptr attr)
{
    if(NULL == attr){
        P_PERROR(RT_ERROR,"input parameter att invalid");
        return RT_ERROR;
    }
    if(0x0 >= identify){
        P_PERROR(RT_ERROR,"input identify:%d invalid",identify);
        return RT_ERROR;
    }
    if(FIND_MULTY == find_array(identify)){
        P_PERROR(RT_ERROR,"input identify:%d invalid,it's exist multy item",identify);
        return RT_ERROR;
    }
    //
    int temp_index = find_array(identify);
    if(FIND_NOTHING != temp_index)
    {
        set_attr(temp_index,attr);
        return 0x0;
    }
    if(true == check_full(temp_index)){
        P_PERROR(RT_ERROR,"no empty array size can be used save parameter");
        return RT_ERROR;
    }
    set_attr(temp_index,attr);
    set_iden(temp_index,identify);
    return 0x0;
}


thread_attr_ptr get_attr_by_iden(int identify)
{
    int temp_index = find_array(identify);
    if(FIND_NOTHING == temp_index || FIND_MULTY == temp_index){
        P_PERROR(RT_ERROR,"search target identify:%d failed, from mgr array",identify);
        return NULL;
    }
    return get_attr(temp_index);
}


thread_control_ptr get_ctrl_by_iden(int identify)
{
    int temp_index = find_array(identify);
    if(FIND_NOTHING == temp_index || FIND_MULTY == temp_index){
        P_PERROR(RT_ERROR,"search target identify:%d failed, from mgr array",identify);
        return NULL;
    }
    return get_ctrl(temp_index);
}


status_t set_func_by_iden(int identify,thread_func func)
{
    if(NULL == func){
        P_PERROR(RT_ERROR,"input parameter func was NULL,identify:%d",identify);
    }
    if(0x0 >= identify){
        P_PERROR(RT_ERROR,"input identify:%d invalid",identify);
        return RT_ERROR;
    }
    if(FIND_MULTY == find_array(identify)){
        P_PERROR(RT_ERROR,"input identify:%d invalid,it's exist multy item",identify);
        return RT_ERROR;
    }
    //
    int temp_index = find_array(identify);
    if(FIND_NOTHING != temp_index)
    {
        set_func(temp_index,func);
        return 0x0;
    }
    if(true == check_full(temp_index)){
        P_PERROR(RT_ERROR,"no empty array size can be used save parameter");
        return RT_ERROR;
    }
    set_func(temp_index,func);
    set_iden(temp_index,identify);
    return 0x0;
}


thread_func get_func_by_iden(int identify)
{
    int temp_index = find_array(identify);
    if(FIND_NOTHING == temp_index || FIND_MULTY == temp_index){
        P_PERROR(RT_ERROR,"search target identify:%d failed, from mgr array",identify);
        return NULL;
    }
    return get_func(temp_index);
}


status_t set_tid_by_iden(int identify,pthread_t tid)
{
    if(0x0 >= identify){
        P_PERROR(RT_ERROR,"input identify:%d invalid",identify);
        return RT_ERROR;
    }
    if(FIND_MULTY == find_array(identify)){
        P_PERROR(RT_ERROR,"input identify:%d invalid,it's exist multy item",identify);
        return RT_ERROR;
    }
    //
    int temp_index = find_array(identify);
    if(FIND_NOTHING != temp_index)
    {
        set_tid(temp_index,tid);
        return 0x0;
    }
    if(true == check_full(temp_index)){
        P_PERROR(RT_ERROR,"no empty array size can be used save parameter");
        return RT_ERROR;
    }
    set_tid(temp_index,tid);
    set_iden(temp_index,identify);
    return 0x0;
}


pthread_t get_tid_by_iden(int identify)
{
    int temp_index = find_array(identify);
    if(FIND_NOTHING == temp_index || FIND_MULTY == temp_index){
        P_PERROR(RT_ERROR,"search target identify:%d failed, from mgr array",identify);
        return MGR_TID_DEFAULT;
    }
    return get_tid(temp_index);
}

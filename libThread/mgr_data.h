/*!
 * @file mgr_data.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _MGR_DATA_H_
#define _MGR_DATA_H_

#include "ThreadHeader.h"
#include "internal_type.h"

#ifdef __cplusplus
extern "C"{
#endif



//! mgr array max size
#define MGR_THREAD_MAX 128


//! mgr array member default value
#define MGR_IDEN_DEFAULT -1
#define MGR_TID_DEFAULT 0x0

/**
 * @brief thread manager information data
 * 
 */
typedef struct _thread_mgr
{
    thread_func func;               //! child thread main function
    thread_control ctrl;            //! control
    thread_param param;             //! param
    thread_attr attr;               //! attribution
    int iden;                       //! unique,identify.default value:-1
    pthread_t tid;                  //! unique,thread id.default value:0
}thread_mgr;



/**
 * @brief initialize mgr array item.links delete operation
 * 
 * @param index 
 */
void init_index(int index);
/**
 * @brief initialize mgr array,only can use once while first initialize
 * 
 */
void frist_init_array();


/**
 * @brief add pthread parameter data into mgr array
 * 
 * @param index 
 * @param param 
 * @return status_t 
 */
void set_param(int index,thread_param_ptr param);
thread_param_ptr get_param(int index);

/**
 * @brief add pthread attribution data into mgr array
 * 
 * @param index 
 * @param att 
 * @return status_t 
 */
void set_attr(int index,thread_attr_ptr attr);
thread_attr_ptr get_attr(int index);

/**
 * @brief Get the ctrl object
 * 
 * @param index 
 * @return thread_control_ptr 
 */
void set_ctrl(int index,thread_control_ptr ctrl);
thread_control_ptr get_ctrl(int index);

/**
 * @brief 
 * 
 * @param index 
 * @param func 
 * @return status_t 
 */
void set_func(int index,thread_func func);
thread_func get_func(int index);


/*!
 * @brief 
 * 
 * @param index 
 * @param tid 
 */
void set_tid(int index,pthread_t tid);
pthread_t get_tid(int index);


/*!
 * @brief 
 * 
 * @param index 
 * @param identify 
 */
void set_iden(int index,int identify);
int get_iden(int index);



#ifdef __cplusplus
}
#endif
#endif

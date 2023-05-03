/**
 * @file ThreadHeader.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>


#ifndef _THREADHEADER_H_
#define _THREADHEADER_H_


// #ifdef __cplusplus
// extern "C" {
// #endif


#ifdef GENERATE_LIB
#define API extern
#else
#define API
#endif

#define RT_ERROR ((int)__LINE__)
#define DATA_ERROR ((int)-1)
typedef int status_t;


#define FIND_NOTHING ((int)-1)
#define FIND_MULTY ((int)-MGR_THREAD_MAX)
#define ARRAY_FULL FIND_NOTHING



//! thread control status enum
typedef enum
{
    s_thread_running = 0x1,
    s_thread_pause,
    s_thread_exit,
    s_thread_not_exist,
    s_thread_error,
    s_thread_unknow = 0x0
}thread_ctrl_id;

//! work thread function point
typedef void* (*thread_func)(void* param);
//! user work function point
typedef int (*user_func)(void* param);

/**
 * @brief child thread parameter
 * @struct
 */
typedef struct thread_param{
    int exit_code;					//!< work thread exit code
    pthread_t thread_id;			//!< work thread id
    pid_t thread_pid;				//!< work thread belong to process id
    char thread_name[256];			//!< work thread name
	void* global_data;				//!< global data point
	void* remain_ptr;				//!< remain data point,can't use this member,it's will be set thread_ctrl_ptr
    void* resource_ptr;             //!< resource infomation data point
    user_func userfunc_ptr;         //!< user function point,can't use this member,it's will be set thread_ctrl_ptr
}thread_param,*thread_param_ptr;

/**
 * @brief thread attribution information data
 * 
 */
typedef struct thread_attr{
    pthread_attr_t attr;                //!thread attribuction parameter
}thread_attr,*thread_attr_ptr;


/**
 * @brief create child thread use data from user input and bind an unique tag_id by user input
 * 
 * @param func 
 * @param param 
 * @param tag_id
 * @return status_t 
 */
API status_t thread_ready_param(thread_func func,thread_param_ptr param,int tag_id);

/**
 * @brief ready create thread attribution information data and bing an unique tag_id bu user input
 * 
 * @param func 
 * @param attr_ptr 
 * @param tag_id 
 * @return API 
 */
API status_t thread_ready_attribute(thread_func func,pthread_attr_t* attr_ptr,int tag_id);

/**
 * @brief create child thread and start it
 * 
 * @param tag_id 
 * @return API 
 */
API status_t thread_create_and_start(int tag_id);

/**
 * @brief ready crate thread reference parameter and attribution data,then create thread and start it
 * 
 * @param func 
 * @param param 
 * @param tag_id 
 * @param sched_type 
 * @param priority_level 
 * @return stats_t  0x0-success,other-failed
 * @note this function used by support typical three scene 
 */
API status_t thread_ready_and_start(thread_func func,thread_param_ptr param,int tag_id,int sched_type,int priority_level);

/**
 * @brief 
 * 
 * @param tag_id 
 * @return API 
 */
API status_t thread_release_and_destroy(int tag_id);



/**
 * @brief pause thread running
 * 
 * @param tag_id 
 * @param pause 
 * @return API 
 */
API status_t thread_pause(int tag_id);

/**
 * @brief resume thread running
 * 
 * @param tag_id 
 * @return API 
 */
API status_t thread_resume(int tag_id);

/**
 * @brief get current thread running status
 * 
 * @param tag_id 
 * @return API 
 */
API thread_ctrl_id thread_current_status(int tag_id);





/**
 * @brief check current environment support runtime shced policy
 * 
 * @return API 
 */
API bool thread_check_runtime_support();

/*!
 * @brief check thread control data struct member stop value
 * 
 * @param tag_id 
 * @return API 
 */
API bool thread_check_exit(int tag_id);
API bool thread_check_exit();

/*!
 * @brief check thread control data struct member pause value
 * 
 * @param tag_id 
 * @return API 
 */
API bool thread_check_pause(int tag_id);
API bool thread_check_pause();


// #ifdef __cplusplus
// }
// #endif


#endif
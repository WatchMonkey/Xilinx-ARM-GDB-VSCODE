/**
 * @file thread_internal_type.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "ThreadHeader.h"


#ifndef _THREAD_INTERNAL_H_
#define _THREAD_INTERNAL_H_


#ifdef __cplusplus
extern "C" {
#endif


#define MGR_THREAD_MAX 128


/**
 * @brief thread running status information
 * 
 */
typedef struct _thread_control{
    pthread_mutex_t mutex_stop;		//!< stop mutex
	bool stop;					    //!< stop work thread flag
	pthread_mutex_t mutex_pause;	//!< pause mutex
	bool pause;				        //!< pause work thread flag
    thread_ctrl_id status;          //! thread running status
    pthread_mutex_t mutex;          //! thread control mutex
}thread_control,*thread_control_ptr;



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
    int tag_id;                     //! unuque tag_id.default value:-1
}thread_mgr;


/**
 * @brief initialize mgr array,only can use once while first initialize
 * 
 */
void frist_init_array();

/**
 * @brief initialize mgr array item.links delete operation
 * 
 * @param id 
 */
void init_index(int id);

/**
 * @brief add pthread parameter data into mgr array
 * 
 * @param tag_id 
 * @param param 
 * @return status_t 
 */
status_t add_param(int tag_id,thread_param_ptr param);
thread_param_ptr get_param(int tag_id);

/**
 * @brief add pthread attribution data into mgr array
 * 
 * @param tag_id 
 * @param att 
 * @return status_t 
 */
status_t add_attr(int tag_id,thread_attr_ptr att);
thread_attr_ptr get_attr(int tag_id);

/**
 * @brief Get the ctrl object
 * 
 * @param tag_id 
 * @return thread_control_ptr 
 */
thread_control_ptr get_ctrl(int tag_id);

/**
 * @brief 
 * 
 * @param tag_id 
 * @param func 
 * @return status_t 
 */
status_t add_func(int tag_id,thread_func func);
thread_func get_func(int tag_id);


#ifdef __cplusplus
}
#endif

#endif

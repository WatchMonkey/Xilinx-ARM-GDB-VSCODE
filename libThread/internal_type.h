/*!
 * @file internal_type.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _INTERNAL_TYPE_H_
#define _INTERNAL_TYPE_H_



#include "ThreadHeader.h"


#ifdef __cplusplus
extern "C" {
#endif


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



#ifdef __cplusplus
}
#endif

#endif

/*!
 * @file thread_internal.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _THREAD_INTERNAL_H_
#define _THREAD_INTERNAL_H_

#include "mgr_header.h"

#ifdef __cplusplus
extern "C"{
#endif


/*!
 * @brief set thread control data member:stop flag
 * 
 * @param tag_id 
 * @return status_t 
 */
status_t internal_thread_start(int tag_id);


/*!
 * @brief set thread control data member:stop flag
 * 
 * @param tag_id 
 * @return status_t 
 */
status_t internal_thread_stop(int tag_id);


#ifdef __cplusplus
}
#endif

#endif
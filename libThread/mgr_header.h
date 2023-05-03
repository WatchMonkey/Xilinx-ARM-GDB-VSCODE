/*!
 * @file mgr_internal.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "ThreadHeader.h"
#include "internal_type.h"


#ifndef _MGR_INTERNAL_H_
#define _MGR_INTERNAL_H_


#ifdef __cplusplus
extern "C" {
#endif


/*!
 * @brief check identify unique
 * 
 * @param identify 
 * @return true 
 * @return false 
 */
bool check_unique(int identify);

/*!
 * @brief check mgr array was full
 * 
 * @param first 
 * @return true 
 * @return false 
 */
bool check_full(int& first);

/*!
 * @brief find index in mgr array reference identify
 * 
 * @param identify 
 * @return int 
 */
int find_array(int identify);


/*!
 * @brief 
 * 
 * @param identify 
 * @return status_t 
 */
status_t init_index_by_iden(int identify);




/*!
 * @brief Set the param by iden object
 * 
 * @param identify 
 * @param param 
 * @return status_t 
 */
status_t set_param_by_iden(int identify,thread_param_ptr param);
thread_param_ptr get_param_by_iden(int identify);


/*!
 * @brief Set the attr by iden object
 * 
 * @param identify 
 * @param attr 
 * @return status_t 
 */
status_t set_attr_by_iden(int identify,thread_attr_ptr attr);
thread_attr_ptr get_attr_by_iden(int identify);


/*!
 * @brief Get the ctrl by iden object
 * 
 * @param identify 
 * @return thread_control_ptr 
 */
thread_control_ptr get_ctrl_by_iden(int identify);


/*!
 * @brief Set the func by iden object
 * 
 * @param identify 
 * @param func 
 * @return status_t 
 */
status_t set_func_by_iden(int identify,thread_func func);
thread_func get_func_by_iden(int identify);


/*!
 * @brief Set the tid by iden object
 * 
 * @param identify 
 * @param tid 
 * @return status_t 
 */
status_t set_tid_by_iden(int identify,pthread_t tid);
pthread_t get_tid_by_iden(int identify);


/*!
 * @brief 
 * 
 * @param tid 
 * @return int 
 */
int find_index_by_tid(pthread_t tid);


#ifdef __cplusplus
}
#endif

#endif

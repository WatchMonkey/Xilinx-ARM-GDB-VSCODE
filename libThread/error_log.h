/**
 * @file error_log.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-04-28
 * 
 * @copyright Copyright (c) 2023
 * 
 */



#ifndef _ERROR_LOG_H_
#define _ERROR_LOG_H_

#ifdef _cplusplus
extern "C"{
#endif


#define ERR_STRING_LENGTH_MAX 1024



/**
 * @brief Set the err info object
 * 
 * @param id 
 * @param format 
 * @param ... 
 */
void set_err_info(int id,char* format,...);

/**
 * @brief Get the err string object
 * 
 * @return char* 
 */
char* get_err_string();

/**
 * @brief Get the err code object
 * 
 * @return int 
 */
int get_err_code();



//! save error information
#define P_PERROR(id,format,...) set_err_info(id,format,##__VA_ARGS__)
//! get error code
#define P_PERROR_CODE ((int)get_err_code())
#define P_PERROR_STR (get_err_string())


#ifdef _cplusplus
}
#endif

#endif

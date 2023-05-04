/*!
 * @file process_op.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef _PROCESS_OP_H_
#define _PROCESS_OP_H_

#ifdef __cplusplus
extern "C"{
#endif


#define RT_ERROR ((int)__LINE__)
#define DATA_ERROR ((int)-1)


typedef enum {
	ad_rx = 0x1, ad_tx = 0x2
} ad_dir_t;
typedef enum {
	simu_exe_running = 0x1, simu_exe_idle = 0x2
} simu_exe_flag_t;
typedef enum {
	simu_resp_operate_success = 0x1,
	simu_resp_child_process_failed = 0x2,
	simu_resp_stop_process_failed = 0x3,
	simu_resp_exe_file_not_exist = 0x4,
	simu_resp_file_primite_invalid = 0x5
} simu_resp_info_t;



/**
 * @brief		simulate application running status
 * @return		int
 */
int simu_exe_flag();
void simu_exe_flag_idle();
void simu_exe_flag_running(int pid, char* cmd);

/*!
 * @brief 
 * 
 * @param elf_file 
 * @return int 
 */
int start_simu_exe(const char* elf_file);

/*!
 * @brief 
 * 
 * @param elf_file 
 * @return int 
 */
int stop_simu_exe(const char* elf_file);

/*!
 * @brief 
 * 
 * @param elf_file 
 * @return int 
 */
int restart_simu_exe(const char* elf_file);


#ifdef __cplusplus
}
#endif
#endif

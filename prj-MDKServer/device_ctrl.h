/*
 * device_ctrl.h
 *
 *  Created on: 2022Äê11ÔÂ28ÈÕ
 *      Author: Administrator
 */

#ifndef SRC_DEVICE_CTRL_H_
#define SRC_DEVICE_CTRL_H_

#include <stdbool.h>
#include <stdint.h>

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
 * @brief		reconfigure fpga bit file
 * @return		int
 * @retval		0x0		success
 * 				other	failed
 */
int fpga_reconfigure(const char* filepath);

/**
 * @brief		reconfigure fpga bit file
 * @return		int
 * @retval		0x0		success
 * 				other	failed
 */
int ad_reset();

/**
 * @brief		ad9361 config frequency
 * @param		dir		direction
 * @return		int
 * @retval		0x0		success
 * 				other	failed
 */
int ad9361_set_freq(int id,int dir, int64_t freq);

/**
 * @brief		ad9361 config bandwidth
 * @param		dir		direction
 * @return		int
 * @retval		0x0		success
 * 				other	failed
 */
int ad9361_set_bd(int id,int dir, int64_t bd);
int ad9361_set_rxgain(int id,int gain);
int ad9361_set_rxgain_mod(int id,int gain_mod);
int ad9361_set_att(int id,int att);

/**
 * @brief		simulate application running status
 * @return		int
 */
int simu_exe_flag();
void simu_exe_flag_idle();
void simu_exe_flag_running(int pid, char* cmd);

int start_simu_exe(const char* elf_file);

int stop_simu_exe(const char* elf_file);

int restart_simu_exe(const char* elf_file);

#endif /* SRC_DEVICE_CTRL_H_ */

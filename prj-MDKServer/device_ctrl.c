/*
 * device_ctrl.c
 *
 *  Created on: 2022年11月28日
 *      Author: Administrator
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>

#include "LogHeader.h"
#include "device_ctrl.h"

#include "ad_func_internal_header.h"
#include "sysfs_op.h"
#include "global_macro.h"

#define RT_ERROR ((int)__LINE__)
#define XDEVCFG_FILE "/dev/xdevcfg"
#define ENABLE_FPGA_DEVICE 0x1
#define ENABLE_AD_DEVICE 0x1
#define ENABLE_CHILD_PROCESS 0x1

static int handler_sysctrl(const char* data,int size)
{
	int64_t temp_int = strtoull(data,NULL,10);
	DLog("read byte value:%llu",temp_int);
	return 0x0;
}

//!	目标ID序号
static int global_ad_file_index = 0x0;
//!
static int global_simu_exe_flag = simu_exe_idle;
static pid_t global_child_pid = 0x0;
static char* global_child_cmd = NULL;
static char sys_info_buf[0x400] = {0x0};

int fpga_reconfigure(const char* bin_file)
{
	int temp_status = 0x0;
	if(NULL == bin_file){
		ELog("input parameter invalid");
		return RT_ERROR;
	}

#if ENABLE_FPGA_DEVICE
	//detect file exist
	int temp_exist = access(XDEVCFG_FILE,R_OK);
	if(temp_exist){
		ELog("file path:%s not exist",XDEVCFG_FILE);
		return RT_ERROR;
	}
	temp_exist = access(bin_file,R_OK);
	if(temp_exist){
		ELog("file path:%s not exist",bin_file);
		return RT_ERROR;
	}

	//open bin_file
	FILE* temp_file_source = fopen(bin_file,"r");
	if(temp_file_source == NULL){
		ELog("file:%s open failed:%s",bin_file,strerror(errno));
		return RT_ERROR;
	}else{
		ILog("file:%s open success",bin_file);
		DLog("file:%s open success",bin_file);
	}
	//open dev file
	FILE* temp_file_dest = fopen(XDEVCFG_FILE,"w");
	if(temp_file_dest == NULL){
		fclose(temp_file_source);
		ELog("file:%s failed:%s",XDEVCFG_FILE,strerror(errno));
		return RT_ERROR;
	}else{
		ILog("file:%s open success",XDEVCFG_FILE);
		DLog("file:%s open success",XDEVCFG_FILE);
	}

	//write data
	char* temp_bit_buffer[1024];
	int temp_read_size = 0x0;
	int temp_read_count = 0x0;
	int temp_write_size = 0x0;
	int temp_write_count = 0x0;
	while(1)
	{
		if(feof(temp_file_source))
			break;
		memset(temp_bit_buffer,0x0,1024);
		//read bin file data
		temp_read_size = fread(temp_bit_buffer,sizeof(char),1024,temp_file_source);
		if(temp_read_size <= 0x0){
			temp_status = RT_ERROR;
			ELog("read target bit:%s failed:%s",bin_file,strerror(errno));
			break;
		}else{
			temp_read_count += temp_read_size;
		}
		//write data into device file
		temp_write_size = fwrite(temp_bit_buffer,sizeof(char),temp_read_size,temp_file_dest);
		fflush(temp_file_dest);
		if(temp_write_size <= 0x0){
			temp_status = RT_ERROR;
			ELog("write data to %s failed:%s",XDEVCFG_FILE,strerror(errno));
			break;
		}else{
			temp_write_count += temp_write_size;
		}
	}
	//close file
	fclose(temp_file_source);
	fclose(temp_file_dest);
	//
	if(temp_status){
		ELog("FPGA Reconfig bit:%s Failed,Read Bit File %uByte,Write Dec File %uByte",bin_file,temp_read_count,temp_write_count);
	}else{
		ILog("FPGA Reconfig bit:%s Success,Read Bit File %uByte,Write Dec File %uByte",bin_file,temp_read_count,temp_write_count);
		DLog("FPGA Reconfig bit:%s Success,Read Bit File %uByte,Write Dec File %uByte",bin_file,temp_read_count,temp_write_count);
	}
#else
	ILog("FPGA Reconfig:%s",bin_file);
	temp_status = 0x0;
#endif

	return temp_status;
}

int ad_reset()
{
	int temp_rt = 0x0;

#if ENABLE_AD_DEVICE
	/*
	char temp_shell[0x200] = { 0x0 };
	sprintf(temp_shell,"echo 1 > /sys/kernel/debug/iio/iio:device%d/initialize",global_ad_file_index);
	temp_rt = system(temp_shell);
	if (0x0 > temp_rt || 127 == temp_rt) {
		ELog("system function failed");
		return RT_ERROR;
	}*/
	{
		sys_info_s sys_info;
		char temp_sys_path_array[0x400] = {0x0};
		sprintf(temp_sys_path_array,"%s/%s",get_ad_debugfs(AD_ONE_ID),"initialize");
		sys_info.sysfile_path = temp_sys_path_array;

		sys_info.read_func = handler_sysctrl;
		sys_info.write_string = "1";
		int temp_sys = sysctrl_write(&sys_info);
		if(temp_sys){
			ELog("write sysctrl data failed");
			return RT_ERROR;
		}else{
			usleep(1000000);
		}
	}

	//
	{
		sys_info_s sys_info;
		char temp_sys_path_array[0x400] = {0x0};
		sprintf(temp_sys_path_array,"%s/%s",get_ad_debugfs(AD_TWO_ID),"initialize");
		sys_info.sysfile_path = temp_sys_path_array;

		sys_info.read_func = handler_sysctrl;
		sys_info.write_string = "1";
		int temp_sys = sysctrl_write(&sys_info);
		if(temp_sys){
			ELog("write sysctrl data failed");
			return RT_ERROR;
		}else{
			usleep(1000000);
		}
	}
#endif

	return 0x0;
}






int ad9361_set_freq(int id,int dir,int64_t freq)
{
	int temp_rt = 0x0;

#if ENABLE_AD_DEVICE
	//
	sys_info_s sys_info;
	memset(sys_info_buf,0x0,sizeof(sys_info_buf));
	sprintf(sys_info_buf,"%lld",freq);
	//
	char temp_sys_path_array[0x400] = {0x0};
	if(ad_rx == dir){
		sprintf(temp_sys_path_array,"%s/%s",get_ad_home(id),"out_altvoltage0_RX_LO_frequency");
		//sys_info.sysfile_path = "/sys/bus/iio/devices/iio:device0/out_altvoltage0_RX_LO_frequency";
	}else if(ad_tx == dir){
		sprintf(temp_sys_path_array,"%s/%s",get_ad_home(id),"out_altvoltage1_TX_LO_frequency");
		//sys_info.sysfile_path = "/sys/bus/iio/devices/iio:device0/out_altvoltage1_TX_LO_frequency";
	}else{
		return RT_ERROR;
	}
	sys_info.sysfile_path = temp_sys_path_array;
	sys_info.read_func = handler_sysctrl;
	sys_info.write_string = &sys_info_buf[0];
	int temp_sys = sysctrl_write(&sys_info);
	if(temp_sys){
		ELog("write sysfs control data failed");
		return RT_ERROR;
	}
#else
	ILog("parameter,dir:%d,freq:%lld",dir,freq);
#endif

	return temp_rt;
}


int ad9361_set_bd(int id,int dir,int64_t bd)
{
	int temp_rt = 0x0;

#if ENABLE_AD_DEVICE
	//
	sys_info_s sys_info;
	memset(sys_info_buf,0x0,sizeof(sys_info_buf));
	sprintf(sys_info_buf,"%lld",bd);
	//
	char temp_sys_path_array[0x400] = {0x0};
	if(ad_rx == dir){
		sprintf(temp_sys_path_array,"%s/%s",get_ad_home(id),"in_voltage_rf_bandwidth");
		//sys_info.sysfile_path = "/sys/bus/iio/devices/iio:device0/in_voltage_rf_bandwidth";
	}else if(ad_tx == dir){
		sprintf(temp_sys_path_array,"%s/%s",get_ad_home(id),"out_voltage_rf_bandwidth");
		//sys_info.sysfile_path = "/sys/bus/iio/devices/iio:device0/out_voltage_rf_bandwidth";
	}else{
		return RT_ERROR;
	}
	sys_info.sysfile_path = temp_sys_path_array;
	sys_info.read_func = handler_sysctrl;
	sys_info.write_string = &sys_info_buf[0];
	int temp_sys = sysctrl_write(&sys_info);
	if(temp_sys){
		ELog("write sysfs control data failed");
		return RT_ERROR;
	}
#else
	ILog("parameter,dir:%d,bd:%lld",dir,bd);
#endif

	return temp_rt;
}

int ad9361_set_rxgain(int id,int gain)
{
	//int convert to string
	char temp_strValue[0x20] = { 0x0 };
	if (0x0 >= sprintf(temp_strValue, "0x%x", gain)) {
		ELog("input value invalid");
		return RT_ERROR;
	}
	//generate string
	memset(sys_info_buf, 0x0, sizeof(sys_info_buf));
	strcat(sys_info_buf, "0x109 ");
	strcat(sys_info_buf, temp_strValue);
	//
	sys_info_s sys_info;
	memset(sys_info_buf,0x0,sizeof(sys_info_buf));
	char temp_sys_path_array[0x400] = {0x0};
	sprintf(temp_sys_path_array,"%s/%s",get_ad_debugfs(id),"direct_reg_access");
	sys_info.sysfile_path = temp_sys_path_array;
	sys_info.read_func = NULL;
	sys_info.write_string = &sys_info_buf[0];
	int temp_sys = sysctrl_write(&sys_info);
	if (temp_sys) {
		ELog("write sysfs control data failed");
		return RT_ERROR;
	}

	return 0x0;
}

int ad9361_set_rxgain_mod(int id,int gain_mod)
{
	//generate string
	memset(sys_info_buf, 0x0, sizeof(sys_info_buf));
	if(0x2 == gain_mod){
		strcat(sys_info_buf, "0xFA 0xE0");
		strcat(sys_info_buf, ",");
		strcat(sys_info_buf, "0xFB 0x08");
	}else if(0x1 == gain_mod){
		strcat(sys_info_buf, "0xFA 0xEA");
	}else{
		return RT_ERROR;
	}
	//
	sys_info_s sys_info;
	memset(sys_info_buf,0x0,sizeof(sys_info_buf));
	char temp_sys_path_array[0x400] = {0x0};
	sprintf(temp_sys_path_array,"%s/%s",get_ad_debugfs(id),"direct_reg_access");
	sys_info.sysfile_path = temp_sys_path_array;
	sys_info.read_func = NULL;
	sys_info.write_string = &sys_info_buf[0];
	int temp_sys = sysctrl_write(&sys_info);
	if (temp_sys) {
		ELog("write sysfs control data failed");
		return RT_ERROR;
	}

	return 0x0;
}

int ad9361_set_att(int id,int att)
{
	//int convert to string
	char temp_strValue[0x20] = { 0x0 };
	if (0x0 >= sprintf(temp_strValue, " 0x%x", (att & 0x000000ff))) {
		ELog("input value invalid");
		return RT_ERROR;
	}
	char temp_strValue_2[0x20] = { 0x0 };
	if (0x0 >= sprintf(temp_strValue_2, " 0x%x",((att & 0x00000100) >> 0x8))) {
		ELog("input value invalid");
		return RT_ERROR;
	}
	//generate string
	memset(sys_info_buf, 0x0, sizeof(sys_info_buf));
	strcat(sys_info_buf, "0x73");
	strcat(sys_info_buf, temp_strValue);
	strcat(sys_info_buf, ",");
	strcat(sys_info_buf, "0x74");
	strcat(sys_info_buf, temp_strValue_2);
	strcat(sys_info_buf, ",");
	strcat(sys_info_buf, "0x7C 0x40");
	//
	sys_info_s sys_info;
	memset(sys_info_buf,0x0,sizeof(sys_info_buf));
	char temp_sys_path_array[0x400] = {0x0};
	sprintf(temp_sys_path_array,"%s/%s",get_ad_debugfs(id),"direct_reg_access");
	sys_info.sysfile_path = temp_sys_path_array;
	sys_info.read_func = NULL;
	sys_info.write_string = &sys_info_buf[0];
	int temp_sys = sysctrl_write(&sys_info);
	if (temp_sys) {
		ELog("write sysfs control data failed");
		return RT_ERROR;
	}

	return 0x0;
}





int simu_exe_flag()
{
	return global_simu_exe_flag;
}
char* simu_exe_cmd()
{
	return global_child_cmd;
}
void simu_exe_flag_idle()
{
	global_simu_exe_flag = simu_exe_idle;
	global_child_pid = 0x0;
	if(NULL != global_child_cmd){
		free(global_child_cmd);
		global_child_cmd = NULL;
	}
}
void simu_exe_flag_running(int pid,char* cmd)
{
	global_child_pid = pid;
	if(NULL != global_child_cmd){
		free(global_child_cmd);
		global_child_cmd = NULL;
	}
	global_child_cmd = strdup(cmd);
	global_simu_exe_flag = simu_exe_running;
}
int start_simu_exe(const char* elf_file)
{
	int temp_rt = 0x0;
	if(NULL == elf_file){
		ELog("input parameter invalid");
		return RT_ERROR;
	}
	if(simu_exe_running == simu_exe_flag())
	{
		ELog("simulate application already running");
		return RT_ERROR;
	}
	//detect file exist
	int temp_exist = access(elf_file,R_OK);
	if(temp_exist){
		ELog("file path:%s not exist",elf_file);
		return RT_ERROR;
	}
	//detect file permission
	int temp_exec = access(elf_file,X_OK);
	if(temp_exec)
	{
		int temp_rt = chmod(elf_file, S_IRWXU);
		if(temp_rt){
			ELog("file path:%s change mode failed",elf_file);
			return RT_ERROR;
		}else{
			DLog("change file:%s mode success",elf_file);
			ILog("change file:%s mode success",elf_file);
		}
	}
	temp_exec = access(elf_file,X_OK);
	if(temp_exec)
	{
		ELog("file path:%s can't execute",elf_file);
		return RT_ERROR;
	}

	//
	pid_t pid = fork();
	if(pid == -1)
	{
		ELog("fork failed");
		return RT_ERROR;
	}
	else if(pid == 0){
		//如果是子进程
		DLog("This is child process, and the pid = %u, the ppid = %u",getpid( ),getppid( ));
		//
		int temp_rt = execv(elf_file, NULL);
		if(temp_rt){
			ELog("execv failed.%d:%s",errno,strerror(errno));
			//return RT_ERROR;
		}
		//
		DLog("child process:%d exit",getpid());
		ILog("child process:%d exit",getpid());
		exit(-1);
	}else{
		//如果是父进程
		DLog("This is parent process, and the pid = %u, the ppid = %u",getpid( ),getppid( ));
		//
		//global_child_pid = pid;
		simu_exe_flag_running(pid,elf_file);
		//global_simu_exe_flag = simu_exe_running;
	}

	return temp_rt;
}
int stop_simu_exe(const char* elf_file)
{
	int temp_rt = 0x0;
	/*if(NULL == elf_file){
		ELog("input parameter invalid");
		return RT_ERROR;
	}*/
	if(simu_exe_idle == simu_exe_flag())
	{
		ELog("simulate application not running");
		return RT_ERROR;
	}
	if(NULL == elf_file){
		ELog("input parameter invalid");
		return RT_ERROR;
	}
	if(strcmp(elf_file,simu_exe_cmd())){
		ELog("input parameter invalid");
		return RT_ERROR;
	}

	int status = 0x0;
	if(0 == (waitpid( global_child_pid, &status, WNOHANG)))
	{
		int retval = kill( global_child_pid,SIGKILL);
		if ( retval )
		{
			waitpid( global_child_pid, &status, 0 );
		}else{
			ILog( "%d killed", global_child_pid );
			DLog( "%d killed", global_child_pid );
			//global_child_pid = 0x0;
			simu_exe_flag_idle();
			//global_simu_exe_flag = simu_exe_idle;
		}
	}

	return temp_rt;
}
int restart_simu_exe(const char* elf_file)
{
	stop_simu_exe(elf_file);
	int temp_rt = start_simu_exe(elf_file);
	return temp_rt;
}

/*!
 * @file process_op.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "process_op.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "LogHeader.h"


//!
static int global_simu_exe_flag = simu_exe_idle;
static pid_t global_child_pid = 0x0;
static char* global_child_cmd = NULL;


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
		simu_exe_flag_running(pid,(char*)elf_file);
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

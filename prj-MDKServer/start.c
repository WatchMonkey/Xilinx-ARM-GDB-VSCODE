/*!
 * @file start.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <signal.h>             // Signal handling functions

#include "network_op.h"
#include "device_ctrl.h"
#include "ad_func_impl.h"

#include "ad_func_internal_header.h"
#include "LogHeader.h"
#include "global_macro.h"

#define RT_ERROR ((int)__LINE__)
#define DATA_ERROR ((int)-1)

#define ENABLE_DAEMON 0x0


/*!
 * @brief		创建守护进程
 * @param		bool
 * @return		int
 * 				0x0-success
 * 				other-failed
 */
int switch_daemon(bool enable)
{
	if(!enable)
		return 0x0;
	else
		fprintf(stdout,"switch process into daemon process\n");

    pid_t pid = fork();  //第一次fork
    if( pid != 0 )
        exit(0);//parent

    //first children
    if(setsid() == -1)
    {
       printf("setsid failed\n");
       assert(0);
       exit(-1);
    }

    umask(0);

    pid = fork();  //第二次fork

    if( pid != 0)
        exit(0);

    //second children
    chdir ("/");

    for (int i = 0; i < 3; i++)
    {
        close (i);
    }

    int stdfd = open ("/dev/null", O_RDWR);
    dup2(stdfd, STDOUT_FILENO);
    dup2(stdfd, STDERR_FILENO);

    return 0;
}


/**
 * @brief		以下参数需要外部输入提供,多数参数是用于init_segment中
 */
#define NEED_INPUT_PARAMATER 0x1
#if NEED_INPUT_PARAMATER
#define IP_PORT 23333
#define IP_ADDR ""
#define IP_PROTOCOL "tcp"
#define IP_CLIENT_MAX_SIZE 0x1
#define NET_DATA_TYPE 0x1
#endif

/*!
 * @brief		work function
 */
int execute_work()
{
	int temp_rt = 0x0;


	gentest_frm();


	//initialize
	network_init(IP_ADDR,IP_PORT,IP_PROTOCOL,IP_CLIENT_MAX_SIZE,&temp_rt);
	if(temp_rt){
		ELog("network initialie failed");
		return RT_ERROR;
	}else{
		DLog("network initialize success.[%s]%s:%d",IP_PROTOCOL,IP_ADDR,IP_PORT);
	}

	bool exit_loop = false;
	char* temp_read_buffer = NULL;
	int temp_read_byte = 0x0;
	//
	while(1)
	{
		if(exit_loop)
			break;

		//read tcp/ip data
		network_read((void**)&temp_read_buffer,&temp_read_byte);
		if(0x0 >= temp_read_byte)
			continue;
	}

	//delete
	network_delete();

	return temp_rt;
}


static void child_proc_exit(int signal, siginfo_t *siginfo, void *context)
{
	//signal action
	DLog("signo %d",signal);
	DLog("pid %d exit",getpid());

	simu_exe_flag_idle();
}


int main(int argc,char** argv)
{
	int temp_error = 0x0;
	//切换进程为守护进程
	bool daemon_enable = false;
#if ENABLE_DAEMON
	daemon_enable = true;
#endif
	int temp_rt = switch_daemon(daemon_enable);
	if(0x0 != temp_rt){
		perror("create daemon process failed\n");
		fprintf(stderr,"create daemon process failed\n");
		return RT_ERROR;
	}

	//initialize log
	temp_rt = InitializeLog("MDKDemo-Service");
	if(temp_rt){
		fprintf(stderr,"Log Module initialize failed\n");
		return RT_ERROR;
	}else{
		//TODO:[调试]这你只用于调试,最终需要修改下面的逻辑
#if COMPILE_VERSION == 0x0
		LogEnableDebug(true);
#else
		//LogEnableDebug(true);
#endif
		LogForceFlush(true);
#if LOGLIB_NET_ENABLE
		LogSetNet(temp_ipaddr_remote,temp_ipport_remote);
		LogEnableNet(true);
#endif
	}

	//signal
	struct sigaction sigact;
	int sig_no = SIGCHLD;
	// Register a signal handler for the real-time signal
	sigact.sa_sigaction = child_proc_exit;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = SA_RESTART | SA_SIGINFO;
	int rc = sigaction(sig_no, &sigact, NULL);
	if (rc < 0) {
		printf("Failed to register DMA callback");
		temp_error = rc;
		goto TAG_ERROR;
	}

#if ENABLE_AD_CTRL
	//
	temp_rt = register_ad_byname(AD_ONE_ID,"ad9361-phy0");
	if(temp_rt){
		ELog("register_ad_byname function occur error,%d",temp_rt);
		temp_error = temp_rt;
		goto TAG_ERROR;
	}

	//
	temp_rt = register_ad_byname(AD_TWO_ID,"ad9361-phy1");
	if(temp_rt){
		ELog("register_ad_byname function occur error,%d",temp_rt);
		temp_error = temp_rt;
		goto TAG_ERROR;
	}
#endif

	//
	temp_rt = execute_work();
	if(temp_rt){
		ELog("execute work failed:%d",temp_rt);
	}

	//release log
	ReleaseLog();
    return 0;
TAG_ERROR:
	ReleaseLog();
    return temp_error;
}

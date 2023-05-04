/*!
 * @file Start.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-05-03
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "LogHeader.h"
#include <string.h>
#include <exception>
#include "process_op.h"


#define RT_ERROR ((int)__LINE__)
#define DATA_ERROR ((int)-1)

#define TEST_ELF "/home/root/gdbDemo/demoThread"

int main(int argc,char** argv)
{
	fprintf(stdout,"start test application\n");
    //
    int temp_rt = 0x0;
	//initialize log
	temp_rt = InitializeLog("demoProcess");
	if(temp_rt){
		fprintf(stderr,"Log Module initialize failed\n");
		return RT_ERROR;
	}else{
		LogEnableDebug(true);
		LogForceFlush(true);
#if LOGLIB_NET_ENABLE
		LogSetNet(temp_ipaddr_remote,temp_ipport_remote);
		LogEnableNet(true);
#endif
	}

	int temp_default = 0x1;
	int temp_value = temp_default;
	int loop_status = 0x0;
	int temp_cnt = 0x0;
	while(1)
	{
		if(0x0 == temp_value)
			break;

		if(11 == temp_value)
		{
			temp_value = temp_default;
			//start
			loop_status = start_simu_exe(TEST_ELF);
			if(loop_status){
				ELog("start process failed");
				break;
			}else{
				DLog("start process success");
			}
		}
		if(22 == temp_value)
		{
			temp_value = temp_default;
			//stop
			loop_status = stop_simu_exe(TEST_ELF);
			if(loop_status){
				ELog("start process failed");
				break;
			}else{
				DLog("start process success");
			}
		}
		if(33 == temp_value)
		{
			//restart
			loop_status = restart_simu_exe(TEST_ELF);
			if(loop_status){
				ELog("start process failed");
				break;
			}else{
				DLog("start process success");
			}
		}
		//
		DTLog(1000,"now was running:%d",++temp_cnt);
	}

	DLog("already try to stop application");
	ReleaseLog();
	return EXIT_SUCCESS;
}

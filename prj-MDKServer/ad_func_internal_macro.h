/*
 * internal_macro.h
 *
 *  Created on: 2021��10��30��
 *      Author: 14452
 */

#ifndef INCLUDE_INTERNAL_MACRO_H_
#define INCLUDE_INTERNAL_MACRO_H_
#ifdef __cplusplus
extern "C"{
#endif

#if 0

#define AD_DRIVER_KO_LOAD 0x0
#if AD_DRIVER_KO_LOAD
//	AD�Ĵ���ֱ�ӷ����ļ�
#define AD_DIRECT_REG_ACCESS_ONE "/sys/kernel/debug/iio/iio:device1/direct_reg_access"
#define AD_DIRECT_REG_ACCESS_TWO "/sys/kernel/debug/iio/iio:device2/direct_reg_access"

//	ADоƬiio����Ŀ¼
#define AD_IIO_DIR_ONE "/sys/bus/iio/devices/iio:device1/"
#define AD_IIO_DIR_TWO "/sys/bus/iio/devices/iio:device2/"
#else
//	AD�Ĵ���ֱ�ӷ����ļ�
#define AD_DIRECT_REG_ACCESS_ONE "/sys/kernel/debug/iio/iio:device0/direct_reg_access"
#define AD_DIRECT_REG_ACCESS_TWO "/sys/kernel/debug/iio/iio:device1/direct_reg_access"

//	AD��λֱ�ӷ����ļ�
#define AD_RESET_REG_ACCESS_ONE "/sys/kernel/debug/iio/iio:device0/initialize"
#define AD_RESET_REG_ACCESS_TWO "/sys/kernel/debug/iio/iio:device1/initialize"

//	ADоƬiio����Ŀ¼
#define AD_IIO_DIR_ONE "/sys/bus/iio/devices/iio:device0/"
#define AD_IIO_DIR_TWO "/sys/bus/iio/devices/iio:device1/"
#endif


//	Ƶ���Լ�������豸״̬��ȡ��ʽ���������Ϊ1����ô�ͱ�ʾ�豸״̬�Ļ�ȡ��ͨ������ʱ�����״ֵ̬��ȷ���ģ�����ȥ��ȡ�����Ӳ��״̬
#define USING_SAVE_STATUS_INFO 0x1

#endif


#define AD_DEBUGFS_ROOT_DIR "/sys/kernel/debug/iio"
#define AD_HOME_ROOT_DIR "/sys/bus/iio/devices"


#define FILENAME_REST "initialize"
#define FILENAME_REG_ACCESS "direct_reg_access"

//	Ƶ��
#define	TX_FREQUENCY "out_altvoltage1_TX_LO_frequency"
#define RX_FREQUENCY "out_altvoltage0_RX_LO_frequency"
//	����
#define TX_BANDWIDTH "out_voltage_rf_bandwidth"
#define RX_BANDWIDTH "in_voltage_rf_bandwidth"
//	RSSI
#define TX_RSSI "out_voltage0_rssi"
#define RX_RSSI "in_voltage0_rssi"

//	get_xxx�����������֮��ĵȴ�ʱ��
#define GET_INFO_WAIT_MS(ms) usleep(ms * 1000)



#ifdef __cplusplus
}
#endif
#endif /* INCLUDE_INTERNAL_MACRO_H_ */

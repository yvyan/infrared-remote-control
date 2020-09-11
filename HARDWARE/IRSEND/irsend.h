#ifndef __IRSEND_H__
#define __IRSEND_H__
#include "sys.h"
#include "delay.h"
#include "pwm.h" //�����ز�

//����˿ڶ���
//DAT -> PA0
#define IR_SEND   PCout(2)	// ���ⷢ�Ϳ���

/**
* ���ⷢ��ģ���ʼ������
*/
void IR_SendPort_Init(void);

/**
* ���ͱ�׼��32λNEC����
*
* @param data 32λҪ���͵�����
*/
void TR_SendData(u32 data);

/**
* ͳһ���ݷ���ӿڣ���־��ģ�����ݾ��ɷ��ͣ�
*
* @param pBuffer Ҫ���͵���������ָ�루���ݸ�ʽ��remote_save.c��remote_save.h��
* @param sta     ���ݵĻ�����Ϣ�����ݸ�ʽ��remote_save.c��remote_save.h��
*/
void TR_SendTimeData(u16 * pBuffer, u16 sta);


#endif

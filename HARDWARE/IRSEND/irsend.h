#ifndef __IRSEND_H__
#define __IRSEND_H__
#include "sys.h"
#include "delay.h"
#include "pwm.h" //依赖载波

//红外端口定义
//DAT -> PA0
#define IR_SEND   PCout(2)	// 红外发送控制

/**
* 红外发射模块初始化函数
*/
void IR_SendPort_Init(void);

/**
* 发送标准的32位NEC数据
*
* @param data 32位要发送的数据
*/
void TR_SendData(u32 data);

/**
* 统一数据发射接口（标志，模拟数据均可发送）
*
* @param pBuffer 要发送的数据数组指针（数据格式见remote_save.c和remote_save.h）
* @param sta     数据的基本信息（数据格式见remote_save.c和remote_save.h）
*/
void TR_SendTimeData(u16 * pBuffer, u16 sta);


#endif

#ifndef __IRSEND_H__
#define __IRSEND_H__
#include "sys.h"
#include "delay.h"
#include "pwm.h" //依赖载波

//红外端口定义
//DAT -> PA0
#define IR_SEND   PCout(2)	// 红外发送控制

void IR_SendPort_Init(void);

void TR_SendData(u32 data);

void TR_SendTimeData(u16 * pBuffer, u16 sta);


#endif

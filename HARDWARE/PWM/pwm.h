/*******************************************************************
	使用定时器TIM2的通道CH1输出38KHz的波形
*******************************************************************/

#ifndef __PWM_H__
#define __PWM_H__
#include "sys.h"

/**
* 定时器2PWM波形输出初始化
*/
void TIM2_PWM_Init(u16 arr,u16 psc);
#endif

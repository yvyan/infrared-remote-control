#include "irsend.h"


/**
* 红外发射模块初始化函数
*/
void IR_SendPort_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_2); //默认输出为高
}

/**
* 发送标准的32位NEC数据
*
* @param data 32位要发送的数据
*/
void TR_SendData(u32 data)
{
	int t;
	//引导码
	IR_SEND = 0;
	delay_us(9000 - 2);
	IR_SEND = 1;
	delay_us(4500 - 2);
	//数据
	for (t = 31; t >= 0; t--)
	{
		IR_SEND = 0;
		delay_us(560 - 2);
		IR_SEND = 1;
		if ((data >> t) % 2)
		{
			delay_us(1680 - 2);
		}
		else
		{
			delay_us(560 - 2);
		}
	}
	//关闭输出
	IR_SEND = 0;
	delay_us(560 - 2);
	IR_SEND = 1;
}

/**
* 统一数据发射接口（标志，模拟数据均可发送）
*
* @param pBuffer 要发送的数据数组指针（数据格式见remote_save.c和remote_save.h）
* @param sta     数据的基本信息（数据格式见remote_save.c和remote_save.h）
*/
void TR_SendTimeData(u16 * pBuffer, u16 sta)
{
	int t;
	int i;
	int lenth;
	if (((sta >> 12) & 0x7) == 1) //模拟模式
	{
		lenth = sta & 0x03FF;
		for (t = 0; t < lenth; t++)
		{
			if (t % 2 == 0) //高
			{
				IR_SEND = 1;
				i = pBuffer[t] - 2;
				delay_us(i);
			}
			else //低
			{
				IR_SEND = 0;
				i = pBuffer[t] - 2;
				delay_us(i);
			}
		}
		//delay_ms(100);
		IR_SEND = 1;
	}
	else if (((sta >> 12) & 0x7) == 0) //标准模式
	{
		u32 data = (pBuffer[1] << 16) + pBuffer[0];
		TR_SendData(data);
	}
}


#include "irsend.h"


/**
* ���ⷢ��ģ���ʼ������
*/
void IR_SendPort_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin	= GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_2); //Ĭ�����Ϊ��
}

/**
* ���ͱ�׼��32λNEC����
*
* @param data 32λҪ���͵�����
*/
void TR_SendData(u32 data)
{
	int t;
	//������
	IR_SEND = 0;
	delay_us(9000 - 2);
	IR_SEND = 1;
	delay_us(4500 - 2);
	//����
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
	//�ر����
	IR_SEND = 0;
	delay_us(560 - 2);
	IR_SEND = 1;
}

/**
* ͳһ���ݷ���ӿڣ���־��ģ�����ݾ��ɷ��ͣ�
*
* @param pBuffer Ҫ���͵���������ָ�루���ݸ�ʽ��remote_save.c��remote_save.h��
* @param sta     ���ݵĻ�����Ϣ�����ݸ�ʽ��remote_save.c��remote_save.h��
*/
void TR_SendTimeData(u16 * pBuffer, u16 sta)
{
	int t;
	int i;
	int lenth;
	if (((sta >> 12) & 0x7) == 1) //ģ��ģʽ
	{
		lenth = sta & 0x03FF;
		for (t = 0; t < lenth; t++)
		{
			if (t % 2 == 0) //��
			{
				IR_SEND = 1;
				i = pBuffer[t] - 2;
				delay_us(i);
			}
			else //��
			{
				IR_SEND = 0;
				i = pBuffer[t] - 2;
				delay_us(i);
			}
		}
		//delay_ms(100);
		IR_SEND = 1;
	}
	else if (((sta >> 12) & 0x7) == 0) //��׼ģʽ
	{
		u32 data = (pBuffer[1] << 16) + pBuffer[0];
		TR_SendData(data);
	}
}


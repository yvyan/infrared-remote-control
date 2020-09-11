/*******************************************************************
	ʹ�ö�ʱ��TIM2��ͨ��CH1���38KHz�Ĳ���
*******************************************************************/
#include "pwm.h"
#include "delay.h"

/********************************************************************
* Function Name  : TIM2_PWM_Init(u16 arr,u16 psc)
* Function       : TIM2��ͨ��CH1��PWMģʽ��ʼ��
* parameter      : arr - �Զ���װֵ
				   psc - ʱ��Ԥ��Ƶ��	
* Description    : Ƶ��f = 72M/[(psc+1)*(arr+1)]               
* Return         : void
*********************************************************************/
//TIM2_PWM_Init(1895,0); 72000/(1895+1) = 37.99K

/**
* ��ʱ��2PWM���������ʼ��
*/
void TIM2_PWM_Init(u16 arr,u16 psc)
{
	/* ��ʼ���ṹ�嶨�� */
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef 	TIM_OCInitStructure;
	/* ʹ����Ӧ�˿ڵ�ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��2ʱ��
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //ʹ��GPIO����ʱ��
	
	/* GPIOA.0��ʼ�� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;			// TIM2 CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// PA.0 �����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	/* TIM2 ��ʼ��*/
	TIM_TimeBaseInitStructure.TIM_Period = arr;	 //��һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	//��ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;  //ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	/* ��ʱ��TIM2 Ch1 PWMģʽ��ʼ�� */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  //ѡ��ʱ��ģʽ:TIM PWM1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//�Ƚ����ʹ��
	//TIM_OCInitStructure.TIM_Pulse = (arr+1)/2;	  //ռ�ձ� 50%
	TIM_OCInitStructure.TIM_Pulse = (arr+1)/3;	  //ռ�ձ�1:3
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	//�������:TIM����Ƚϼ��Ը�
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	/* ʹ��TIM2��CCR1�ϵ�Ԥװ�ؼĴ��� */
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	/* ʹ�ܶ�ʱ�� */
	TIM_Cmd(TIM2, ENABLE); 			
}

/*******************************************************************
	使用定时器TIM2的通道CH1输出38KHz的波形
*******************************************************************/
#include "pwm.h"
#include "delay.h"

/********************************************************************
* Function Name  : TIM2_PWM_Init(u16 arr,u16 psc)
* Function       : TIM2的通道CH1的PWM模式初始化
* parameter      : arr - 自动重装值
				   psc - 时钟预分频数	
* Description    : 频率f = 72M/[(psc+1)*(arr+1)]               
* Return         : void
*********************************************************************/
//TIM2_PWM_Init(1895,0); 72000/(1895+1) = 37.99K

/**
* 定时器2PWM波形输出初始化
*/
void TIM2_PWM_Init(u16 arr,u16 psc)
{
	/* 初始化结构体定义 */
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseInitStructure;
	TIM_OCInitTypeDef 	TIM_OCInitStructure;
	/* 使能相应端口的时钟 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器2时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);   //使能GPIO外设时钟
	
	/* GPIOA.0初始化 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;			// TIM2 CH1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 	// PA.0 复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_0);
	/* TIM2 初始化*/
	TIM_TimeBaseInitStructure.TIM_Period = arr;	 //下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;	//作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseInitStructure.TIM_ClockDivision = 0;  //时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	/* 定时器TIM2 Ch1 PWM模式初始化 */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  //选择定时器模式:TIM PWM1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	//比较输出使能
	//TIM_OCInitStructure.TIM_Pulse = (arr+1)/2;	  //占空比 50%
	TIM_OCInitStructure.TIM_Pulse = (arr+1)/3;	  //占空比1:3
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	//输出极性:TIM输出比较极性高
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);

	/* 使能TIM2在CCR1上的预装载寄存器 */
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	/* 使能定时器 */
	TIM_Cmd(TIM2, ENABLE); 			
}

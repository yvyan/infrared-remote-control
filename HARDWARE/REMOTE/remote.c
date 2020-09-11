#include "remote.h"
#include "delay.h"


/**
* 红外传感器接收头引脚初始化
*/
//设置IO以及定时器4的输入捕获
void Remote_Init(void)    			  
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //使能PORTB时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	 //TIM4 时钟使能 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				     //PB9 输入 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		   //上拉输入 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	                     //初始化GPIOB.9
	
						  
 	TIM_TimeBaseStructure.TIM_Period = 10000;                   //设定计数器自动重装值 最大10ms溢出  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	              //预分频器,1M的计数频率,1us加1.	   	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);             //根据指定的参数初始化TIMx

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;            // 选择输入端 IC4映射到TI4上
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//下降沿捕获
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	      //配置输入分频,不分频 
  TIM_ICInitStructure.TIM_ICFilter = 0x03;                    //IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
  TIM_ICInit(TIM4, &TIM_ICInitStructure);                     //初始化定时器输入捕获通道

  TIM_Cmd(TIM4,ENABLE ); //使能定时器4
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;             //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;          //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);                             //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器	

	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);        //允许更新中断 ,允许CC4IE捕获中断								 
}

/**
* 打开红外接收
*/
void Remote_ON(void)
{
	TIM_Cmd(TIM4, ENABLE);
	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);
}

/**
* 关闭红外接收
*/
void Remote_OFF(void)
{
	TIM_Cmd(TIM4,DISABLE );
	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,DISABLE);
}

u8 start = 0; //标志记录模式是否开启
u16 a[350];   //用于记录模式下记录波形时间数据
u16 i = 0;    //数组index
u8 type = 0;  //记录波形初始电平（仅用于验证，因为选择下降沿捕获，记录的第一个时间为低电平持续时间）

//遥控器接收状态：
//
//[7]:收到了引导码标志
//[6]:得到了一个按键的所有信息
//[5]:保留	
//[4]:标记上升沿是否已经被捕获
//[3:0]:溢出计时器
u8 	RmtSta=0;	  	  
u16 Dval;		  //下降沿时计数器的值
u32 RmtRec=0;	//红外接收到的数据	   		    
u8  RmtCnt=0;	//按键按下的次数	  

//定时器4中断服务程序	 
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)								//上次有数据被接收到了
		{	
			RmtSta&=~0X10;							//取消上升沿已经被捕获标记
			if((RmtSta&0X0F)==0X00)
				RmtSta|=1<<6;	            //标记已经完成一次按键的键值信息采集
			if((RmtSta&0X0F)<14)
				RmtSta++;
			else
			{
				RmtSta&=~(1<<7);					//清空引导标识
				RmtSta&=0XF0;						  //清空计数器
			}								 	   	
		}							    
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
	{	  
		if(RDATA)//上升沿捕获
		{
			int temp = TIM_GetCapture4(TIM4);
			if (start) // 记录持续时间数据
			{
				if (type == 0)
					type = 1;
				a[i++] = temp;
			}
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling); //CC4P=1	设置为下降沿捕获
			TIM_SetCounter(TIM4,0);							                //清空定时器值
			RmtSta|=0X10;						                          	//标记上升沿已经被捕获
		}
		else //下降沿捕获
		{
			Dval=TIM_GetCapture4(TIM4);					               //读取CCR4也可以清CC4IF标志位
			if (start) // 记录持续时间数据
			{
				if (type == 0)
					type = 2;
				a[i++] = Dval;
			}
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC4P=0	设置为上升沿捕获
			if(RmtSta&0X10)							                       //完成一次高电平捕获 
			{
 				if(RmtSta&0X80) //接收到了引导码
				{
					
					if(Dval>450&&Dval<700) //560为标准值,560us
					{
						RmtRec<<=1;					 //左移一位.
						RmtRec|=0;					 //接收到0	   
					}else if(Dval>1400&&Dval<1800) //1680为标准值,1680us
					{
						RmtRec<<=1;					//左移一位.
						RmtRec|=1;					//接收到1
					}else if(Dval>2200&&Dval<2600) //得到按键键值增加的信息 2500为标准值2.5ms
					{
						RmtCnt++; 					//按键次数增加1次
						RmtSta&=0XF0;				//清空计时器		
					}
 				}
				else if(Dval>4200&&Dval<4700) //4500为标准值4.5ms
				{
					RmtSta|=1<<7;					//标记成功接收到了引导码
					RmtCnt=0;						  //清除按键次数计数器
				}
			}
			RmtSta&=~(1<<4);
			TIM_SetCounter(TIM4,0);	
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);	 	    
}

/**
* 遥控扫描
*
* @return 0,没有任何按键按下；其他：按下的按键键值
*/
u8 Remote_Scan(void)
{
	u8 sta=0;       
  u8 t1,t2;
	if(RmtSta&(1<<6))                  //得到一个按键的所有信息了
	{
		t1=RmtRec>>24;			             //得到地址码
	  t2=(RmtRec>>16)&0xff;	           //得到地址反码 
 	  if((t1==(u8)~t2)&&t1==REMOTE_ID) //检验遥控识别码(ID)及地址 
	  {
			t1=RmtRec>>8;
	    t2=RmtRec;
	    if(t1==(u8)~t2)
				sta=t1;                      //键值正确	 
		}   
		if((sta==0)||((RmtSta&0X80)==0)) //按键数据错误/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);               //清除接收到有效按键标识
			RmtCnt=0;		                   //清除按键次数计数器
		}
	}
  return sta;
}

/**
* 遥控扫描（不校验地址）
*
* @return 0,没有任何按键按下；其他：按下的按键键值
*/
u16 Remote_Scan_Pro(void)
{
	u16 sta=0;       
  u8 t1,t2;
	if(RmtSta&(1<<6))                  //得到一个按键的所有信息了
	{
		t1=RmtRec>>24;			             //得到地址码
	  t2=(RmtRec>>16)&0xff;	           //得到地址反码 
 	  if(/*t1==(u8)~t2*/1) //检验遥控识别码(ID)及地址 **不校验地址
	  {
			sta = t2 << 8;
			t1=RmtRec>>8;
	    t2=RmtRec;
	    if(t1==(u8)~t2)
				sta += t1;                      //键值正确
			else
				sta = 0;
		}   
		if((sta==0)||((RmtSta&0X80)==0)) //按键数据错误/遥控已经没有按下了
		{
		 	RmtSta&=~(1<<6);               //清除接收到有效按键标识
			RmtCnt=0;		                   //清除按键次数计数器
		}
	}
  return sta;
}

/**
* 遥控扫描（不校验地址）
*
* @return 接受到的32位数据
*/
u32 Remote_Scan_All(void)
{
	u8 sta=0;
	u8 t1,t2;
	if (RmtSta&(1<<6))
	{
		t1=RmtRec>>8;
	  t2=RmtRec;
	  if(t1==(u8)~t2)
			sta = t1;
		else
			sta = 0;
		if((sta==0)||((RmtSta&0X80)==0)) //按键数据错误/遥控已经没有按下了
		{
			RmtSta&=~(1<<6);               //清除接收到有效按键标识
			RmtCnt=0;		                   //清除按键次数计数器
		}
	}
	
	if (sta)
		return RmtRec;
	else
		return 0;
}

/**
* 反馈数字（直到按键按下）
*
* @return 按下的按键值
*/
u8 Remote_Num(void)
{
	RmtSta = 0; //清空之前数据记录
	while(1)
	{
		u8 key = 0;
		RmtCnt = 1;
		while(RmtCnt != 0)
			key = Remote_Scan();
		if(key)
		{
			switch(key)
			{
				case 104: return 0;
				case 48 : return 1;
				case 24 : return 2;
				case 122: return 3;
				case 16 : return 4;
				case 56 : return 5;
				case 90 : return 6;
				case 66 : return 7;
				case 74 : return 8;
				case 82 : return 9;
				case 144: return ADD;
				case 168: return LESS;
				case 98 : return MODE;
				case 224: return EQ;
				case 176: return USD;
				case 152: return RPT;
				case 2  : return LEFT;
				case 194: return RIGHT;
				case 34 : return PLAY;
				case 162: return POWER;
				case 226: return VOLX;
			}
		}
	}
}


/**
* 获得遥控器数据值
* return 数据值
*/
u16 Remote_Get(void)
{
	RmtSta = 0; //清空之前数据记录
	while(1)
	{
		u16 key = 0;
		RmtCnt = 1;
		while(RmtCnt != 0)
			key = Remote_Scan_Pro();
		if (key)
			return key;
	}
}

/**
* 开启记录模式
*/
void Re_Record_ON(void)
{
	start = 1;
	i = 0;
}

/**
* 关闭记录模式
* @return 记录数组的长度
*/
u16 Re_Record_OFF(void)
{
	start = 0;
	return i;
}

/**
* 获取记录数组数据
* @return 数据内容
*/
u16* Re_Record_Get()
{
	return a;
}

/**
* 确定首个数据是高低电平
*/
u8 Re_Get_Type(void)
{
	return type;
}

u32 Remote_GetData(void)
{
	RmtSta = 0; //清空之前数据记录
	while(1)
	{
		u32 key = 0;
		RmtCnt = 1;
		while(RmtCnt != 0)
			key = Remote_Scan_All();
		if (key)
			return key;
	}
}

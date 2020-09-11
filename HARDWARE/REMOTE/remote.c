#include "remote.h"
#include "delay.h"
#include "usart.h"


//����ң�س�ʼ��
//����IO�Լ���ʱ��4�����벶��
void Remote_Init(void)    			  
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;  
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //ʹ��PORTBʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	 //TIM4 ʱ��ʹ�� 

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				     //PB9 ���� 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		   //�������� 
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	                     //��ʼ��GPIOB.9
	
						  
 	TIM_TimeBaseStructure.TIM_Period = 10000;                   //�趨�������Զ���װֵ ���10ms���  
	TIM_TimeBaseStructure.TIM_Prescaler =(72-1); 	              //Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   	
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);             //����ָ���Ĳ�����ʼ��TIMx

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;            // ѡ������� IC4ӳ�䵽TI4��
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Falling;	//�½��ز���
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	      //���������Ƶ,����Ƶ 
  TIM_ICInitStructure.TIM_ICFilter = 0x03;                    //IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
  TIM_ICInit(TIM4, &TIM_ICInitStructure);                     //��ʼ����ʱ�����벶��ͨ��

  TIM_Cmd(TIM4,ENABLE ); //ʹ�ܶ�ʱ��4
 
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;             //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;   //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;          //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);                             //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���	

	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);        //��������ж� ,����CC4IE�����ж�								 
}

/**
* �򿪺������
*/
void Remote_ON(void)
{
	TIM_Cmd(TIM4, ENABLE);
	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);
}

/**
* �رպ������
*/
void Remote_OFF(void)
{
	TIM_Cmd(TIM4,DISABLE );
	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,DISABLE);
}

u8 start = 0;
u16 a[350];
u16 i = 0;
u8 type = 0;

//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval;		  //�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	  
//��ʱ��4�жϷ������	 
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
	{
		if(RmtSta&0x80)								//�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;							//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)
				RmtSta|=1<<6;	            //����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			if((RmtSta&0X0F)<14)
				RmtSta++;
			else
			{
				RmtSta&=~(1<<7);					//���������ʶ
				RmtSta&=0XF0;						  //��ռ�����
			}								 	   	
		}							    
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4)!=RESET)
	{	  
		if(RDATA)//�����ز���
		{
			int temp = TIM_GetCapture4(TIM4);
			if (start)
			{
				if (type == 0)
					type = 1;
				a[i++] = temp;
			}
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Falling); //CC4P=1	����Ϊ�½��ز���
			TIM_SetCounter(TIM4,0);							                //��ն�ʱ��ֵ
			RmtSta|=0X10;						                          	//����������Ѿ�������
		}
		else //�½��ز���
		{
			Dval=TIM_GetCapture4(TIM4);					               //��ȡCCR4Ҳ������CC4IF��־λ
			if (start)
			{
				if (type == 0)
					type = 2;
				a[i++] = Dval;
			}
  		TIM_OC4PolarityConfig(TIM4,TIM_ICPolarity_Rising); //CC4P=0	����Ϊ�����ز���
			if(RmtSta&0X10)							                       //���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&0X80) //���յ���������
				{
					
					if(Dval>450&&Dval<700) //560Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;					 //����һλ.
						RmtRec|=0;					 //���յ�0	   
					}else if(Dval>1400&&Dval<1800) //1680Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;					//����һλ.
						RmtRec|=1;					//���յ�1
					}else if(Dval>2200&&Dval<2600) //�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						RmtCnt++; 					//������������1��
						RmtSta&=0XF0;				//��ռ�ʱ��		
					}
 				}
				else if(Dval>4200&&Dval<4700) //4500Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;					//��ǳɹ����յ���������
					RmtCnt=0;						  //�����������������
				}
			}
			RmtSta&=~(1<<4);
			TIM_SetCounter(TIM4,0);	
		}
	}
	TIM_ClearITPendingBit(TIM4,TIM_IT_Update|TIM_IT_CC4);	 	    
}

//����������
//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
u8 Remote_Scan(void)
{
	u8 sta=0;       
  u8 t1,t2;
	if(RmtSta&(1<<6))                  //�õ�һ��������������Ϣ��
	{
		t1=RmtRec>>24;			             //�õ���ַ��
	  t2=(RmtRec>>16)&0xff;	           //�õ���ַ���� 
 	  if((t1==(u8)~t2)&&t1==REMOTE_ID) //����ң��ʶ����(ID)����ַ 
	  {
			t1=RmtRec>>8;
	    t2=RmtRec;
	    if(t1==(u8)~t2)
				sta=t1;                      //��ֵ��ȷ	 
		}   
		if((sta==0)||((RmtSta&0X80)==0)) //�������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);               //������յ���Ч������ʶ
			RmtCnt=0;		                   //�����������������
		}
	}
  return sta;
}

//������������ǿ
//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
u16 Remote_Scan_Pro(void)
{
	u16 sta=0;       
  u8 t1,t2;
	if(RmtSta&(1<<6))                  //�õ�һ��������������Ϣ��
	{
		t1=RmtRec>>24;			             //�õ���ַ��
	  t2=(RmtRec>>16)&0xff;	           //�õ���ַ���� 
 	  if(/*t1==(u8)~t2*/1) //����ң��ʶ����(ID)����ַ **��У���ַ
	  {
			sta = t2 << 8;
			t1=RmtRec>>8;
	    t2=RmtRec;
	    if(t1==(u8)~t2)
				sta += t1;                      //��ֵ��ȷ
			else
				sta = 0;
		}   
		if((sta==0)||((RmtSta&0X80)==0)) //�������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);               //������յ���Ч������ʶ
			RmtCnt=0;		                   //�����������������
		}
	}
  return sta;
}

//����������(��ȡȫ������) **������**
//����ֵ:
//	 0,û���κΰ�������
//����,����.
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
		if((sta==0)||((RmtSta&0X80)==0)) //�������ݴ���/ң���Ѿ�û�а�����
		{
			RmtSta&=~(1<<6);               //������յ���Ч������ʶ
			RmtCnt=0;		                   //�����������������
		}
	}
	
	if (sta)
		return RmtRec;
	else
		return 0;
}

/******
0:   ERROR
162: POWER   20
98:  MODE  18
2:   LEFT  17
226: VOLX	 16
194: RIGHT 15
34:  PLAY	 14
224: EQ	     19
168: VOL-	 13
144: VOL+	 12
104: 0	  
152: RPT   11
176: USD   10
48:  1	    
24:  2	    
122: 3	  
16:  4		   					
56:  5 
90:  6
66:  7
74:  8
82:  9
******/

u8 Remote_Num(void)
{
	RmtSta = 0; //���֮ǰ���ݼ�¼
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
* ���ң��������ֵ
* return ����ֵ
*/
u16 Remote_Get(void)
{
	RmtSta = 0; //���֮ǰ���ݼ�¼
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
* ������¼ģʽ
*/
void Re_Record_ON(void)
{
	start = 1;
	i = 0;
}

/**
* �رռ�¼ģʽ
* @return ��¼����ĳ���
*/
u16 Re_Record_OFF(void)
{
	start = 0;
	return i;
}

/**
* ��ȡ��¼��������
* @return ��������
*/
u16* Re_Record_Get()
{
	return a;
}

/**
* ȷ���׸������Ǹߵ͵�ƽ
*/
u8 Re_Get_Type(void)
{
	return type;
}

u8 Remote_Test1(void)
{
	return RmtSta;
}

u32 Remote_Test2(void)
{
	RmtSta = 0; //���֮ǰ���ݼ�¼
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

u32 Remote_Test3(void)
{
	return RmtRec;
}

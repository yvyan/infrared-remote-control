#include "oled.h"
#include "delay.h"
#include "oledfont.h"
#include "string.h"


//IIC��ʼ��
void OLED_IIC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PB.6,PB.7 �˿�����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	     //�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	     //IO���ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_6 | GPIO_Pin_7);					 //PB.6,PB.7 �����
}

//��ʼ�ź�
void OLED_IIC_Start(void)
{
	OLED_SDA_OUT();   //SDA����Ϊ���
	OLED_IIC_SDA = 1; //��ʼ��Ϊ�ߵ�ƽ
	OLED_IIC_SCL = 1;
	delay_us(4);
	
	OLED_IIC_SDA = 0; //��SCLΪ���ڼ䣬SDA�ɸߵ�������
	delay_us(4);
	
	OLED_IIC_SCL = 0; //����ʱ���ߣ�׼�����ͻ��������
}

//ֹͣ�ź�
void OLED_IIC_Stop(void)
{
	OLED_SDA_OUT();   //SDA����Ϊ���
	OLED_IIC_SCL = 0;
	OLED_IIC_SDA = 0;
	delay_us(2);
	
	OLED_IIC_SCL = 1; //��SCLΪ���ڼ䣬SDA�ɵ͵�������
	delay_us(2);
	OLED_IIC_SDA = 1;
	delay_us(4);
}

//�ȴ�Ӧ��
//����ֵ��1.ʧ�� 0.�ɹ�
u8 OLED_IIC_Wait_Ack(void)
{
	u8 ucErrTime = 0;
	OLED_SDA_IN(); //SDA����Ϊ����  
	OLED_IIC_SDA = 1;
	delay_us(1);	   
	OLED_IIC_SCL = 1;
	delay_us(1);	 
	while(OLED_READ_SDA) //��SDAΪ�ߣ���ѭ��
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			OLED_IIC_Stop();
			return 1;
		}
	}
	OLED_IIC_SCL = 0; //ʱ�����ͣ�Ӧ��ɹ�����
	return 0;  
}

//����һ���ֽڣ���Ӧ��
void OLED_IIC_Send_Byte(u8 txd)
{
	u8 t;
	OLED_SDA_OUT(); //SDA����Ϊ���
	OLED_IIC_SCL = 0;
	for(t = 0; t < 8; t++)
	{
		OLED_IIC_SDA = (txd & 0x80) >> 7; //0x80->10000000B
		txd <<= 1;
		delay_us(2);
		OLED_IIC_SCL = 1;
		delay_us(2);
		OLED_IIC_SCL = 0;
		delay_us(2);
	}
	OLED_IIC_Wait_Ack(); //�ȴ�Ӧ��
}

//����һ���ֽڣ���Ӧ��
void OLED_IIC_NSend_Byte(u8 txd)
{
	u8 t;
	OLED_SDA_OUT(); ////SDA����Ϊ���
	OLED_IIC_SCL = 0;
	for(t = 0; t < 8; t++)
	{
		OLED_IIC_SDA = (txd & 0x80) >> 7; //0x80->10000000B
		txd <<= 1;
		delay_us(2);
		OLED_IIC_SCL = 1;
		delay_us(2);
		OLED_IIC_SCL = 0;
		delay_us(2);
	}
}

// IIC Write Command
void Write_IIC_Command(unsigned char IIC_Command)
{
	OLED_IIC_Start();
	OLED_IIC_Send_Byte(0x78);
	OLED_IIC_Send_Byte(0x00);
	OLED_IIC_Send_Byte(IIC_Command);
	OLED_IIC_Stop();
}

// IIC Write Data
void Write_IIC_Data(unsigned char IIC_Data)
{
	OLED_IIC_Start();
	OLED_IIC_Send_Byte(0x78);
	OLED_IIC_Send_Byte(0x40);
	OLED_IIC_Send_Byte(IIC_Data);
	OLED_IIC_Stop();
}

// fill_Picture��ȫ����ʾ��
void OLED_Fill_picture(unsigned char fill_Data)
{
	unsigned char m,n;
	for(m = 0; m < 8; m++)
	{
		Write_IIC_Command(0xb0+m);	//page0-page8
		Write_IIC_Command(0x00);		//low column start address
		Write_IIC_Command(0x10);		//high column start address
		for(n = 0; n < 128; n++)
		{
			Write_IIC_Data(fill_Data);
		}
	}
}

//���һ������
void OLED_Clean(unsigned char i)
{
	unsigned char m,n;
	for(m = 2 * i; m < 2 * i + 2; m++)
	{
		Write_IIC_Command(0xb0+m);
		Write_IIC_Command(0x00);		//low column start address
		Write_IIC_Command(0x10);		//high column start address
		for(n = 0; n < 128; n++)
		{
			Write_IIC_Data(0x00);
		}
	}
}

// picture������ʾһ��ͼƬ
void OLED_Picture(unsigned char show[])
{
	unsigned char x,y;
	unsigned int i=0;
	for(y=0;y<8;y++)
	{
		Write_IIC_Command(0xb0+y);
		Write_IIC_Command(0x0);
		Write_IIC_Command(0x10);
		for(x = 0; x < 64; x++)
		{
			Write_IIC_Data(show[i++]);
		}
	}
}

// picture������ʾһ��ͼƬ(��ȫ��)
void OLED_Picture_Part(unsigned char show[], unsigned char start, unsigned char width)
{
	unsigned char x,y;
	unsigned int i=0;
	for(y=start;y<start + width;y++)
	{
		Write_IIC_Command(0xb0+y);
		Write_IIC_Command(0x0);
		Write_IIC_Command(0x10);
		for(x = 0; x < 128; x++)
		{
			Write_IIC_Data(show[i++]);
		}
	}
}

//��ʾ�����ַ���1608��
//x:0-15 y:0-3(x>15������ʾ)
void OLED_ShowChar(int x, int y, char chr) //'0' + 9
{
	unsigned char t;
	unsigned char add_y, l_x;
	
	x = x % 64;   //x>63����ж���ˢ��
	add_y = x/16; //x>16�������
	l_x = x%16;

	chr = chr - ' ';
	
	Write_IIC_Command(0xb0+2*y + 2*add_y); //��ʽΪ1608����Ҫyƫ�ƣ����û�2���������ַ�ռ16����
	Write_IIC_Command(0x0 + (l_x%2)*8);    //xƫ��8������λ+1
	Write_IIC_Command(0x10 + l_x/2);
	for(t = 0; t < 8; t++)
		Write_IIC_Data(asc2_1608[chr][t]);   //����Ϸ�8*8
	
	Write_IIC_Command(0xb0+2*y + 2*add_y + 1);
	Write_IIC_Command(0x0 + (l_x%2)*8);
	Write_IIC_Command(0x10 + l_x/2);
	for(; t < 16; t++)
		Write_IIC_Data(asc2_1608[chr][t]);   //����·�8*8
}

//����ַ���
void OLED_ShowStr(int x, int y, char show[])
{
	int t,i = strlen(show);
	for(t = 0; t < i; t++)
	{
			OLED_ShowChar(x+t, y, show[t]);
	}
}

unsigned int oled_pow(unsigned char m,unsigned char n)
{
	unsigned int result=1;	 
	while(n--)
		result*=m;    
	return result;
}

//��ʾ����
void OLED_ShowNum(int x, int y, long int num, unsigned char len)
{
	int t,temp;
	for(t = 0; t < len; t++)
	{
		temp = (num/oled_pow(10,len-t-1))%10;
		OLED_ShowChar(x+t, y, 48+temp);
	}
}

//��ʾ���ݵĶ�����ֵ(0-255)
void OLED_ShowBite(int x, int y, u8 num)
{
	unsigned char t;
	for (t = 0; t < 8; t++)
	{
		OLED_ShowChar(x + 8 - t, y, 48 + (num >> t) % 2);
	}
}

//���������ѡ��ʼ�ͽ���ҳ
void OLED_Scroll(char start, char stop)
{
	Write_IIC_Command(0x2E);	//�رչ���
	Write_IIC_Command(0x27);	//ˮƽ�������
	Write_IIC_Command(0x00);	//�����ֽ�
	Write_IIC_Command(0x00);	//��ʼҳ 0
	Write_IIC_Command(0x00);	//����ʱ����
	Write_IIC_Command(0x03);	//��ֹҳ 1
	Write_IIC_Command(0x00);	//�����ֽ�
	Write_IIC_Command(0xFF);	//�����ֽ�
	Write_IIC_Command(0x2F);	//��������
}

void OLED_NScroll()
{
	Write_IIC_Command(0x2E);	//�رչ���
}

//��ʼ��
void Initial_M096128x64_ssd1306()
{
	Write_IIC_Command(0xAE); //display off
	Write_IIC_Command(0x20); //Set Memory Addressing Mode	
	Write_IIC_Command(0x10); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	Write_IIC_Command(0xb0); //Set Page Start Address for Page Addressing Mode,0-7
	Write_IIC_Command(0xc8); //Set COM Output Scan Direction
	Write_IIC_Command(0x00); //---set low column address
	Write_IIC_Command(0x10); //---set high column address
	Write_IIC_Command(0x40); //--set start line address
	Write_IIC_Command(0x81); //--set contrast control register
	Write_IIC_Command(0xdf);
	Write_IIC_Command(0xa1); //--set segment re-map 0 to 127
	Write_IIC_Command(0xa6); //--set normal display
	Write_IIC_Command(0xa8); //--set multiplex ratio(1 to 64)
	Write_IIC_Command(0x3F);
	Write_IIC_Command(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	Write_IIC_Command(0xd3); //-set display offset
	Write_IIC_Command(0x00); //-not offset
	Write_IIC_Command(0xd5); //--set display clock divide ratio/oscillator frequency
	Write_IIC_Command(0xf0); //--set divide ratio
	Write_IIC_Command(0xd9); //--set pre-charge period
	Write_IIC_Command(0x22);
	Write_IIC_Command(0xda); //--set com pins hardware configuration
	Write_IIC_Command(0x12);
	Write_IIC_Command(0xdb); //--set vcomh
	Write_IIC_Command(0x20); //0x20,0.77xVcc
	Write_IIC_Command(0x8d); //--set DC-DC enable
	Write_IIC_Command(0x14); //
	Write_IIC_Command(0xaf); //--turn on oled panel */
}

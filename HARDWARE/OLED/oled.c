#include "oled.h"
#include "delay.h"
#include "oledfont.h"
#include "string.h"


//IIC初始化
void OLED_IIC_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PB.6,PB.7 端口配置
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 	     //推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	     //IO口速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_6 | GPIO_Pin_7);					 //PB.6,PB.7 输出高
}

//开始信号
void OLED_IIC_Start(void)
{
	OLED_SDA_OUT();   //SDA设置为输出
	OLED_IIC_SDA = 1; //初始均为高电平
	OLED_IIC_SCL = 1;
	delay_us(4);
	
	OLED_IIC_SDA = 0; //当SCL为高期间，SDA由高到低跳变
	delay_us(4);
	
	OLED_IIC_SCL = 0; //拉低时钟线，准备发送或接收数据
}

//停止信号
void OLED_IIC_Stop(void)
{
	OLED_SDA_OUT();   //SDA设置为输出
	OLED_IIC_SCL = 0;
	OLED_IIC_SDA = 0;
	delay_us(2);
	
	OLED_IIC_SCL = 1; //当SCL为高期间，SDA由低到高跳变
	delay_us(2);
	OLED_IIC_SDA = 1;
	delay_us(4);
}

//等待应答
//返回值：1.失败 0.成功
u8 OLED_IIC_Wait_Ack(void)
{
	u8 ucErrTime = 0;
	OLED_SDA_IN(); //SDA设置为输入  
	OLED_IIC_SDA = 1;
	delay_us(1);	   
	OLED_IIC_SCL = 1;
	delay_us(1);	 
	while(OLED_READ_SDA) //若SDA为高，则循环
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			OLED_IIC_Stop();
			return 1;
		}
	}
	OLED_IIC_SCL = 0; //时钟拉低，应答成功结束
	return 0;  
}

//发送一个字节（有应答）
void OLED_IIC_Send_Byte(u8 txd)
{
	u8 t;
	OLED_SDA_OUT(); //SDA设置为输出
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
	OLED_IIC_Wait_Ack(); //等待应答
}

//发送一个字节（无应答）
void OLED_IIC_NSend_Byte(u8 txd)
{
	u8 t;
	OLED_SDA_OUT(); ////SDA设置为输出
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

// fill_Picture（全屏显示）
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

//清空一行内容
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

// picture用来显示一个图片
void OLED_Picture(unsigned char show[])
{
	unsigned char x,y;
	unsigned int i=0;
	for(y=0;y<8;y++)
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

/**
* 显示一个图片(可以非全屏)
*
* @param x_start 横向开始坐标（单位：像素，0-127）
* @param length  横向长度（单位：像素）
* @param y_start 纵向开始坐标（单位：8个像素，0-7）
* @param width   纵向长度（单位：8个像素）
*/
void OLED_Picture_Part(unsigned char show[], unsigned char x_start, unsigned char length, unsigned char y_start, unsigned char width)
{
	unsigned char x,y;
	unsigned int i=0;
	for(y=y_start;y<y_start + width;y++)
	{
		Write_IIC_Command(0xb0+y);
		Write_IIC_Command(0x0);
		Write_IIC_Command(0x10);
		for(x = x_start; x < x_start + length; x++)
		{
			Write_IIC_Data(show[i++]);
		}
	}
}

//显示单个字符（1608）
//x:0-15 y:0-3(x>15换行显示)
void OLED_ShowChar(int x, int y, char chr) //'0' + 9
{
	unsigned char t;
	unsigned char add_y, l_x;
	
	x = x % 64;   //x>63则进行二次刷新
	add_y = x/16; //x>16换行输出
	l_x = x%16;

	chr = chr - ' ';
	
	Write_IIC_Command(0xb0+2*y + 2*add_y); //格式为1608，需要y偏移，则用户2被跳，单字符占16比特
	Write_IIC_Command(0x0 + (l_x%2)*8);    //x偏移8，第四位+1
	Write_IIC_Command(0x10 + l_x/2);
	for(t = 0; t < 8; t++)
		Write_IIC_Data(asc2_1608[chr][t]);   //输出上方8*8
	
	Write_IIC_Command(0xb0+2*y + 2*add_y + 1);
	Write_IIC_Command(0x0 + (l_x%2)*8);
	Write_IIC_Command(0x10 + l_x/2);
	for(; t < 16; t++)
		Write_IIC_Data(asc2_1608[chr][t]);   //输出下方8*8
}

//输出字符串
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

//显示数字
void OLED_ShowNum(int x, int y, long int num, unsigned char len)
{
	int t,temp;
	for(t = 0; t < len; t++)
	{
		temp = (num/oled_pow(10,len-t-1))%10;
		OLED_ShowChar(x+t, y, 48+temp);
	}
}

//显示数据的二进制值(0-255)
void OLED_ShowBite(int x, int y, u8 num)
{
	unsigned char t;
	for (t = 0; t < 8; t++)
	{
		OLED_ShowChar(x + 8 - t, y, 48 + (num >> t) % 2);
	}
}

//向左滚动，选择开始和结束页
void OLED_Scroll(char start, char stop)
{
	Write_IIC_Command(0x2E);	//关闭滚动
	Write_IIC_Command(0x27);	//水平向左滚动
	Write_IIC_Command(0x00);	//虚拟字节
	Write_IIC_Command(0x00);	//起始页 0
	Write_IIC_Command(0x00);	//滚动时间间隔
	Write_IIC_Command(0x03);	//终止页 1
	Write_IIC_Command(0x00);	//虚拟字节
	Write_IIC_Command(0xFF);	//虚拟字节
	Write_IIC_Command(0x2F);	//开启滚动
}

void OLED_NScroll()
{
	Write_IIC_Command(0x2E);	//关闭滚动
}

//初始化
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

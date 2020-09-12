
#ifndef __OLED_H__
#define __OLED_H__
#include "sys.h"


//IO方向设置
#define OLED_SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define OLED_SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//IO操作函数	 
#define OLED_IIC_SCL    PBout(6)  //SCL
#define OLED_IIC_SDA    PBout(7)  //SDA	 
#define OLED_READ_SDA   PBin(7)   //输入SDA 

void OLED_IIC_Init(void);         //IIC初始化


void Initial_M096128x64_ssd1306(void);             // OLED初始化
void Write_IIC_Command(unsigned char IIC_Command); // IIC Write Command
void Write_IIC_Data(unsigned char IIC_Data);       // IIC Write Data
void OLED_Fill_picture(unsigned char fill_Data);   //全屏显示(可用于清屏0x00)
void OLED_Clean(unsigned char i);                  //清除某一行(0 - 3)
void OLED_Picture(unsigned char show[]);           //显示一个图片

/**
* 显示一个图片(可以非全屏)
*
* @param x_start 横向开始坐标（单位：像素，0-127）
* @param length  横向长度（单位：像素）
* @param y_start 纵向开始坐标（单位：8个像素，0-7）
* @param width   纵向长度（单位：8个像素）
*/
void OLED_Picture_Part(unsigned char show[], unsigned char x_start, unsigned char length, unsigned char y_start, unsigned char width);
void OLED_ShowChar(int x, int y, char chr);        //显示单个字符 x:0-15 y:0-3(x>15换行显示)
void OLED_ShowStr(int x, int y, char show[]);      //输出字符串
void OLED_ShowNum(int x, int y, long int num, unsigned char len); //输出数字
void OLED_ShowBite(int x, int y, u8 num);          //显示数据的二进制值(0-255)
void OLED_Scroll(char start, char stop);           //向左滚动，选择开始和结束页
void OLED_NScroll(void);                           //关闭滚动

#endif


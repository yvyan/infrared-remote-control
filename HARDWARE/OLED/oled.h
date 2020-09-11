
#ifndef __OLED_H__
#define __OLED_H__
#include "sys.h"


//IO��������
#define OLED_SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define OLED_SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//IO��������	 
#define OLED_IIC_SCL    PBout(6)  //SCL
#define OLED_IIC_SDA    PBout(7)  //SDA	 
#define OLED_READ_SDA   PBin(7)   //����SDA 

void OLED_IIC_Init(void);         //IIC��ʼ��


void Initial_M096128x64_ssd1306(void);             // OLED��ʼ��
void Write_IIC_Command(unsigned char IIC_Command); // IIC Write Command
void Write_IIC_Data(unsigned char IIC_Data);       // IIC Write Data
void OLED_Fill_picture(unsigned char fill_Data);   //ȫ����ʾ(����������0x00)
void OLED_Clean(unsigned char i);                  //���ĳһ��(0 - 3)
void OLED_Picture(unsigned char show[]);           //��ʾһ��ͼƬ
void OLED_Picture_Part(unsigned char show[], unsigned char start, unsigned char width); // picture������ʾһ��ͼƬ(��ȫ��)
void OLED_ShowChar(int x, int y, char chr);        //��ʾ�����ַ� x:0-15 y:0-3(x>15������ʾ)
void OLED_ShowStr(int x, int y, char show[]);      //����ַ���
void OLED_ShowNum(int x, int y, long int num, unsigned char len); //�������
void OLED_ShowBite(int x, int y, u8 num);          //��ʾ���ݵĶ�����ֵ(0-255)
void OLED_Scroll(char start, char stop);           //���������ѡ��ʼ�ͽ���ҳ
void OLED_NScroll(void);                           //�رչ���

#endif


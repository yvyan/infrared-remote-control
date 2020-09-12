#include "oled.h"
#include "remote.h"
#include "pwm.h"
#include "irsend.h"
#include "usart.h"
#include "delay.h"
#include "remote_save.h"
#include "guet.h"


//��Ҫ�ĺ���ģ��
void showData(u32 data); //��ʾ������Ϣ
void data_Init(void);    //�������������ʼ��
void ShowSquareWave_Init(void); //��ʼ����������

void SetPart(void);   //��������1-9  ��Ҫ��Ϊ�������豸�����ԣ��ò�ͬ������ͬ����ʵ�ֲ�ͬ�Ĺ��ܣ������ĸ��ã�
int GetPart(void);    //�õ����ţ�����ҳ��ʾ�������û�����
void SendLearn(void); //ѧϰ/���ͺ���
void Delete(void);    //ɾ�����ݺ���
void ShowData(void);  //չʾ�����ź���Ϣ����



int key; //���ڼ�¼�û����µİ�������������Ϣ
u16 data[350]; //�����������飬�洢����ʱ������
u16 addr = 0;  //ƫ����
unsigned char show[128 * 2] = {0}; //������ʾ�洢����

int main()
{
	//��ʼ������
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);
	delay_init(72);
	OLED_IIC_Init();                //��ʼ��OLED IIC
	Initial_M096128x64_ssd1306();   //OLED��ʼ��
	OLED_Fill_picture(0x00); //����
	Remote_Init();
	TIM2_PWM_Init(1895,0);
	IR_SendPort_Init();
	ShowSquareWave_Init();
	
	//��ʾ��ӭҳ
	OLED_Picture_Part(guet, 0, 64, 0, 8);
	OLED_ShowStr(10, 1, "GUET");
	OLED_ShowStr(8, 3, "Welcome");
	Remote_Num(); //��ͣ
	
	//��ʾ��ҳ������ʾ��Ϣ
	while (1)
	{
		OLED_Fill_picture(0x00); //����
		OLED_ShowStr(0, 0, "1.SetPart( )");
		OLED_ShowNum(10, 0, GetPart(), 1);
		OLED_ShowStr(0, 1, "2.Send/Learn");
		OLED_ShowStr(0, 2, "3.Delete");
		OLED_ShowStr(0, 3, "4.ShowData");
		
		switch (Remote_Num()) //�������ֽ�����Ӧ�Ĺ���ģ��
		{
			case 1:
				SetPart();
				break;
			case 2:
				SendLearn();
				break;
			case 3:
				Delete();
				break;
			case 4:
				ShowData();
				break;
		}
	}
}


/***************************************************************************************************/

/**
* OLED��ʾ32λ���ݵ���Ϣ����߶����ƣ��ұ�ʮ���ƣ�
*
* @param data 32λ����
*/
void showData(u32 data) 
{
	OLED_Fill_picture(0x00); //����
	OLED_ShowBite(0, 0, data >> 24);
	OLED_ShowNum(10, 0, data >> 24, 3);
	
	OLED_ShowBite(0, 1, data >> 16);
	OLED_ShowNum(10, 1, (data >> 16) % 256, 3);
	
	OLED_ShowBite(0, 2, data >> 8);
	OLED_ShowNum(10, 2, (data >> 8) % 256, 3);
	
	OLED_ShowBite(0, 3, data);
	OLED_ShowNum(10, 3, data % 256, 3);
}

/**
* �������������ʼ��
*/
void data_Init(void)
{
	int i;
	for(i = 0; i < 350; i++)
	{
		data[i] = 0;
	}
}

/**
* ��������1-9  ��Ҫ��Ϊ�������豸�����ԣ��ò�ͬ������ͬ����ʵ�ֲ�ͬ�Ĺ��ܣ������ĸ��ã�
*/
void SetPart(void)
{
	OLED_Fill_picture(0x00); //����
	OLED_ShowStr(0, 0, "SetPart:0-9");
	while (1)
	{
		key = Remote_Num();
		if (key == POWER)
			break;
		else if (key >= 1 && key <= 9)
		{
			addr = (key - 1) * 20;
			break;
		}
	}
}

/**
* �õ����ţ�����ҳ��ʾ�������û�����
*/
int GetPart(void)
{
	return addr / 20 + 1;
}

/**
* ����ѧϰģʽ
*/
void DigitalLearn(void)
{
	while (1)
	{
		u32 get;
		OLED_Fill_picture(0x00); //����
		OLED_ShowStr(0, 0, "Select key!");
		key = Remote_Num();
		if (key == POWER)
			break;
		OLED_ShowStr(0, 1, "Getting...");
		get = Remote_GetData();
		showData(get);
		data[0] = (u16)get;
		data[1] = (u16)(get >> 16);
		SaveData(addr + key, 0, data, 2);
		delay_ms(100);
	}
}

/**
* ģ��ѧϰģʽ
*/
void AnalogLearn(void)
{
	while (1)
	{
		int a;
		OLED_Fill_picture(0x00); //����
		OLED_ShowStr(0, 0, "Select key!");
		key = Remote_Num();
		if (key == POWER)
			break;
		OLED_ShowStr(0, 1, "Input after 2s!");
		delay_ms(1000);
		OLED_ShowStr(0, 1, "Input after 1s!");
		delay_ms(1000);
		OLED_Clean(1);
		Re_Record_ON();
		OLED_ShowStr(5, 1, "Input!");
		delay_ms(1500);
		OLED_ShowStr(5, 2, "OK!");
		a = Re_Record_OFF();
		OLED_ShowNum(9, 2, a, 4);
		SaveData(addr + key, 1, Re_Record_Get(), a);
		delay_ms(400);
	}
}

/**
* ѧϰ/���ͺ���
*/
void SendLearn(void)
{
	int dot = 0; //���˼�¼�ϴ�ִ�����ĸ�������ȷ���Ƿ���Ҫ�����Ļ
	while (1)
	{
		if (dot == 0)
			OLED_Fill_picture(0x00); //����
		else if (dot == 1)
			OLED_Clean(3);
		else if (dot == 2)
			OLED_Clean(0);
		
		OLED_ShowStr(0, 0, "Mode:learn");
		OLED_ShowStr(0, 1, "POWER:exit");
		OLED_ShowStr(0, 2, "Other:send");
		dot = 0;
		key = Remote_Num();
		if (key == POWER)
			break;
		if (key == MODE)
		{
			// ѧϰģʽ
			while (1)
			{// ѡ��ѧϰģʽ
				OLED_Fill_picture(0x00); //����
				OLED_ShowStr(0, 0, "Learn Mode:");
				OLED_ShowStr(0, 1, "1.Digital");
				OLED_ShowStr(0, 2, "2.Analog");
				key = Remote_Num();
				if (key == POWER)
					break;
				else if (key == 1)
				{// ����ģʽ
					DigitalLearn();
				}
				else if (key == 2)
				{// ģ��ģʽ
					AnalogLearn();
				}
			}
		}
		else
		{
			// ����ģʽ
			int sta;
			data_Init();
			sta = GetData(addr + key, data);
			if (sta != -1)
			{
				OLED_ShowStr(2, 3, "SEND!");
				delay_ms(100);
				Remote_OFF();
				TR_SendTimeData(data, sta);
				Remote_ON();
				delay_ms(200);
				dot = 1;
			}
			else //δ��ȡ������
			{
				OLED_Fill_picture(0x00); //����
				OLED_ShowStr(0, 0, "Don't have data!");
				delay_ms(300);
				dot = 2;
			}
		}
	}
}

/**
* ɾ�����ݺ���
*/
void Delete(void)    //ɾ�����ݺ���
{
	OLED_Fill_picture(0x00); //����
	while (1)
	{
		OLED_Clean(3);
		OLED_ShowStr(0, 0, "Mode:del all");
		OLED_ShowStr(0, 1, "POWER:exit");
		OLED_ShowStr(0, 2, "Other:del one");
		key = Remote_Num();
		if (key == POWER)
			break;
		if (key == MODE)
		{
			DelPartData(addr);
			OLED_ShowStr(0, 3, "del all success!");
			delay_ms(300);
		}
		else
		{
			DelData(addr + key);
			OLED_ShowStr(0, 3, "del one success!");
			delay_ms(300);
		}
	}
}

/**
* ��ʼ����������
*/
void ShowSquareWave_Init(void)
{
	int i, j;
	
	show[0] = 0xfe;
	show[1] = 0xfe;
	show[128 + 0] = 0x7f;
	show[128 + 1] = 0x7f;
	
	j = 128 + 2;
	for (i = 0; i < 30; i++)
	{
		show[j + i] = 0x60;
	}
	
	show[32 + 0] = 0xfe;
	show[32 + 1] = 0xfe;
	show[128 + 32 + 0] = 0x7f;
	show[128 + 32 + 1] = 0x7f;
	
	j = 32 + 2;
	for (i = 0; i < 30; i++)
	{
		show[j + i] = 0x06;
	}
	
	show[64 + 0] = 0xfe;
	show[64 + 1] = 0xfe;
	show[128 + 64 + 0] = 0x7f;
	show[128 + 64 + 1] = 0x7f;
	
	j = 128 + 64 + 2;
	for (i = 0; i < 30; i++)
	{
		show[j + i] = 0x60;
	}
	
	show[96 + 0] = 0xfe;
	show[96 + 1] = 0xfe;
	show[128 + 96 + 0] = 0x7f;
	show[128 + 96 + 1] = 0x7f;
	
	j = 96 + 2;
	for (i = 0; i < 30; i++)
	{
		show[j + i] = 0x06;
	}
}

/**
* OLEDչʾ���κ����������Σ�
*/
void ShowSquareWave(void)
{
	OLED_Picture_Part(show, 0, 128, 4, 2);
}

/**
* չʾ������Ϣ
* @param pBuffer ʱ������ָ��
* @param lenth   ���鳤��
*/
void ShowDataAnalog(u16 * pBuffer, u16 lenth)
{
	int page = 1;
	OLED_Fill_picture(0x00); //����
	OLED_ShowStr(0, 0, "Mode:show lenth");
	OLED_ShowStr(0, 1, "Left:left page");
	OLED_ShowStr(0, 2, "right:right page");
	OLED_ShowStr(0, 3, "POWER:exit");
	while (1)
	{
		key = Remote_Num();
		if (key == POWER)
			break;
		if (key == MODE)
		{ //չʾ������Ϣ
			OLED_Fill_picture(0x00); //����
			OLED_ShowStr(0, 0, "Lenth:    ");
			OLED_ShowNum(6, 0, lenth, 4);
		}
		else if (key == LEFT || key == RIGHT || key == 1 || key == 9)
		{
			if (key == LEFT)
			{
				page -= 1;
				if (page <= 1)
					page = 1;
			}
			else if (key == RIGHT)
			{
				page += 1;
				if (page >= (lenth + 3) / 4)
					page = (lenth + 3) / 4;
			}
			else if (key == 1)
			{
				page = 1;
			}
			else if (key == 9)
			{
				page = (lenth + 3) / 4;
			}
			
			OLED_Fill_picture(0x00); //����
			OLED_ShowStr(0, 0, "P:   Start:    ");
			OLED_ShowNum(2, 0, page, 2);
			OLED_ShowNum(11, 0, (page - 1) * 4 + 1, 4);
			ShowSquareWave();
			OLED_ShowNum(0, 3, pBuffer[(page - 1) * 4 + 0 + 1], 4);
			OLED_ShowNum(4, 1, pBuffer[(page - 1) * 4 + 1 + 1], 4);
			OLED_ShowNum(8, 3, pBuffer[(page - 1) * 4 + 2 + 1], 4);
			OLED_ShowNum(12, 1, pBuffer[(page - 1) * 4 + 3 + 1], 4);
		}
	}
}

/**
* չʾ�����ź���Ϣ����
*/
void ShowData(void)
{
	while (1)
	{
		OLED_Fill_picture(0x00); //����
		OLED_ShowStr(0, 0, "Select From:");
		OLED_ShowStr(0, 1, "1.input");
		OLED_ShowStr(0, 2, "2.flash");
		OLED_ShowStr(0, 3, "POWER:exit");
		key = Remote_Num();
		if (key == POWER)
			break;
		if (key == 1)
		{//������Ϣչʾ
			while (1)
			{
				OLED_Fill_picture(0x00); //����
				OLED_ShowStr(0, 0, "Select Mode:");
				OLED_ShowStr(0, 1, "1.Digital");
				OLED_ShowStr(0, 2, "2.Analog");
				OLED_ShowStr(0, 3, "POWER:exit");
				key = Remote_Num();
				if (key == POWER)
					break;
				else if (key == 1)
				{// ����ģʽ
					while (1)
					{
						OLED_Fill_picture(0x00); //����
						OLED_ShowStr(0, 0, "Getting...");
						key = Remote_GetData();
						showData(key);
						key = Remote_Num(); //POWER�˳���������������
						if (key == POWER)
							break;
					}
				}
				else if (key == 2)
				{// ģ��ģʽ
					OLED_Fill_picture(0x00); //����
					OLED_ShowStr(0, 0, "Input after 2s!");
					delay_ms(1000);
					OLED_ShowStr(0, 0, "Input after 1s!");
					delay_ms(1000);
					OLED_Clean(0);
					Re_Record_ON();
					OLED_ShowStr(5, 0, "Input!");
					delay_ms(1500);
					OLED_ShowStr(5, 1, "OK!");
					
					ShowDataAnalog(Re_Record_Get(), Re_Record_OFF());
				}
			}
		}
		else if (key == 2)
		{//�洢��Ϣչʾ
			OLED_Fill_picture(0x00); //����
			OLED_ShowStr(0, 0, "Select key!");
			while (1)
			{
				key = Remote_Num();
				if (key == POWER)
					break;
				else
				{
					int sta;
					data_Init();
					sta = GetData(addr + key, data);
					if (sta != -1)
					{
						if (((sta >> 12) & 0x7) == 1) //ģ��ģʽ
							ShowDataAnalog(data, sta & 0x03FF);
						else if (((sta >> 12) & 0x7) == 0) //��׼ģʽ
						{
							key = (data[1] << 16) + data[0];
							showData(key);
						}
					}
					else //δ��ȡ������
					{
						OLED_Fill_picture(0x00); //����
						OLED_ShowStr(0, 0, "Don't have data!");
						delay_ms(300);
					}
				}
			}
			
		}
	}
}




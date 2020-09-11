#include "remote_save.h"

//16λ״̬��
//[15]:�Ƿ�洢������
//[14:12]:�������ͣ�Ŀǰ��֪��0->NEC; 1->ģ�����洢; .....��
//[9:0]:���ݴ�С��1-700��
//���700�����֣�u16��

//��ȡ��ַ��addr + N * (350 + 1)

/**
* ɾ���������򱣴��־λ���״�ʹ����ִ�У�
*/
void Save_Init(void)
{
	int i;
	u16 sta = 0;
	for (i = 0; i < DATA_NUMBER; i++)
		STMFLASH_Write(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2), &sta, 1);
}

/**
* ɾ��ĳ���ռ䱣���־λ����ɾ����¼��
* @param i �ռ����
*/
void DelData(int i)
{
	u16 sta = 0;
	STMFLASH_Write(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2), &sta, 1);
}

/**
* ɾ��ĳ�������������򱣴��־λ����ɾ����¼��
* @param addr �����׵�ַ
*/
void DelPartData(int addr)
{
	int i;
	u16 sta = 0;
	for (i = addr; i < addr + 20; i++)
		STMFLASH_Write(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2), &sta, 1);
}

/**
* ���ĳһ�ռ��Ƿ�洢������
* @param i �ռ����
* @return 0:δ����; 1:�ѱ���
*/
int isSave(int i)
{
	if (i >= 0 && i < DATA_NUMBER)
	{
		u16 sta = STMFLASH_ReadHalfWord(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2));
		return sta >> 15;
	}
	else
	{
		return -1;
	}
}


/**
* д���������
* @param i       �ռ����
* @param type    �������ͣ�Ŀǰ��֪��0->NEC; 1->ģ�����洢; .....��
* @param pBuffer ����ָ��
* @param lenth   ����(16λ)��
* @return д����
*/
int SaveData(int i, u8 type, u16 * pBuffer, u16 lenth)
{
	if (i >= 0 && i < DATA_NUMBER && type < 8 && lenth < 1024) //��ַ�͸�ʽ��ȷ
	{
		u16 sta = 0x8000;
		sta |= type << 12;
		sta += lenth;
		STMFLASH_Write(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2), &sta, 1);
		STMFLASH_Write(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2) + 2, pBuffer, lenth);
		return 0;
	}
	else
	{
		return -1;
	}
}

/**
* �õ���������
* @param i       �ռ����
* @param pBuffer ����ָ��
* @return 16λ״̬��
*         -1:��ַ�������δ�洢����
*/
int GetData(int i, u16 * pBuffer)
{
	if (i >= 0 && i < DATA_NUMBER && isSave(i)) //��ַ��ȷ
	{
		u16 sta = STMFLASH_ReadHalfWord(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2));
		STMFLASH_Read(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2) + 2, pBuffer, sta & 0x03FF);
		return sta;
	}
	else
	{
		return -1;
	}
}


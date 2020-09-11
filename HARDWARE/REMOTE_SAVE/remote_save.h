#ifndef __REMOTE_SAVE_H__
#define __REMOTE_SAVE_H__
#include "stmflash.h"

#define FLASH_SAVE_ADDR  0X08010000 //����FLASH �����ַ(����Ϊż��������ֵҪ���ڱ�������ռ��FLASH�Ĵ�С+0X08000000)RCT6���3FFFF

#define DATA_NUMBER 180
#define MAX_LENTH   (350 + 1)

//16λ״̬��
//[15]:�Ƿ�洢������
//[14:12]:�������ͣ�Ŀǰ��֪��0->NEC; 1->ģ�����洢; .....��
//[9:0]:���ݴ�С��1-350��
//���700�����֣�u16��

//��ȡ��ַ��addr + N * (350 + 1)

/**
* ɾ���������򱣴��־λ���״�ʹ����ִ�У�
*/
void Save_Init(void);

/**
* ���ĳһ�ռ��Ƿ�洢������
* @param i �ռ����
* @return 0:δ����; 1:�ѱ���
*/
int isSave(int i);

/**
* ɾ��ĳ���ռ䱣���־λ����ɾ����¼��
* @param i �ռ����
*/
void DelData(int i);

/**
* ɾ��ĳ�������������򱣴��־λ����ɾ����¼��
* @param addr �����׵�ַ
*/
void DelPartData(int addr);

/**
* д���������
* @param i       �ռ����
* @param type    �������ͣ�Ŀǰ��֪��0->NEC; 1->ģ�����洢; .....��
* @param pBuffer ����ָ��
* @param lenth   ����(16λ)��
* @return д����
*/
int SaveData(int i, u8 type, u16 * pBuffer, u16 lenth);

/**
* �õ���������
* @param i       �ռ����
* @param pBuffer ����ָ��
* @return 16λ״̬��
*         -1:��ַ�������δ�洢����
*/
int GetData(int i, u16 * pBuffer);

#endif

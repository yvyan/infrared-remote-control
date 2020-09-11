#ifndef __RED_H
#define __RED_H 
#include "sys.h"   

#define RDATA 	PBin(9)	 //�������������

#define USD   10
#define RPT   11
#define ADD   12
#define LESS  13
#define PLAY  14
#define RIGHT 15
#define VOLX  16
#define LEFT  17
#define MODE  18
#define EQ      19
#define POWER   20

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
//ң����ʶ����Ϊ0
#define REMOTE_ID 0

extern u8 RmtCnt;			  //�������µĴ���

/**
* �򿪺������
*/
void Remote_ON(void);

/**
* �رպ������
*/
void Remote_OFF(void);

void Remote_Init(void); //���⴫��������ͷ���ų�ʼ��
u8 Remote_Scan(void);   //ң��ɨ��
u8 Remote_Num(void);    //��������

/**
* ���ң��������ֵ
* @return ����ֵ
*/
u16 Remote_Get(void);

/**
* ������¼ģʽ
* @return ��¼����ĳ���
*/
void Re_Record_ON(void);

/**
* �رռ�¼ģʽ
*/
u16 Re_Record_OFF(void);

/**
* ��ȡ��¼��������
* @return ��������
*/
u16* Re_Record_Get(void);

/**
* ȷ���׸������Ǹߵ͵�ƽ
*/
u8 Re_Get_Type(void);

u8 Remote_Test1(void);
u32 Remote_Test2(void);
u32 Remote_Test3(void);

#endif

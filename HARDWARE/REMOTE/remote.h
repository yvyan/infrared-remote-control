#ifndef __RED_H
#define __RED_H 
#include "sys.h"   

#define RDATA 	PBin(9)	 //红外数据输入脚

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

//单片机遥控器识别码为0
#define REMOTE_ID 0

extern u8 RmtCnt;			  //按键按下的次数

/**
* 打开红外接收
*/
void Remote_ON(void);

/**
* 关闭红外接收
*/
void Remote_OFF(void);

/**
* 红外传感器接收头引脚初始化
*/
void Remote_Init(void);

/**
* 遥控扫描
*
* @return 0,没有任何按键按下；其他：按下的按键键值
*/
u8 Remote_Scan(void);


/**
* 反馈数字（直到按键按下）
*
* @return 按下的按键值
*/
u8 Remote_Num(void);
/***************
0:   ERROR
162: POWER 20
98:  MODE  18
2:   LEFT  17
226: VOLX	 16
194: RIGHT 15
34:  PLAY	 14
224: EQ	   19
168: VOL-	 13
144: VOL+	 12
104: 0	   0
152: RPT   11
176: USD   10
48:  1	   1
24:  2	   2
122: 3	   3
16:  4		 4  					
56:  5     5
90:  6     6
66:  7     7
74:  8     8
82:  9     9
***************/

/**
* 获得遥控器数据值
* @return 数据值
*/
u16 Remote_Get(void);

/**
* 开启记录模式
* @return 记录数组的长度
*/
void Re_Record_ON(void);

/**
* 关闭记录模式
*/
u16 Re_Record_OFF(void);

/**
* 获取记录数组数据
* @return 数据内容
*/
u16* Re_Record_Get(void);

/**
* 确定首个数据是高低电平
*/
u8 Re_Get_Type(void);

/**
* 得到32位红外数据信息
*
* @return 接受到的32位数据
*/
u32 Remote_GetData(void);

#endif

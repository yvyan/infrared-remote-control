#ifndef __REMOTE_SAVE_H__
#define __REMOTE_SAVE_H__
#include "stmflash.h"

//该驱动是再stmflash驱动的基础上进行了抽象，目的是为了方便红外数据的存储

#define FLASH_SAVE_ADDR  0X08010000 //设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)RCT6最大3FFFF

#define DATA_NUMBER 180             //设置最大的空间数
#define MAX_LENTH   (350 + 1)       //设置一个空间占用flash大小（单位：16位）

//16位状态字表示的信息：
//
//[15]:是否存储了数据
//[14:12]:数据类型（目前已知：0->NEC; 1->模拟编码存储; .....）
//[9:0]:数据大小（1-350）
//后接700个半字（u16）

//读取地址：addr + N * (350 + 1)

/**
* 初始化所有区域保存标志位（首次使用请执行）
* flash里面未使用空间默认为1，这会导致程序判断错误，所以执行这个函数可以初始化标志位（状态字[15]）
*/
void Save_Init(void);

/**
* 检测某一空间是否存储了数据
* @param i 空间编码
* @return 0:未保存; 1:已保存
*/
int isSave(int i);

/**
* 删除某个空间保存标志位（即删除记录）
* @param i 空间编码
*/
void DelData(int i);

/**
* 删除某个区号所有区域保存标志位（即删除记录）
* @param addr 区域首地址
*/
void DelPartData(int addr);

/**
* 写入红外数据
* @param i       空间编码
* @param type    数据类型（目前已知：0->NEC; 1->模拟编码存储; .....）
* @param pBuffer 数据指针
* @param lenth   半字(16位)数
* @return 写入结果
*/
int SaveData(int i, u8 type, u16 * pBuffer, u16 lenth);

/**
* 得到红外数据
* @param i       空间编码
* @param pBuffer 数据指针
* @return 16位状态字
*         -1:地址错误或者未存储数据
*/
int GetData(int i, u16 * pBuffer);

#endif

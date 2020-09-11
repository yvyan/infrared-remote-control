#include "remote_save.h"

//16位状态字
//[15]:是否存储了数据
//[14:12]:数据类型（目前已知：0->NEC; 1->模拟编码存储; .....）
//[9:0]:数据大小（1-350）
//后接700个半字（u16）

//读取地址：addr + N * (350 + 1)

/**
* 初始化所有区域保存标志位（首次使用请执行）
* flash里面未使用空间默认为1，这会导致程序判断错误，所以执行这个函数可以初始化标志位（状态字[15]）
*/
void Save_Init(void)
{
	int i;
	u16 sta = 0;
	for (i = 0; i < DATA_NUMBER; i++)
		STMFLASH_Write(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2), &sta, 1);
}

/**
* 删除某个空间保存标志位（即删除记录）
* @param i 空间编码
*/
void DelData(int i)
{
	u16 sta = 0;
	STMFLASH_Write(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2), &sta, 1);
}

/**
* 删除某个区号所有区域保存标志位（即删除记录）
* @param addr 区域首地址
*/
void DelPartData(int addr)
{
	int i;
	u16 sta = 0;
	for (i = addr; i < addr + 20; i++)
		STMFLASH_Write(FLASH_SAVE_ADDR + i * (MAX_LENTH * 2), &sta, 1);
}

/**
* 检测某一空间是否存储了数据
* @param i 空间编码
* @return 0:未保存; 1:已保存
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
* 写入红外数据
* @param i       空间编码
* @param type    数据类型（目前已知：0->NEC; 1->模拟编码存储; .....）
* @param pBuffer 数据指针
* @param lenth   半字(16位)数
* @return 写入结果
*/
int SaveData(int i, u8 type, u16 * pBuffer, u16 lenth)
{
	if (i >= 0 && i < DATA_NUMBER && type < 8 && lenth < 1024) //地址和格式正确
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
* 得到红外数据
* @param i       空间编码
* @param pBuffer 数据指针
* @return 16位状态字
*         -1:地址错误或者未存储数据
*/
int GetData(int i, u16 * pBuffer)
{
	if (i >= 0 && i < DATA_NUMBER && isSave(i)) //地址正确
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


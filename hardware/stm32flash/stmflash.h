#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "sys.h"
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F7开发板
//STM32内部FLASH读写 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/31
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
#define FLASH_WAITETIME  50000          //FLASH等待超时时间

//STM32F767 FLASH 扇区的起始地址
    #define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) //扇区0起始地址, 32 Kbytes 
    #define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08008000) //扇区1起始地址, 32 Kbytes 
    #define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08010000) //扇区2起始地址, 32 Kbytes 
    #define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08018000) //扇区3起始地址, 32 Kbytes 
    #define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08020000) //扇区4起始地址, 128 Kbytes 
    #define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08040000) //扇区5起始地址, 256 Kbytes 
    #define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08080000) //扇区6起始地址, 256 Kbytes 
    #define ADDR_FLASH_SECTOR_7     ((uint32_t)0x080C0000) //扇区7起始地址, 256 Kbytes 
    #define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08100000) //扇区8起始地址, 256 Kbytes  
    #define ADDR_FLASH_SECTOR_9     ((uint32_t)0x08140000) //扇区9起始地址, 256 Kbytes  
    #define ADDR_FLASH_SECTOR_10    ((uint32_t)0x08180000) //扇区10起始地址,256 Kbytes  
    #define ADDR_FLASH_SECTOR_11    ((uint32_t)0x081C0000) //扇区11起始地址,256 Kbytes   


u32 STMFLASH_ReadWord(u32 faddr);		  	//读出字  
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//从指定地址开始读出指定长度的数据

void STMFLASH_EraseSector(u32 WriteAddr);
u32 STMFLASH_WriteDate(u32 WriteAddr,u8 *pBuffer,u16 numToWrite);

#endif

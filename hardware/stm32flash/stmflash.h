#ifndef __STMFLASH_H
#define __STMFLASH_H
#include "sys.h"
#include "stdio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//STM32�ڲ�FLASH��д ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/31
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
#define FLASH_WAITETIME  50000          //FLASH�ȴ���ʱʱ��

//STM32F767 FLASH ��������ʼ��ַ
    #define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) //����0��ʼ��ַ, 32 Kbytes 
    #define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08008000) //����1��ʼ��ַ, 32 Kbytes 
    #define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08010000) //����2��ʼ��ַ, 32 Kbytes 
    #define ADDR_FLASH_SECTOR_3     ((uint32_t)0x08018000) //����3��ʼ��ַ, 32 Kbytes 
    #define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08020000) //����4��ʼ��ַ, 128 Kbytes 
    #define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08040000) //����5��ʼ��ַ, 256 Kbytes 
    #define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08080000) //����6��ʼ��ַ, 256 Kbytes 
    #define ADDR_FLASH_SECTOR_7     ((uint32_t)0x080C0000) //����7��ʼ��ַ, 256 Kbytes 
    #define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08100000) //����8��ʼ��ַ, 256 Kbytes  
    #define ADDR_FLASH_SECTOR_9     ((uint32_t)0x08140000) //����9��ʼ��ַ, 256 Kbytes  
    #define ADDR_FLASH_SECTOR_10    ((uint32_t)0x08180000) //����10��ʼ��ַ,256 Kbytes  
    #define ADDR_FLASH_SECTOR_11    ((uint32_t)0x081C0000) //����11��ʼ��ַ,256 Kbytes   


u32 STMFLASH_ReadWord(u32 faddr);		  	//������  
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

void STMFLASH_EraseSector(u32 WriteAddr);
u32 STMFLASH_WriteDate(u32 WriteAddr,u8 *pBuffer,u16 numToWrite);

#endif

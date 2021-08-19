#include "stmflash.h"
#include "delay.h"
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

//��ȡָ����ַ����(32λ����) 
//faddr:����ַ 
//����ֵ:��Ӧ����.
u32 STMFLASH_ReadWord(u32 faddr)
{
	return *(__IO uint32_t *)faddr; 
}

//��ȡĳ����ַ���ڵ�flash����
//addr:flash��ַ
//����ֵ:0~11,��addr���ڵ�����
uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_SECTOR_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_SECTOR_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_SECTOR_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_SECTOR_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_SECTOR_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_SECTOR_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_SECTOR_6;
        else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_SECTOR_7;
        else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_SECTOR_8;
        else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_SECTOR_9;
        else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_SECTOR_10;
	return FLASH_SECTOR_11;	
}

//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ر�ע��:��ΪSTM32F7������ʵ��̫��,û�취���ر�����������,���Ա�����
//         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
//         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
//         û����Ҫ����,��������������Ȳ�����,Ȼ����������д. 
//�ú�����OTP����Ҳ��Ч!��������дOTP��!
//OTP�����ַ��Χ:0X1FF0F000~0X1FF0F41F
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
//pBuffer:����ָ��
//NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���.) 
void STMFLASH_Write(u32 WriteAddr,u32 *pBuffer,u32 NumToWrite)	
{ 
    FLASH_EraseInitTypeDef FlashEraseInit;
    HAL_StatusTypeDef FlashStatus=HAL_OK;
    u32 SectorError=0;
	u32 addrx=0;
	u32 endaddr=0;	
    if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
    
 	HAL_FLASH_Unlock();             //����	
	addrx=WriteAddr;				//д�����ʼ��ַ
	endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
    
    if(addrx<0X1FF00000)
    {
        while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
		{
			if(STMFLASH_ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
			{   
                FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //�������ͣ��������� 
                FlashEraseInit.Sector=STMFLASH_GetFlashSector(addrx);   //Ҫ����������
                FlashEraseInit.NbSectors=1;                             //һ��ֻ����һ������
                FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //��ѹ��Χ��VCC=2.7~3.6V֮��!!
                if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
                {
                    break;//����������	
                }
                SCB_CleanInvalidateDCache();                            //�����Ч��D-Cache
			}else addrx+=4;
            FLASH_WaitForLastOperation(FLASH_WAITETIME);                //�ȴ��ϴβ������
        }
    }
    FlashStatus=FLASH_WaitForLastOperation(FLASH_WAITETIME);            //�ȴ��ϴβ������
	if(FlashStatus==HAL_OK)
	{
		while(WriteAddr<endaddr)//д����
		{
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,*pBuffer)!=HAL_OK)//д������
			{ 
				break;	//д���쳣
			}
			WriteAddr+=4;
			pBuffer++;
		} 
	}
	HAL_FLASH_Lock();           //����
} 

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToRead:��(32λ)��
void STMFLASH_Read(u32 ReadAddr,u32 *pBuffer,u32 NumToRead)   	
{
	u32 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadWord(ReadAddr);//��ȡ4���ֽ�.
		ReadAddr+=4;//ƫ��4���ֽ�.	
	}
}

//��������
void STMFLASH_EraseSector(u32 WriteAddr)
{
        FLASH_EraseInitTypeDef FlashEraseInit;
        u32 SectorError=0;
        
        HAL_FLASH_Unlock();             //����	
        FlashEraseInit.TypeErase=FLASH_TYPEERASE_SECTORS;       //�������ͣ��������� 
        FlashEraseInit.Sector=STMFLASH_GetFlashSector(WriteAddr);   //Ҫ����������
        FlashEraseInit.NbSectors=1;                             //һ��ֻ����һ������
        FlashEraseInit.VoltageRange=FLASH_VOLTAGE_RANGE_3;      //��ѹ��Χ��VCC=2.7~3.6V֮��!!
        if(HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError)!=HAL_OK) 
        {
                printf("��������\n");	
        }
        SCB_CleanInvalidateDCache();                            //�����Ч��D-Cache
        HAL_FLASH_Lock();           //����
}

//����д��
u32 STMFLASH_WriteDate(u32 WriteAddr,u8 *pBuffer,u16 numToWrite)
{
        HAL_FLASH_Unlock();	        //����
        s16 number=numToWrite;
        u32 buffer;
	while(number>3)       //д����
	{
              buffer=(((u32)(*(pBuffer+3)))<<24) + ((*(pBuffer+2))<<16) + ((*(pBuffer+1))<<8) + (*(pBuffer+0));
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,WriteAddr,buffer)!=HAL_OK)//д������
		{ 
                        printf("д�����\n");
			break;	//д���쳣
		}
		WriteAddr+=4;
		pBuffer+=4;
                number-=4;
	} 
//        uint64_t buffer;
//        u32 bufferHigh;
//        u32 bufferLow;
//	while(number>7)       //д����
//	{
//                bufferHigh=((*(pBuffer+7))<<24) + ((*(pBuffer+6))<<16) + ((*(pBuffer+5))<<8) + (*(pBuffer+4));
//                bufferLow=((*(pBuffer+3))<<24) + ((*(pBuffer+2))<<16) + ((*(pBuffer+1))<<8) + (*(pBuffer+0));
//                buffer=(uint64_t)bufferHigh<<32 + (uint64_t)bufferLow;
//		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,WriteAddr,buffer)!=HAL_OK)//д������
//		{ 
//                        printf("д�����\n");
//			break;	//д���쳣
//		}
//		WriteAddr+=8;
//		pBuffer+=8;
//                number-=8;
//	} 
        while(number)       //д����
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,WriteAddr,*pBuffer)!=HAL_OK)//д������
		{ 
                        printf("д�����\n");
			break;	//д���쳣
		}
		WriteAddr++;
		pBuffer++;
                number--;
	}
        HAL_FLASH_Lock();           //����
        return WriteAddr;
}

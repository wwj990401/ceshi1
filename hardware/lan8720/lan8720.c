#include "lan8720.h"
#include "lwip_comm.h"
#include "delay.h"
#include "malloc.h"
#include "includes.h"

ETH_HandleTypeDef ETH_Handler = {0};      //��̫�����

ETH_DMADescTypeDef *DMARxDscrTab;	//��̫��DMA�������������ݽṹ��ָ��
ETH_DMADescTypeDef *DMATxDscrTab;	//��̫��DMA�������������ݽṹ��ָ�� 
uint8_t *Rx_Buff; 					//��̫���ײ���������buffersָ�� 
uint8_t *Tx_Buff; 					//��̫���ײ���������buffersָ��
  
u8 LAN8720_Init(void)
{      
  u8 macaddress[6];
    


  macaddress[0]=lwipdev.mac[0]; 
	macaddress[1]=lwipdev.mac[1]; 
	macaddress[2]=lwipdev.mac[2];
	macaddress[3]=lwipdev.mac[3];   
	macaddress[4]=lwipdev.mac[4];
	macaddress[5]=lwipdev.mac[5];
        
	ETH_Handler.Instance=ETH;
  ETH_Handler.Init.AutoNegotiation=ETH_AUTONEGOTIATION_DISABLE;//ʹ����Э��ģʽ 
  ETH_Handler.Init.Speed=ETH_SPEED_100M;//�ٶ�100M,�����������Э��ģʽ�������þ���Ч
	ETH_Handler.Init.DuplexMode=ETH_MODE_HALFDUPLEX;
  ETH_Handler.Init.PhyAddress=LAN8720_PHY_ADDRESS;//LAN8720��ַ  
  ETH_Handler.Init.MACAddr=macaddress;            //MAC��ַ  
  ETH_Handler.Init.RxMode=ETH_RXINTERRUPT_MODE;   //�жϽ���ģʽ 
  ETH_Handler.Init.ChecksumMode=ETH_CHECKSUM_BY_HARDWARE;//Ӳ��֡У��  
  ETH_Handler.Init.MediaInterface=ETH_MEDIA_INTERFACE_RMII;//RMII�ӿ�  
  if(HAL_ETH_Init(&ETH_Handler)==HAL_OK)
  {                         //���������ж�  
    return 0;   //�ɹ�
  }
  else return 1;  //ʧ��
}

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_ETH_CLK_ENABLE();             //����ETHʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
	  __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	//����GPIOGʱ��
	
    GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7; 
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //���츴��
    GPIO_Initure.Pull=GPIO_NOPULL;              //����������
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
    GPIO_Initure.Alternate=GPIO_AF11_ETH;       //����ΪETH����
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);         //��ʼ��
    
   
    //PC1,4,5
    GPIO_Initure.Pin=GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5; //PC1,4,5,6
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);         //��ʼ��
			
    //PG13,14
    GPIO_Initure.Pin=GPIO_PIN_11;   //PG13,14
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);         //��ʼ��
		
		GPIO_Initure.Pin=GPIO_PIN_12|GPIO_PIN_13;
		HAL_GPIO_Init(GPIOB,&GPIO_Initure); 
	  //����PD3Ϊ�������
		GPIO_Initure.Pin=GPIO_PIN_3; 
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP; 
    GPIO_Initure.Pull=GPIO_NOPULL;          
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //����
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);    
		    
    HAL_NVIC_SetPriority(ETH_IRQn,0,0);         //�����ж����ȼ�Ӧ�ø�һ��
    HAL_NVIC_EnableIRQ(ETH_IRQn);
	  INTX_DISABLE();                         //�ر������жϣ���λ���̲��ܱ���ϣ�
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_RESET);       //Ӳ����λ
    delay_ms(50);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_3,GPIO_PIN_SET);        //��λ����
    delay_ms(50);
    INTX_ENABLE(); 
}
void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
{
  if(ethHandle->Instance==ETH)
  {
  /* USER CODE BEGIN ETH_MspDeInit 0 */

  /* USER CODE END ETH_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ETH_CLK_DISABLE();


	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_12|GPIO_PIN_13);
			HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11);

    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(ETH_IRQn);

  }
}

//��ȡPHY�Ĵ���ֵ
u32 LAN8720_ReadPHY(u16 reg)
{
    u32 regval;
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&regval);
    return regval;
}
//��LAN8720ָ���Ĵ���д��ֵ
//reg:Ҫд��ļĴ���
//value:Ҫд���ֵ
void LAN8720_WritePHY(u16 reg,u16 value)
{
    u32 temp=value;
    HAL_ETH_ReadPHYRegister(&ETH_Handler,reg,&temp);
}

u8 LAN8720_Get_Speed(void)
{
	u8 speed;
	speed=((LAN8720_ReadPHY(31)&0x1C)>>2); //��LAN8720��31�żĴ����ж�ȡ�����ٶȺ�˫��ģʽ
	return speed;
}

extern void lwip_pkt_handle(void);		//��lwip_comm.c���涨��

//�жϷ�����
void ETH_IRQHandler(void)
{
    OSIntEnter(); 
    while(ETH_GetRxPktSize(ETH_Handler.RxDesc))   
    {
        lwip_pkt_handle();//������̫�����ݣ����������ύ��LWIP
    }
    //����жϱ�־λ
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_R); 
    __HAL_ETH_DMA_CLEAR_IT(&ETH_Handler,ETH_DMA_IT_NIS); 
    OSIntExit();  
}

//��ȡ���յ���֡����
//DMARxDesc:����DMA������
//����ֵ:���յ���֡����
u32  ETH_GetRxPktSize(ETH_DMADescTypeDef *DMARxDesc)
{
    u32 frameLength = 0;
    if(((DMARxDesc->Status&ETH_DMARXDESC_OWN)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_ES)==(uint32_t)RESET) &&
     ((DMARxDesc->Status&ETH_DMARXDESC_LS)!=(uint32_t)RESET)) 
    {
        frameLength=((DMARxDesc->Status&ETH_DMARXDESC_FL)>>ETH_DMARXDESC_FRAME_LENGTHSHIFT);
    }
    return frameLength;
}

//ΪETH�ײ����������ڴ�
//����ֵ:0,����
//    ����,ʧ��
u8 ETH_Mem_Malloc(void)
{ 
	DMARxDscrTab=mymalloc(SRAMDTCM,ETH_RXBUFNB*sizeof(ETH_DMADescTypeDef));//�����ڴ�
	DMATxDscrTab=mymalloc(SRAMDTCM,ETH_TXBUFNB*sizeof(ETH_DMADescTypeDef));//�����ڴ�  
	Rx_Buff=mymalloc(SRAMDTCM,ETH_RX_BUF_SIZE*ETH_RXBUFNB);	//�����ڴ�
	Tx_Buff=mymalloc(SRAMDTCM,ETH_TX_BUF_SIZE*ETH_TXBUFNB);	//�����ڴ�
	if(!(u32)&DMARxDscrTab||!(u32)&DMATxDscrTab||!(u32)&Rx_Buff||!(u32)&Tx_Buff)
	{
		ETH_Mem_Free();
		return 1;	//����ʧ��
	}	
	return 0;		//����ɹ�
}

//�ͷ�ETH �ײ�����������ڴ�
void ETH_Mem_Free(void)
{ 
	myfree(SRAMDTCM,DMARxDscrTab);//�ͷ��ڴ�
	myfree(SRAMDTCM,DMATxDscrTab);//�ͷ��ڴ�
	myfree(SRAMDTCM,Rx_Buff);		//�ͷ��ڴ�
	myfree(SRAMDTCM,Tx_Buff);		//�ͷ��ڴ�  
}




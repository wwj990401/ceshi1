#include "usmart.h"
#include "usmart_str.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
#include "delay.h"	 	
#include "sys.h"
#include "led.h"
#include "lan8720.h"
#include "lwip_tcp_client_socket.h"
#include "lwip_tcp_server_socket.h"
#include "stmflash.h"
								 									  
//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		   
        (void*)led_out,"void led_out(int n)",
        (void*)LAN8720_ReadPHY,"u32 LAN8720_ReadPHY(u16 reg)",	
        (void*)Init_Client,"void Init_Client(void)",
        (void*)Socket_init,"void Socket_init(void)",
        (void*)Send_File_Name,"void Send_File_Name(char* fileName)",
        (void*)Pause_Download,"void Pause_Download(void)",
        (void*)Continue_Download,"void Continue_Download(void)",
        (void*)Stop_Download,"void Stop_Download(void)",
        (void*)STMFLASH_EraseSector,"void STMFLASH_EraseSector(u32 WriteAddr)",
};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   




















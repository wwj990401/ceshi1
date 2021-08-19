#include "lwip_tcp_client_socket.h"

//��ʼ���ͻ�������
//�������ȼ�
#define INIT_CLIENT_TASK_PRIO	        15
//�����ջ��С
#define INIT_CLIENT_STK_SIZE		128
//�����ջ
OS_STK	INIT_CLIENT_TASK_STK[INIT_CLIENT_STK_SIZE];

//���ӷ���������
//�������ȼ�
#define CONNECT_SERVER_TASK_PRIO	14
//�����ջ��С
#define CONNECT_SERVER_STK_SIZE	        128
//�����ջ
OS_STK	CONNECT_SERVER_TASK_STK[CONNECT_SERVER_STK_SIZE];

//���շ�������Ϣ����
//�������ȼ�
#define REV_SERVER_TASK_PRIO	        13
//�����ջ��С
#define REV_SERVER_STK_SIZE		1600
//�����ջ
OS_STK	REV_SERVER_TASK_STK[REV_SERVER_STK_SIZE];

//�����ļ�·������������
//�������ȼ�
#define SEND_FILENAME_TASK_PRIO	        12
//�����ջ��С
#define SEND_FILENAME_STK_SIZE		256
//�����ջ
OS_STK	SEND_FILENAME_TASK_STK[SEND_FILENAME_STK_SIZE];

//�����ļ�·������������
//�������ȼ�
#define STOP_DOWNLOAD_TASK_PRIO	        11
//�����ջ��С
#define STOP_DOWNLOAD_STK_SIZE		128
//�����ջ
OS_STK	STOP_DOWNLOAD_TASK_STK[STOP_DOWNLOAD_STK_SIZE];

int sock_connect;	        //�ͻ����׽���
char g_fileName[100];           //�ļ����ּ�·��
u32 g_fileSize;                 //�ļ���С

//������ʼ���ͻ�������
void Init_Client(void)
{
        OS_CPU_SR cpu_sr=0;
        OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
	OSTaskCreate(Init_Client_task,(void*)0,(OS_STK*)&INIT_CLIENT_TASK_STK[INIT_CLIENT_STK_SIZE-1],INIT_CLIENT_TASK_PRIO);
        OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
}

//��ʼ���ͻ�������
void Init_Client_task(void *pdata)
{
        close(sock_connect);         //һ��ֻ����һ������ 
        OS_CPU_SR cpu_sr=0;
	//�����ͻ����׽���
	sock_connect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //�ͻ����׽���
	if (sock_connect == -1)
	{
		printf("\n�ͻ����׽��ִ���ʧ��!\n");
		OSTaskDel(OS_PRIO_SELF);
	}
        printf("\n�ͻ����׽��ִ����ɹ�!\n");
	OS_ENTER_CRITICAL();    //�����ٽ���(�ر��ж�)
	OSTaskCreate(Connect_Server_task,(void*)0,(OS_STK*)&CONNECT_SERVER_TASK_STK[CONNECT_SERVER_STK_SIZE-1],CONNECT_SERVER_TASK_PRIO);
        OS_EXIT_CRITICAL();     //�˳��ٽ���(���ж�)
        OSTaskDel(OS_PRIO_SELF);
}

//���ӷ���������
void Connect_Server_task(void *pdata)
{
        OS_CPU_SR cpu_sr=0;
        struct sockaddr_in server_addr;
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.10");
	server_addr.sin_port = htons(8000);
        
	if(connect(sock_connect, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) //��ָ��IP��ַ�Ͷ˿ڵķ��������
	{
                close(sock_connect);         //һ��ֻ����һ������ 
		printf("����������ʧ��!\n");
		OSTaskDel(OS_PRIO_SELF); 
	}
	printf("���������ӳɹ�!\n");
        
        int val = SO_KEEPALIVE;                 //��������֡���
        setsockopt(sock_connect, SOL_SOCKET, SO_KEEPALIVE, &val, 4); 
        int keepIdle = 3;                       //�״�̽�⿪ʼǰ��tcp�������շ�����ʱ��
        int keepInterval = 1;                   //ÿ��̽��ļ��ʱ��
        int keepCount = 2;                      //̽�����
        setsockopt(sock_connect, IPPROTO_TCP, TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle)); 
        setsockopt(sock_connect, IPPROTO_TCP,TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval)); 
        setsockopt(sock_connect,IPPROTO_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
        
        OS_ENTER_CRITICAL();    //�����ٽ���(�ر��ж�)
	OSTaskCreate(Rev_Server_File_task,(void*)0,(OS_STK*)&REV_SERVER_TASK_STK[REV_SERVER_STK_SIZE-1],REV_SERVER_TASK_PRIO);
        OS_EXIT_CRITICAL();     //�˳��ٽ���(���ж�)
	OSTaskDel(OS_PRIO_SELF); 
}

//�����ļ�����
void Rev_Server_File_task(void *pdata)
{
        
  	s16 newLength = 0;
        u16 lastLength = 0;
	u8 receiveData[DATA_MAX_LENGTH];         //�������ݳ���
        u8 flag=0;                               //�쳣��־
        u32 writeAddress;                        //���ص�ַ
        char buf[1];
        
        while (1) 
        {
                memset(receiveData, 0, DATA_MAX_LENGTH);
                newLength = recv(sock_connect, receiveData, DATA_MAX_LENGTH, 0);      //���շ�������Ϣ
                if(newLength <= 0 || newLength > DATA_MAX_LENGTH)
                {
                        if(newLength==0)
                                printf("�ļ����ս���\n");
                        else if(newLength==-1)
                                printf("�������\n");
                        else
                                printf("�쳣����%d\n",newLength);
                        break;
                }
                
                lastLength=newLength;
                while(newLength < 5)                                            //ȷ�����ձ�ͷ����
                {
                        newLength=lastLength+recv(sock_connect, receiveData+lastLength, DATA_MAX_LENGTH-lastLength, 0);
                        if(newLength == lastLength || newLength > DATA_MAX_LENGTH)
                        {
                                flag=1;
                                break;
                        }
                        lastLength=newLength;
                }
                if(0!=receiveData[1]*256+receiveData[2])
                {
                        while(newLength != (receiveData[3]*256+receiveData[4]+5))       //ȷ��������һ֡����
                        {
                                newLength=lastLength+recv(sock_connect, receiveData+lastLength, DATA_MAX_LENGTH-lastLength, 0);
                                if(newLength == lastLength || newLength > DATA_MAX_LENGTH)
                                {
                                        flag=1;
                                        break;
                                }
                                lastLength=newLength;
                        }
                }
                if(flag==1)             //�жϽ����Ƿ�����                  
                        break;
                
                
                if(receiveData[0]!=1)
                {
                        buf[0]=0x03;                    //PLC�������
                        send(sock_connect,buf, 1, 0);      //������Ϣ��������
                        printf("PLC�������!\n");
                        break;
                }
                else if((receiveData[1]*256+receiveData[2])==0)
                {                       
                        writeAddress=ADDR_FLASH_SECTOR_5;
                        STMFLASH_EraseSector(writeAddress);
                        g_fileSize=receiveData[3]*256+receiveData[4];
                        buf[0]=0x00;                    //PLC�ɿ�ʼ�����ļ�
                        send(sock_connect,buf, 1, 0);      //������Ϣ��������
                        printf("��ʽ��ʼ�����ļ�!\n");
                }
                else if((receiveData[3]*256+receiveData[4])!=newLength-5)
                {
                        buf[0]=0x04;                    //����֡���ȴ���
                        send(sock_connect,buf, 1, 0);      //������Ϣ��������
                        printf("����֡���ȴ���!\n");
                        break;
                }
                else if((receiveData[1]*256+receiveData[2])==188&&((newLength-5)>344))
                {
                        buf[0]=0x05;                    //�������洢�ڴ�
                        send(sock_connect,buf, 1, 0);      //������Ϣ��������
                        printf("PLC�ڴ�����!\n");
                        break;
                }
                else
                {
                        writeAddress=ADDR_FLASH_SECTOR_5+((receiveData[1]*256+receiveData[2])-1)*1400;
                        STMFLASH_WriteDate(writeAddress,receiveData+5,newLength-5);
                        buf[0]=0x01;                    //�ļ����ճɹ�
                        send(sock_connect,buf, 1, 0);      //������Ϣ��������
                }
                if(0!=receiveData[1]*256+receiveData[2])
                        printf("���ؽ���: %.4f %%\n",((float)(receiveData[1]*256+receiveData[2])/(float)g_fileSize)*100);
        }
        close(sock_connect);         //һ��ֻ����һ������ 
        printf("���ӶϿ�!\n");
        OSTaskDel(OS_PRIO_SELF);
}

//�����ļ�·������������
void Send_File_Name_task(void *pdata)
{
        int length=strlen(g_fileName);
        printf("%d\n",length);
        if(length==send(sock_connect,g_fileName, length, 0))      //�����ļ����ָ��ͻ���
        {
                printf("�����ļ�·�������ֳɹ���\n");
        }
        else
        {
                printf("�����ļ�·��������ʧ��\n");
        }
        OSTaskDel(OS_PRIO_SELF);
}

//�����ļ�·�������ֲ����������ļ�·������������
void Send_File_Name(char* fileName)
{
        OS_CPU_SR cpu_sr=0;
        int length=strlen(fileName);
        printf("%d\n",length);
        memset(g_fileName, 0, 100);     //����ļ����ֻ�����
        strncpy(g_fileName,fileName,length);
        OS_ENTER_CRITICAL();            //�����ٽ���(�ر��ж�)
	OSTaskCreate(Send_File_Name_task,(void*)0,(OS_STK*)&SEND_FILENAME_TASK_STK[SEND_FILENAME_STK_SIZE-1],SEND_FILENAME_TASK_PRIO);
        OS_EXIT_CRITICAL();             //�˳��ٽ���(���ж�)
}

//��ͣ����
void Pause_Download(void)
{
        OSTaskSuspend(REV_SERVER_TASK_PRIO);
        printf("��ͣ����!\n");
}

//��������
void Continue_Download(void)
{ 
        OSTaskResume(REV_SERVER_TASK_PRIO);
        printf("��������!\n");
}

//ֹͣ����
void Stop_Download(void)
{
        OS_CPU_SR cpu_sr=0;
        OSTaskResume(REV_SERVER_TASK_PRIO);
        OS_ENTER_CRITICAL();            //�����ٽ���(�ر��ж�)
	OSTaskCreate(Stop_Download_task,(void*)0,(OS_STK*)&STOP_DOWNLOAD_TASK_STK[STOP_DOWNLOAD_STK_SIZE-1],STOP_DOWNLOAD_TASK_PRIO);
        OS_EXIT_CRITICAL();             //�˳��ٽ���(���ж�)
}

//ֹͣ��������
void Stop_Download_task(void *pdata)
{
        char buffer[1];
        buffer[0]=0x06;               //ֹͣ����
        send(sock_connect,buffer, 1, 0);   //������Ϣ��������
        printf("ֹͣ����\n");
        OSTaskDel(OS_PRIO_SELF);
}
#include "main.h"
#include "LCD_ZK.H"
#include "string.H"
#include "ADXL345.h"
#include "DS18B20.h"
#include "FLSH.h"




RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
//-----------------------------------------------------------------------------------------------> Data Definition
//----->  Uart1  And  Uart2
//���涨�����  GSM ��  ---->���������GSM�õ��Ǵ��� 1 
unsigned char Uart1_Buff[1024];  
unsigned char Uart1_Count=0;    
unsigned char AT_Count=0;
unsigned  int AT_Time =0;
unsigned char Link_Error=0;
unsigned char Send_Error=0;
unsigned char System_ID[]="20190124";	          //������д�豸ID,��λ�������Ȳ��ɱ�
unsigned char Data_Reverse=0;
unsigned int Uart1_ERROR=0;      //Uart_ERROR


uint8_t aRxBuffer[3];            //Uart
unsigned char Uart2_Buff[1024];  //Uart_Buff
unsigned int Uart2_Count=0;      //Uart_Count
unsigned int Uart2_ERROR=0;      //Uart_ERROR

//----->  Timer
unsigned int Time4_ms=0;

//----->  GPS
uint8_t GLL_flag=0;	
uint8_t GLL[1024];
unsigned char GPS_shi=0,GPS_fen=0,GPS_miao=0;	  //���ǻ�ȡ��ʱ��,UTC ʱ��
unsigned long GPS_jingdu=0,GPS_weidu=0;
uint16_t  GPS_Time=0;
uint8_t Close_GPS_Send=0;

uint8_t GPS_Config_Time=0;
uint8_t GPS_Config_Count=1;


// System 
uint16_t Num=0;  //���Գ�����

uint8_t GPS_Time_flag=1;  //��GPS��ȡ����ʱ��֮�󣬾ͻ����GPSʱ�����һ�ε�ǰʱ��
 
//----> RTC
RTC_TimeTypeDef GetTime;
//-----> Time
unsigned char Get_Time_flag=0;
unsigned char Hour=0;
unsigned char Min=0;
unsigned char Sec=0;
unsigned char nian=0;
unsigned char yue=0;
unsigned char ri=0;
unsigned char Set_Time=0;
RTC_TimeTypeDef sTime;//��ǰ������
RTC_DateTypeDef DateToUpdate;
 
//-----> ADXL345
uint8_t BUF[8];
int     ADXL345_y=0;
uint8_t Angle_flag=0;


//-----> Button  And  Beep
uint16_t Police_WaitTime=0;
uint8_t  Police_flag=0;

#define Key HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7)
uint8_t Key_flag=1;


#define Key2 HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_6)
uint8_t Key2_flag=1;





#define Beep_GPIO       GPIO_PIN_0
#define Beep_GPIO_Group GPIOB
#define Beep_0 HAL_GPIO_WritePin(Beep_GPIO_Group,Beep_GPIO,GPIO_PIN_RESET)
#define Beep_1 HAL_GPIO_WritePin(Beep_GPIO_Group,Beep_GPIO,GPIO_PIN_SET)
unsigned char Beep1=0;
unsigned char Beep_Count=0;

//---------------> DS18B20 Definition
unsigned char Read_DS18B20=1;
int DS18B20_Temp=0;

// Heart
uint8_t Heart_flag=0;	
uint8_t Heart_Conut_max=0;	
unsigned int Heart_Count=0;	
unsigned long Heart_xinlv=0;	
unsigned int xinlv=0;	
uint8_t XinLv_Count=0;
uint8_t XinLv_Buff1=0;
uint8_t XinLv_Buff2=0;
uint8_t XinLv_Buff3=0;
uint8_t XinLv_Wucha=8;

uint8_t Star=0;
uint8_t memory_GPS=0;


//���涨��һЩ����GSM�ı���

unsigned char Quest_flag=1;
unsigned char GSM_int=1;
unsigned char GSM_Time_OFF=1;
unsigned int GSM_Send_Time=0;

unsigned char GSM_Num=0;
unsigned char GSM_Send=0;
unsigned char GSM_Send_Num=0;
unsigned char Phone[11]="17839363707";
unsigned char Rec0_Error=0;





//-----------> Memory
uint8_t memory_flag=0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//-----------------------------------------------------------------------------------------------> User Code

//�����Ǳ�д�ĳ����ļ�
unsigned char table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //���������ݱ�
unsigned char Conver_week(unsigned char year,unsigned char month,unsigned char day)
{//c=0 Ϊ21����,c=1 Ϊ19���� ����������ݾ�ΪBCD����
    unsigned char p1,p2,week;
    year+=0x64;  //���Ϊ21����,�������100
    p1=year/0x4;  //����������ֻ��1900��֮���
    p2=year+p1;
    p2=p2%0x7;  //Ϊ��ʡ��Դ,�Ƚ���һ��ȡ��,����������0xff,����ʹ����������
    p2=p2+day+table_week[month-1];
    if (year%0x4==0&&month<3)p2-=1;
    week=p2%0x7;
	return week;
}

void Get_Time()
{

	if(Set_Time!=0)  //����״̬
	{
 
								if (HAL_RTC_SetTime(&hrtc, &GetTime, RTC_FORMAT_BIN) != HAL_OK)
								{
								    Error_Handler();
								} 
								
								if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK)
								{
								    Error_Handler();
								}
								/* д��һ����ֵ��0x32F2��RTC�������ݼĴ���1 */		
								HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x32F2);//���д��һ������Ŀ�ľ��Ǽ���Ƿ����
								HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, DateToUpdate.Year);   //ʹ�ö�����д��Ĵ�����Ӧ���Զ����ƴ���
								HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, DateToUpdate.Month);
								HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, DateToUpdate.Date);  
								HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, DateToUpdate.Month);		
			 
     Set_Time=0;
	}
	else   //������״̬
	{
			HAL_RTC_GetTime(&hrtc, &GetTime, RTC_FORMAT_BIN);           //��ȡʱ��
			HAL_RTC_GetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN);      //��ȡ������
		  if(GetTime.Seconds==0) 
			{
				HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, DateToUpdate.Year);   //ʹ�ö�����д��Ĵ�����Ӧ���Զ����ƴ���
				HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, DateToUpdate.Month);
				HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, DateToUpdate.Date);  
				HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, DateToUpdate.Month);
			}
  }
}	


void GLL_Dispose()	 //�ɻ�ȡ��γ�ȣ�UTCʱ��
 {  
 

	//�������ݣ�  $GNGLL,3447.6722,N,11329.8948,E,071134.000,A,A*47  \r\n
  //            $BDGLL,3906.1703,N,11720.8518,E,144745.000,A,A*47	 	
  //            $GPGLL,2236.91284,N,11403.24705,E,060826.00,A,D*66 \r\n
	
  //���о�γ�����ݶ��Ǿ�ȷ��С�������λ���ݵ�
	//����1���ݽ������   GPS_jingdu= 133482460	    GPS_weidu=34794536
	//UTCʱ�䣬��˵��������ͳһʱ�䲻�����κ�ʱ��������ʱ���UTC����8��Сʱ,��ʵGLLֱ���ڴ��ڴ�����򵥣�������Ϊ��ͳһ������Ҳ����д��
  unsigned char x=0;
	unsigned long y=0,z=0;
	 if(GLL_flag==1) 
	 {
				  GLL_flag=0;
      		y=0;
					x=0;
						do
					 {
						 x++;
					 }
					 while((GLL[x]!='\r'&&GLL[x+1]!='\n')&&x<80);  //��ȡ���ݳ���
					 
//����ǻ��˴���΢�����GPS��ֻ��Ҫ���ж� GP ����Ĵ�������Ƴ�һ���滻��GN���У��Ǹ�ģ��������������ݵ�ʱ���ʽ��GP��һ��
					 
					 if(x>=43&&GLL[x-5]==','&&GLL[x-6]=='A') //���������A��˵����λ��Ч�������V��������Ч
						{	
							 if((GLL[1]=='G'&&GLL[2]=='N')||(GLL[1]=='B'&&GLL[2]=='D'))
							{
									 y=(GLL[19]-0x30)*100000000+(GLL[20]-0x30)*10000000+(GLL[21]-0x30)*1000000;
									 z=((GLL[22]-0x30)*10+(GLL[23]-0x30))/0.00006+(((GLL[25]-0x30)*1000+(GLL[26]-0x30)*100+(GLL[27]-0x30)*10+(GLL[28]-0x30))/0.6);
									
									 GPS_jingdu=y+z;
							
									 //����γ��
									 y=(GLL[7]-0x30)*10000000+(GLL[8]-0x30)*1000000;
									 z=((GLL[9]-0x30)*10+(GLL[10]-0x30))/0.00006+(((GLL[12]-0x30)*1000+(GLL[13]-0x30)*100+(GLL[14]-0x30)*10+(GLL[15]-0x30))/0.6);
									 GPS_weidu=y+z;

									 GPS_shi= (GLL[32]-0x30)*10+(GLL[33]-0x30)+8;
									 GPS_fen= (GLL[34]-0x30)*10+(GLL[35]-0x30) ;
									 GPS_miao=(GLL[36]-0x30)*10+(GLL[37]-0x30) ;
							}
						 else if(GLL[1]=='G'&&GLL[2]=='P')
							 {
										 y=(GLL[20]-0x30)*100000000+(GLL[21]-0x30)*10000000+(GLL[22]-0x30)*1000000;
										 z=((GLL[23]-0x30)*10+(GLL[24]-0x30))/0.00006+(((GLL[26]-0x30)*1000+(GLL[27]-0x30)*100+(GLL[28]-0x30)*10+(GLL[29]-0x30))/0.6);
										
										 GPS_jingdu=y+z;
								
										 //����γ��
										 y=(GLL[7]-0x30)*10000000+(GLL[8]-0x30)*1000000;
										 z=((GLL[9]-0x30)*10+(GLL[10]-0x30))/0.00006+(((GLL[12]-0x30)*1000+(GLL[13]-0x30)*100+(GLL[14]-0x30)*10+(GLL[15]-0x30))/0.6);
										 GPS_weidu=y+z;

										 GPS_shi= (GLL[34]-0x30)*10+(GLL[35]-0x30)+8;
										 GPS_fen= (GLL[36]-0x30)*10+(GLL[37]-0x30) ;
										 GPS_miao=(GLL[38]-0x30)*10+(GLL[39]-0x30) ;
							 }
	//					 	 printf("���ȣ�%ld γ�ȣ�%ld,Time-> %d:%d:%d\r\n",GPS_jingdu,GPS_weidu,GPS_shi,GPS_fen,GPS_miao);
							 GPS_Time=10000;
							 memory_GPS=1;
						}
//						else 
//							printf("��λʧ�ܣ�GPS�ź���...\r\n");
	 }
 }
 
void display_buff(unsigned char X,unsigned char Y,unsigned char dat)  //��ʾһ������
 {
  switch(dat)
   {
     case 0: display_GB2312_string(X,Y,"0"); break;
     case 1: display_GB2312_string(X,Y,"1"); break;
     case 2: display_GB2312_string(X,Y,"2"); break;
     case 3: display_GB2312_string(X,Y,"3"); break;
     case 4: display_GB2312_string(X,Y,"4"); break;
     case 5: display_GB2312_string(X,Y,"5"); break;
     case 6: display_GB2312_string(X,Y,"6"); break;
     case 7: display_GB2312_string(X,Y,"7"); break;
     case 8: display_GB2312_string(X,Y,"8"); break;
     case 9: display_GB2312_string(X,Y,"9"); break;
   }
 }
 


 void Key_Dispose()
{
	if(Key==0) 
	{
	   if(Key_flag==1) 
		 {
		    Key_flag=0;
			  Beep_Count=3;
			  Star=1;
			  XinLv_Count=0;
		 }
	}else Key_flag=1;
	
	if(Key2==0) 
	{
	   if(Key2_flag==1) 
		 {
		    Key2_flag=0;
			  Beep_Count=1;
			  
		 }Police_WaitTime = 2005; 
	}else Key2_flag=1;
	
	
	
	
}
 
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)           //��ʱ���ص�����
{
   if(htim==&htim4)  //->  1ms�ж�
      {
			    if(GSM_Send_Time!=0) GSM_Send_Time--;
          Heart_Count++;  //������ÿ��1ms��һ
				  if(Star==1)     //������־λ
					{
								 if(Heart_Conut_max==1)   //ֻҪ�ñ���Ϊ1 
								 {
									 Heart_xinlv=Heart_Count;
								 }else  //����  
									{
										 if(Heart_xinlv<1000&&Heart_xinlv>100)  //���˸ߵ�����  ���� = ��ʱ�� / ����ʱ��   
										 {
											 if((60000/Heart_xinlv)<180)  xinlv=(60000/Heart_xinlv);
											 if(xinlv>100) xinlv=xinlv-20;
											 else if(xinlv>90) xinlv=xinlv-10;
											 else if(xinlv<60) xinlv=xinlv+8;
											 switch(XinLv_Count) 
											 {
												 case 0: XinLv_Buff1 = xinlv;  XinLv_Count=1; break;
												 case 1: XinLv_Buff2 = xinlv;  XinLv_Count=2; break;
												 case 2: XinLv_Buff3 = xinlv;  XinLv_Count=3; break;  //��¼��������
												 case 3: 
																 if((xinlv<XinLv_Buff1+XinLv_Wucha)&&(xinlv>XinLv_Buff1-XinLv_Wucha))
																 { 
																		if((xinlv<XinLv_Buff2+XinLv_Wucha)&&(xinlv>XinLv_Buff2-XinLv_Wucha))
																		{
																			 if((xinlv<XinLv_Buff3+XinLv_Wucha)&&(xinlv>XinLv_Buff3-XinLv_Wucha)) //�ж�
																			 {
																				 Star=0;
																				 Beep_Count=5;
																				 XinLv_Count=0;
																			 }else XinLv_Count=0;	
																		}else XinLv_Count=0;														
																 }else XinLv_Count=0;
												 break;
												 
											 }
											 
											 Heart_xinlv=0;
										 }
									}
					}	


  				Time4_ms++;
				  if(Time4_ms%10==0) 
					{  Key_Dispose();
					   if(AT_Time!=0) AT_Time--;
						 if(GPS_Time!=0) GPS_Time--;
						
						 if(Angle_flag==0)
						 {
							  if((Police_WaitTime>=1000)&&(Police_WaitTime<2000)) 
								{
									 if(Police_flag==0) 
									 {
									    Police_flag=1;
										  GSM_Send_Num=1;  GSM_Send=1;    
									 }
								}else  
								{
								   if(Police_WaitTime<1500) Police_WaitTime++;
									 Police_flag=0;
								}
						 }
             else 
						 {
						    Police_WaitTime=0;
							  
							 if(Police_flag==1) 
							 {
							   if(GSM_Send==0)
								 {
								    GSM_Send=1;
									  Police_flag=0;
									  GSM_Send_Num=2; 
								 }
							 }
						 }	


					}
					 if(Time4_ms%50==0) 
					 {
							if(Police_flag==1||Beep_Count!=0) 
							{
								 if(Beep_Count!=0) Beep_Count--;
								 Beep_GPIO_Group->ODR^=Beep_GPIO;
							}
							else Beep_0;
					 }
					 
				  if(Time4_ms>=1000) 
					{  
					   Time4_ms=0;
						 if(GPS_Config_Time!=0) GPS_Config_Time--;
						 Read_DS18B20=1;
					}
      }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)               //���ڻص�����
{
	uint8_t ret1 = HAL_OK;
	uint8_t ret2 = HAL_OK;

  if(huart==&huart1)
	 {
	    Uart1_Buff[Uart1_Count] = aRxBuffer[0];
		  Uart1_Count=(Uart1_Count+1)%1024;
		  Uart1_Buff[Uart1_Count]=0;
		 
		  if(Quest_flag==1) //˵����ʱ�ڳ�ʼ��
			{
			   if(GSM_Num==0||GSM_Num==1||GSM_Num==2||GSM_Num==3) //���ﶼ�Ƿ��ص�  OK
				 {
				   if(Uart1_Count>=4&&Uart1_Buff[Uart1_Count-1]=='\n'&&Uart1_Buff[Uart1_Count-2]=='\r'&&Uart1_Buff[Uart1_Count-3]=='K'&&Uart1_Buff[Uart1_Count-4]=='O')
					 {
					   GSM_Num++;
						 GSM_Send_Time=10;
					 }
				 }
				 else if(GSM_Num==4) //+COPS: 0,0,"CHINA MOBILE"
				 {
				   if(Uart1_Count>=25&&Uart1_Buff[Uart1_Count-1]=='\n'&&Uart1_Buff[Uart1_Count-2]=='\r'&&Uart1_Buff[Uart1_Count-3]=='"'&&Uart1_Buff[Uart1_Count-4]=='E'&&Uart1_Buff[Uart1_Count-11]=='A')
					 {
					   GSM_Num++;
						 GSM_Send_Time=10;
					 }
				 }
			}//��������ڳ�ʼ����������Ƿ��Ͷ���Ϣ��
			else 
			{
			   if(GSM_Num==0||GSM_Num==2||GSM_Num==3) //���ﶼ�Ƿ��ص�  OK
				 {
				   if(Uart1_Count>=4&&Uart1_Buff[Uart1_Count-1]=='\n'&&Uart1_Buff[Uart1_Count-2]=='\r'&&Uart1_Buff[Uart1_Count-3]=='K'&&Uart1_Buff[Uart1_Count-4]=='O')
					 {
					   GSM_Num++;
						 GSM_Send_Time=10;
					 }
				   if(Uart1_Count>=7&&Uart1_Buff[Uart1_Count-1]=='\n'&&Uart1_Buff[Uart1_Count-2]=='\r'&&Uart1_Buff[Uart1_Count-3]=='R'&&Uart1_Buff[Uart1_Count-7]=='E')
					 { //ERROR
					   if(GSM_Num==3) 
						 {
						    GSM_Num=0;
							 GSM_Send_Time=10;
						 }
					 }			 
				 }
				 else if(GSM_Num==1) //>  
				 {
				   if(Uart1_Count>=1&&Uart1_Buff[Uart1_Count-1]=='>')
					 {
					   GSM_Num++;
						 GSM_Send_Time=10;
					 }
				 }	 
			}

		  if(Uart1_Count>=2&&Uart1_Buff[Uart1_Count-2]=='\r'&&Uart1_Buff[Uart1_Count-1]=='\n') //���յ��س���
			{//PHONE SMS:13343851798\r\n
				if(Uart1_Count>=23&&Uart1_Buff[Uart1_Count-23]=='P'&&Uart1_Buff[Uart1_Count-22]=='H'&&Uart1_Buff[Uart1_Count-17]=='S'&&Uart1_Buff[Uart1_Count-14]==':')
				{
				    Phone[0] =Uart1_Buff[Uart1_Count-13];
					  Phone[1] =Uart1_Buff[Uart1_Count-12];
					  Phone[2] =Uart1_Buff[Uart1_Count-11];
					  Phone[3] =Uart1_Buff[Uart1_Count-10];
					  Phone[4] =Uart1_Buff[Uart1_Count-9];
					  Phone[5] =Uart1_Buff[Uart1_Count-8];
					  Phone[6] =Uart1_Buff[Uart1_Count-7];
					  Phone[7] =Uart1_Buff[Uart1_Count-6];
					  Phone[8] =Uart1_Buff[Uart1_Count-5];
					  Phone[9] =Uart1_Buff[Uart1_Count-4];
					  Phone[10]=Uart1_Buff[Uart1_Count-3];
					  GSM_Send =1;
					  GSM_Send_Num =0;
						memory_flag=1;
				}  
				
				
				Uart1_Count=0;
			}
			Uart1_ERROR=0;
			do  
			{  
					ret1 = HAL_UART_Receive_IT(&huart1,(uint8_t *)&aRxBuffer[0],1); 
          Uart1_ERROR++;  				
			}while(ret1 != HAL_OK&&Uart1_ERROR<3);
	 }
	 
	
  if(huart==&huart2)
	 {
		 
		  Uart2_Buff[Uart2_Count] = aRxBuffer[1];
		  Uart2_Count=(Uart2_Count+1)%1024;
		  Uart2_Buff[Uart2_Count]='\0';
		 
		  if(Uart2_Count>=2&&Uart2_Buff[Uart2_Count-2]=='\r'&&Uart2_Buff[Uart2_Count-1]=='\n') //���յ��س���
			{ 
				 if(Uart2_Count>=46&&Uart2_Buff[0]=='$'&&Uart2_Buff[3]=='G'&&Uart2_Buff[4]=='L'&&Uart2_Buff[5]=='L') 
				 {
						strcpy(GLL,Uart2_Buff);
						GLL_flag=1; 
				 }
				 else if(Uart2_Count>=6&&Uart2_Buff[0]=='$'&&Uart2_Buff[3]=='G'&&Uart2_Buff[4]=='G'&&Uart2_Buff[5]=='A') //��һ����ר�żӵģ��ر���������Ҫ����Ϣ���
				 {
		 		   Close_GPS_Send=1;
				 }
				 Uart2_Count=0;
			}

			Uart2_ERROR=0;  
			do  
			{  
					ret2 = HAL_UART_Receive_IT(&huart2,(uint8_t *)&aRxBuffer[1],1);  
    			Uart2_ERROR++;   	
			}while(ret2 != HAL_OK&&Uart2_ERROR<3);
	 }
	 
	 
}



void Uart_ErrorCle()                                                  //���ڴ���ص�
{
		if(Uart1_ERROR>=3)  HAL_UART_Receive_IT(&huart1,(uint8_t *)&aRxBuffer[0],1); 
		if(Uart2_ERROR>=3)  HAL_UART_Receive_IT(&huart2,(uint8_t *)&aRxBuffer[1],1);
}



// ģ�������飬�Ǵ�51����ֲ�����ģ��������£��������ﶨ��һЩ��������32��ʹ��
void Uart1Data(unsigned char dat)	 //���� 1 ������һ���ֽ�
{
	 HAL_UART_Transmit(&huart1,&dat,1,0xffff);
}
void Uart1Databyte(unsigned char *byte) //���� 1 ������һ��
{
	while(*byte != '\0')
	{
		Uart1Data(*byte++);	
	}
}



void GSM_Init()
{
   unsigned char G_Tab[20];
   if(Quest_flag==1) //��ʼ��ѯGSM״̬//��ʱ��û�г�ʼ����ɣ����ȣ��ȳ�ʼ�� ����AT���ܹ���Ӧ�ϣ��ػ��ԣ����ö��Ŵ���ʽ
	 {

						switch(GSM_Num)
						{
							case 0: HAL_UART_Transmit(&huart1,(unsigned char *)"AT\r\n",4,100);                  break; //AT
							case 1: HAL_UART_Transmit(&huart1,(unsigned char *)"ATE1\r\n",6,100);                break;//�ػ���
							case 2: HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CNMI=3,2,2,0,1\r\n",19,100);  break;//����
							case 3: HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CMGF=1\r\n",11,100);          break; //����
							case 4: HAL_UART_Transmit(&huart1,(unsigned char *)"AT+COPS?\r\n",10,100);            break; //��ѯ����
							case 5: Quest_flag=0;  GSM_Num=0; GSM_Send=0;   GSM_Send_Num=0;                               break; //��ʼ�����׽���
					 }
					GSM_Send_Time=1000;
	 }
	 else
	 {
		 
		    GSM_Send_Time=2000;
			 if(GSM_Send==1)  //���Ͷ��ű�־
				{
					if(GSM_Send_Num==0)  //Num �Ƕ��ű�ţ����Ƿ����������ţ�����Ƿ���Set End ��
					 {
							switch(GSM_Num)
							{
								case 0: HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CMGF=1\r\n",11,100);              break; 
								case 1: HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CMGS=\"",9,100);  HAL_Delay (1);                                        
								        HAL_UART_Transmit(&huart1,(unsigned char *)Phone,11,100);         HAL_Delay (1);
									      HAL_UART_Transmit(&huart1,(unsigned char *)"\"\r\n",3,100);                      break; //AT+CMGS="13343851798"
								case 2: HAL_UART_Transmit(&huart1,(unsigned char *)"Set End",7,100);  GSM_Num=3;                break; 
								case 3: G_Tab[0]=0x1a; 
								        HAL_UART_Transmit(&huart1,(unsigned char *)G_Tab,1,100);                         break; 
								case 4: GSM_Num=0;   GSM_Send=0;                         break; 
							}
					 }
					else if(GSM_Send_Num==1)//����Ǳ���Ƶ����Ķ�������
					 {
							switch(GSM_Num)
							{
								case 0: HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CMGF=0\r\n",11,100);      break;  
								case 1: HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CMGS=117\r\n",13,100);     break;   
								case 2: HAL_UART_Transmit(&huart1,(unsigned char *)"0011000D9168",12,100); HAL_Delay (1);
                        G_Tab[0]=Phone[1];
								        G_Tab[1]=Phone[0];
								        G_Tab[2]=Phone[3];
								        G_Tab[3]=Phone[2];
								        G_Tab[4]=Phone[5];
								        G_Tab[5]=Phone[4];
								        G_Tab[6]=Phone[7];
								        G_Tab[7]=Phone[6];
								        G_Tab[8]=Phone[9];
								        G_Tab[9]=Phone[8];
								        G_Tab[10]='F';
									      G_Tab[11]=Phone[10];
								        HAL_UART_Transmit(&huart1,(unsigned char *)G_Tab,12,100); HAL_Delay (1);
								        HAL_UART_Transmit(&huart1,(unsigned char *)"0008A966",8,100); HAL_Delay (1);//ĩβ��λ���� ���͵Ķ��ų���
								
												Uart1Databyte("80014EBA645450124E863002"); //---> 12
								 
												if(GPS_Time==0)  Uart1Databyte("672A83B753D60047005000534FE153F7002C67008FD14E006B215B9A4F4D003A002000207EAC5EA6");//δ��ȡGPS�ź�,���һ�ζ�λ:
												else 		         Uart1Databyte("5DF283B753D60047005000534FE153F7002C76EE524D5B9A4F4D57305740003A002000207EAC5EA6");//�ѻ�ȡGPS�ź�,Ŀǰ��λ��ַ:	 
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_weidu/10000000%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_weidu/1000000%10);			
												Uart1Databyte("002E003");
												Uart1Data(0x30+GPS_weidu/100000%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_weidu/10000%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_weidu/1000%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_weidu/100%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_weidu/10%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_weidu%10);
												Uart1Databyte("0020002000207ECF5EA6003A");
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_jingdu/100000000%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_jingdu/10000000%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_jingdu/1000000%10);
												Uart1Databyte("002E003");
												Uart1Data(0x30+GPS_jingdu/100000%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_jingdu/10000%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_jingdu/1000%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_jingdu/100%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_jingdu/10%10);
												Uart1Databyte("003");
												Uart1Data(0x30+GPS_jingdu%10);

								
                        GSM_Num=3; 
								break;  
								case 3: G_Tab[0]=0x1a;  
								        HAL_UART_Transmit(&huart1,(unsigned char *)G_Tab,1,100);                 break;   
								case 4:    
												HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CMGF=1\r\n",11,100); 
								        HAL_Delay(1500);
								        GSM_Num=0; GSM_Send=0;  
								break; 		
							}
					 }
				  else if(GSM_Send_Num==2)//����Ǳ���Ƶ����Ķ�������
					 {
							switch(GSM_Num)
							{
								case 0: HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CMGF=0\r\n",11,100);      break;  
								
								case 1: HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CMGS=39\r\n",12,100);     break;   

								case 2: HAL_UART_Transmit(&huart1,(unsigned char *)"0011000D9168",12,100); HAL_Delay (1);
                        G_Tab[0]=Phone[1];
								        G_Tab[1]=Phone[0];
								        G_Tab[2]=Phone[3];
								        G_Tab[3]=Phone[2];
								        G_Tab[4]=Phone[5];
								        G_Tab[5]=Phone[4];
								        G_Tab[6]=Phone[7];
								        G_Tab[7]=Phone[6];
								        G_Tab[8]=Phone[9];
								        G_Tab[9]=Phone[8];
								        G_Tab[10]='F';
									      G_Tab[11]=Phone[10];
								        HAL_UART_Transmit(&huart1,(unsigned char *)G_Tab,12,100); HAL_Delay (1);
								        HAL_UART_Transmit(&huart1,(unsigned char *)"0008A918",8,100); HAL_Delay (1);//ĩβ��λ���� ���͵Ķ��ų���
								//
                        HAL_UART_Transmit(&huart1,(unsigned char *)"80014eba5df27ecf813179bb8dcc501272b6600130023002",48,100);
                        GSM_Num=3;                                  
								break;  
								
								case 3: G_Tab[0]=0x1a;  
								        HAL_UART_Transmit(&huart1,(unsigned char *)G_Tab,1,100);                 break;   
								case 4: 
									      
												HAL_UART_Transmit(&huart1,(unsigned char *)"AT+CMGF=1\r\n",11,100); 
								        HAL_Delay(1500);
								        GSM_Num=0;  GSM_Send=0;     
								break; 
												
							}
					 }
				}	else GSM_Num=0;
	 }
}

void Display() 
{
//------> ��һ�� 
	
	    display_GB2312_string(0,0,"�¶�:");
			display_buff(0,40,DS18B20_Temp/100%10);
			display_buff(0,48,DS18B20_Temp/10%10);
	    display_GB2312_string(0,56,".");
			display_buff(0,64,DS18B20_Temp%10);
	    display_GB2312_string(0,72,"�� ");
	
	    if(Angle_flag==0)  display_GB2312_string(0,96,"�㵹");
			else 							 display_GB2312_string(0,96,"����");

    	display_GB2312_string(2,0,"��������:");
			display_buff(2,72,xinlv/100%10);
			display_buff(2,80,xinlv/10%10);
			display_buff(2,88,xinlv%10);
			display_GB2312_string(2,96,"/min");
//	  display_GB2312_string(0,0,"20");
//		display_buff(0,16,DateToUpdate.Year/10%10);
//		display_buff(0,24,DateToUpdate.Year%10);
//		display_GB2312_string(0,32,"/");	
//		display_buff(0,40,DateToUpdate.Month/10%10);
//		display_buff(0,48,DateToUpdate.Month%10);
//		display_GB2312_string(0,56,"/");	
//		display_buff(0,64,DateToUpdate.Date/10%10);
//		display_buff(0,72,DateToUpdate.Date%10);
//		display_GB2312_string(0,80,"  ");	
//    switch(Conver_week(DateToUpdate.Year,DateToUpdate.Month,DateToUpdate.Date))
//		 {
//				case 0: display_GB2312_string(0,96,"Sun");    break;
//				case 1: display_GB2312_string(0,96,"Mon");    break;
//				case 2: display_GB2312_string(0,96,"Tue");    break;
//				case 3: display_GB2312_string(0,96,"Wed");    break;
//				case 4: display_GB2312_string(0,96,"Thu");    break;
//				case 5: display_GB2312_string(0,96,"Fri");    break;
//				case 6: display_GB2312_string(0,96,"Sat");    break;	 	 
//		 }
//		
////----> �ڶ���	
//		display_buff(2,0,GetTime.Hours/10%10);
//		display_buff(2,8,GetTime.Hours%10);
//		display_GB2312_string(2,16,":");	
//		display_buff(2,24,GetTime.Minutes/10%10);
//		display_buff(2,32,GetTime.Minutes%10);
//		display_GB2312_string(2,40,":");	
//		display_buff(2,48,GetTime.Seconds/10%10);
//		display_buff(2,56,GetTime.Seconds%10);
//		display_GB2312_string(2,64,"  ");	 
//		if(GPS_Time==0) display_GB2312_string(2,80,"δ��λ");	
//		else 						display_GB2312_string(2,80,"�Ѷ�λ");	
			
 
//------> ������ And ������	 	
		display_GB2312_string(4,0,"����:");	
		display_buff(4,40,GPS_jingdu/100000000%10);
		display_buff(4,48,GPS_jingdu/10000000%10);
		display_buff(4,56,GPS_jingdu/1000000%10);
		display_GB2312_string(4,64,".");	
		display_buff(4,72,GPS_jingdu/100000%10);
		display_buff(4,80,GPS_jingdu/10000%10);
		display_buff(4,88,GPS_jingdu/1000%10);
		display_buff(4,96,GPS_jingdu/100%10);
		display_buff(4,104,GPS_jingdu/10%10);
		display_buff(4,112,GPS_jingdu%10);
 
		display_GB2312_string(6,0,"γ��: ");		 
//	display_buff(6,40,GPS_weidu/100000000%10);
		display_buff(6,48,GPS_weidu/10000000%10);
		display_buff(6,56,GPS_weidu/1000000%10);
		display_GB2312_string(6,64,".");	
		display_buff(6,72,GPS_weidu/100000%10);
		display_buff(6,80,GPS_weidu/10000%10);
		display_buff(6,88,GPS_weidu/1000%10);
		display_buff(6,96,GPS_weidu/100%10);
		display_buff(6,104,GPS_weidu/10%10);
		display_buff(6,112,GPS_weidu%10);
 
}
 
void Angle()   //���������㣬���ŷ�Χ     800<Angle<1300 �������������鴮�ڴ�ӡһ�¿���
{
	  ADXL345_y=(BUF[3]<<8)+BUF[2];
   if(ADXL345_y>=0) 
	 {
	   ADXL345_y=ADXL345_y*3.9;
	 }
	else 
	 {
	   ADXL345_y=ADXL345_y*-1*3.9;
	 }	
	 if((550<ADXL345_y)&&(ADXL345_y<1300)) Angle_flag=1;
	 else                                  Angle_flag=0;
} 



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)  
{
//���ԭ������һ���ⲿ�жϣ����ݵ�·���ԣ��ýŻ��������������һ�µ�����
//����Ҫ��������֮���ʱ�䣬һ��������һ��������Ҳ����Ҫ��������֮���ʱ��  
	if(Heart_flag!=0) Heart_flag=0; 
	else Heart_flag=1;
	if(Heart_flag==1)    //�������ֻҪ�����жϾͻ�ȡ������ֵΪ1
	{  
		 Heart_Conut_max=1;//��һ֮������ʱ�����  Heart_Count ��һֱ���� Heart_xinlv��ֱ���ñ������㣬 Heart_xinlv���ǵõ�����������ļ��ʱ��
	   Heart_Count=0;    //����ʱ�����
	}
	else 
	{
	  Heart_Conut_max=0;  //�ɼ����������壬����д���ܻ���ֵ������Ǹտ�ʼ��������ƫ�󣬵���ֻҪӲ����⵽�������ȶ�����������֮��ͻ�û���⣬�ڶ�ʱ�����д���
	}
} 



void Memory()
{
  if(memory_flag==1) 
	{
		memory_flag=0; //08001C00
		Write_flsh_byte(0x0800F000,Phone[0]);
		Write_flsh_byte(0x08010000,Phone[1]);
		Write_flsh_byte(0x08011000,Phone[2]);
		Write_flsh_byte(0x08012000,Phone[3]);
		Write_flsh_byte(0x08013000,Phone[4]);
		Write_flsh_byte(0x08014000,Phone[5]);
		Write_flsh_byte(0x08015000,Phone[6]);
		Write_flsh_byte(0x08016000,Phone[7]);
		Write_flsh_byte(0x08017000,Phone[8]);
		Write_flsh_byte(0x08018000,Phone[9]);
		Write_flsh_byte(0x08019000,Phone[10]);
	}
	
	if(memory_GPS == 1 ) 
	{
		memory_GPS =0;
		Write_flsh_byte(0x0801A000,GPS_jingdu);
		Write_flsh_byte(0x0801B000,GPS_weidu);
	}
	
	
}
void Read_Memory()
{
		Phone[0]=readFlash(0x0800F000);
		Phone[1]=readFlash(0x08010000);
		Phone[2]=readFlash(0x08011000);
		Phone[3]=readFlash(0x08012000);
		Phone[4]=readFlash(0x08013000);
		Phone[5]=readFlash(0x08014000);
		Phone[6]=readFlash(0x08015000);
		Phone[7]=readFlash(0x08016000);
		Phone[8]=readFlash(0x08017000);
		Phone[9]=readFlash(0x08018000);
		Phone[10]=readFlash(0x08019000);
	
		GPS_jingdu=readFlash(0x0801A000);
		GPS_weidu=readFlash(0x0801B000);
}




/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM4_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
//---------------------------------------------------------> Used Init Code Star
  HAL_Delay(200);
	initial_lcd();
  clear_screen();
	Init_ADXL345();
  DS18B20_Init();
	HAL_UART_Receive_IT(&huart1,&aRxBuffer[0],1);  //���ô��ڽ��յ�һ���ֽھͲ����ж�
	HAL_UART_Receive_IT(&huart2,&aRxBuffer[1],1);  //���ô��ڽ��յ�һ���ֽھͲ����ж�
  HAL_TIM_Base_Start_IT(&htim4); 
	do
	{
	  DS18B20_Temp=DS18B20_Get_Temp();
	}while(DS18B20_Temp==850);	
//	Read_Memory();
//---------------------------------------------------------> Used Init Code End

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
//---------------------------------------------------------> Used  Code Star
			Multiple_ReadADXL345();       	//�����������ݣ��洢��BUF��
			Angle(); 
		  if(GSM_Send_Time==0)  GSM_Init();
			Memory();
			Uart_ErrorCle();  
			GLL_Dispose();    //�������������GPSģ�飬�޸��������
			Get_Time();       
			Display();        
			if(Read_DS18B20==1)
			{
				 Read_DS18B20=0;
				 DS18B20_Temp=DS18B20_Get_Temp();
			}
			if(GPS_Config_Count!=0) //���������
			{
					GPS_Config_Count=0;
					GPS_Config_Time=3; 
					HAL_UART_Transmit(&huart2,(unsigned char *)"$PCAS04,7*1E\r\n",14,0xffff);
					HAL_Delay(5);
					HAL_UART_Transmit(&huart2,(unsigned char *)"$PCAS03,0,1,0,0,0,0,0,0,0,0,0,0,0,0*03\r\n",41,0xffff);
			}
		
 
//---------------------------------------------------------> Used  Code End
	
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

//  RTC_TimeTypeDef sTime = {0};
//  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initialize RTC and set the Time and Date 
    */
   if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
	{

		//		sTime.Hours = 0x23;
		//		sTime.Minutes = 0x59;
		//		sTime.Seconds = 0x50;
		//		if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
		//		{
		//			Error_Handler();
		//		}
		DateToUpdate.WeekDay = RTC_WEEKDAY_SATURDAY;
		DateToUpdate.Month = RTC_MONTH_DECEMBER;
		DateToUpdate.Date = 0x3;
		DateToUpdate.Year = 0x17;
		if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
		{
			Error_Handler();
		}
			/* д��һ����ֵ��0x32F2��RTC�������ݼĴ���1 */		

    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x32F2);//���д��һ������Ŀ�ľ��Ǽ���Ƿ����
  }
 else
  { 
    /* ����ϵ縴λ��־λ�Ƿ�Ϊ��SET */

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST) != RESET)
    {
  //    printf("�����ϵ縴λ������\n");
    }
    /* ������Ÿ�λ��־λ�Ƿ�Ϊ��SET */
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST) != RESET)
    {
  //     printf("�����ⲿ���Ÿ�λ������\n");
      DateToUpdate.Year  = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
      DateToUpdate.Month = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
      DateToUpdate.Date  = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
      DateToUpdate.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6);
      HAL_RTC_SetDate(&hrtc,&DateToUpdate,RTC_FORMAT_BIN);        //��д��ķ�ʽ����һ�£�main��      ������д����Ƕ�����
    }    
    /* �����λԴ��־λ */
    __HAL_RCC_CLEAR_RESET_FLAGS();
	}
}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 63;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//���������PA6����������ȡ�����α���
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	
  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */



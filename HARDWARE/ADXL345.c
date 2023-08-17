
#include "ADXL345.h"


	
extern unsigned char  BUF[8]; 


void ADXL_345_ALL_Int()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  ADXL345_GPIOCLK;
 
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ADXL345_GPIO, ADXL345_SDA|ADXL345_SCL, GPIO_PIN_SET);

  /*Configure GPIO pins : PB10 PB11 */
  GPIO_InitStruct.Pin = ADXL345_SDA|ADXL345_SCL;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ADXL345_GPIO, &GPIO_InitStruct);
	
}

void ADXL345_SDA_Output()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  ADXL345_GPIOCLK;

  GPIO_InitStruct.Pin = ADXL345_SDA;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ADXL345_GPIO, &GPIO_InitStruct);
	

	
}
void ADXL345_SDA_Intput()
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  ADXL345_GPIOCLK;
  /*Configure GPIO pins : PB10 PB11 */
  GPIO_InitStruct.Pin =  ADXL345_SDA;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed =GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ADXL345_GPIO, &GPIO_InitStruct);
}


/**************************************
��ʱ5΢��(STC90C52RC@12M)
��ͬ�Ĺ�������,��Ҫ�����˺�����ע��ʱ�ӹ���ʱ��Ҫ�޸�
������1T��MCUʱ,���������ʱ����
**************************************/
void Delay5us()
{
	__nop();__nop();__nop();__nop(); 
	__nop();__nop();__nop();__nop(); 
	__nop();__nop();__nop();__nop(); 
	__nop();__nop();__nop();__nop(); 

}

/**************************************
��ʱ5����(STC90C52RC@12M)
��ͬ�Ĺ�������,��Ҫ�����˺���
������1T��MCUʱ,���������ʱ����
**************************************/
void Delay5ms()
{
   HAL_Delay(3);
}

/**************************************
��ʼ�ź�
**************************************/
void ADXL345_Start()
{
  	SDA_1;                    //����������
    SCL_1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA_0;                    //�����½���
    Delay5us();                 //��ʱ
    SCL_0;                    //����ʱ����
	
}

/**************************************
ֹͣ�ź�
**************************************/
void ADXL345_Stop()
{

    SDA_0;                    //����������
    SCL_1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SDA_1;                    //����������
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
��ڲ���:ack (0:ACK 1:NAK)
**************************************/
void ADXL345_SendACK(unsigned char ack)
{
    if(ack==1) SDA_1;
		else       SDA_0;
//	SDA = ack;                  //дӦ���ź�
    SCL_1;                    //����ʱ����
    Delay5us();                 //��ʱ
    SCL_0;                    //����ʱ����
    Delay5us();                 //��ʱ
}

/**************************************
����Ӧ���ź�
**************************************/
unsigned char ADXL345_RecvACK()
{

  	unsigned char CY_Dat=0;
   	SCL_1;                    //����ʱ����
    Delay5us();                 //��ʱ
	  if(ADXL345SDA==1) CY_Dat=1;
	  else   CY_Dat=0;
//    CY_Dat = SDA;                   //��Ӧ���ź�
	
    SCL_0;                    //����ʱ����
    Delay5us();                 //��ʱ
    ADXL345_SDA_Output();
	return CY_Dat;
	  
}

/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void ADXL345_Sendbyte(unsigned char dat)
{
    unsigned char i;
    ADXL345_SDA_Output();//SDAΪ���
    for (i=0; i<8; i++)         //8λ������
    {
			  if((dat&0x80)==0x80) SDA_1;
				else 							   SDA_0;
   			dat <<= 1;              //�Ƴ����ݵ����λ
  //    SDA = CY;               //�����ݿ�
        SCL_1;                //����ʱ����
        Delay5us();             //��ʱ
        SCL_0;                //����ʱ����
        Delay5us();             //��ʱ
    }
		 ADXL345_SDA_Intput();
     ADXL345_RecvACK();
     ADXL345_SDA_Output();
}

/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
unsigned char ADXL345_Recvbyte()
{
    unsigned char i;
    unsigned char dat = 0;
    SDA_1;                    //ʹ���ڲ�����,׼����ȡ����,
    ADXL345_SDA_Intput();
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL_1;                //����ʱ����
        Delay5us();             //��ʱ
			  if(ADXL345SDA==1) dat |= 0x01;     //������  
				else              dat |= 0x00; 
        SCL_0;                //����ʱ����
        Delay5us();             //��ʱ
    }
		ADXL345_SDA_Output();
    return dat;
		    
}

//******���ֽ�д��*******************************************

void Single_Write_ADXL345(unsigned char REG_Address,unsigned char REG_data)
{
    ADXL345_Start();                  //��ʼ�ź�
    ADXL345_Sendbyte(SlaveAddress);   //�����豸��ַ+д�ź�
    ADXL345_Sendbyte(REG_Address);    //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
    ADXL345_Sendbyte(REG_data);       //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
    ADXL345_Stop();                   //����ֹͣ�ź�
}

//********���ֽڶ�ȡ*****************************************
unsigned char Single_Read_ADXL345(unsigned char REG_Address)
{   unsigned char REG_data;
		ADXL345_Start();                          //��ʼ�ź�
		ADXL345_Sendbyte(SlaveAddress);           //�����豸��ַ+д�ź�
		ADXL345_Sendbyte(REG_Address);            //���ʹ洢��Ԫ��ַ����0��ʼ	
		ADXL345_Start();                          //��ʼ�ź�
		ADXL345_Sendbyte(SlaveAddress+1);         //�����豸��ַ+���ź�
		REG_data=ADXL345_Recvbyte();              //�����Ĵ�������
		ADXL345_SendACK(1);   
		ADXL345_Stop();                           //ֹͣ�ź�
		return REG_data; 
}
//*********************************************************
//
//��������ADXL345�ڲ����ٶ����ݣ���ַ��Χ0x32~0x37
//
//*********************************************************
void Multiple_ReadADXL345()
{   unsigned char i;
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_Sendbyte(SlaveAddress);           //�����豸��ַ+д�ź�
    ADXL345_Sendbyte(0x32);                   //���ʹ洢��Ԫ��ַ����0x32��ʼ	
    ADXL345_Start();                          //��ʼ�ź�
    ADXL345_Sendbyte(SlaveAddress+1);         //�����豸��ַ+���ź�
	 for (i=0; i<6; i++)                      //������ȡ6����ַ���ݣ��洢��BUF
    {
        BUF[i] = ADXL345_Recvbyte();          //BUF[0]�洢0x32��ַ�е�����
        if (i == 5)
        {
           ADXL345_SendACK(1);                //���һ��������Ҫ��NOACK
        }
        else
        {
          ADXL345_SendACK(0);                //��ӦACK
       }
   }
    ADXL345_Stop();                          //ֹͣ�ź�
    Delay5ms();
}

//*****************************************************************

//��ʼ��ADXL345��������Ҫ��ο�pdf�����޸�************************
void Init_ADXL345()
{
   
	 ADXL_345_ALL_Int();
	 HAL_Delay(500);
	 Single_Write_ADXL345(0x31,0x0B);   //������Χ,����16g��13λģʽ
   Single_Write_ADXL345(0x2C,0x08);   //�����趨Ϊ12.5 �ο�pdf13ҳ
   Single_Write_ADXL345(0x2D,0x08);   //ѡ���Դģʽ   �ο�pdf24ҳ
   Single_Write_ADXL345(0x2E,0x80);   //ʹ�� DATA_READY �ж�
   Single_Write_ADXL345(0x1E,0x00);   //X ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x1F,0x00);   //Y ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
   Single_Write_ADXL345(0x20,0x00);   //Z ƫ���� ���ݲ��Դ�������״̬д��pdf29ҳ
}





























/**************************************************************************//**
 * @file     wm_i2c_demo.c
 * @version  
 * @date 
 * @author    
 * @note
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd. All rights reserved.
 *****************************************************************************/
#include "wm_include.h"
#include "wm_drivers.h"
#include "wm_i2c.h"
#include "wm_gpio_afsel.h"
#include <string.h>

#define I2C_FREQ		(200000)

#define AT24C16		2047
#define AT24C512	65535  
//Mini STM32������ʹ�õ���24c02�����Զ���EE_TYPEΪAT24C02
#define EE_TYPE AT24C512

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  //IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		tls_i2c_write_byte(0XA0,1);	   //����д����
		tls_i2c_wait_ack();
		tls_i2c_write_byte(ReadAddr>>8,0);//���͸ߵ�ַ	    
	}
	else 
	{
		tls_i2c_write_byte(0XA0+((ReadAddr/256)<<1),1);   //����������ַ0XA0,д���� 	   
	}
	tls_i2c_wait_ack(); 
  tls_i2c_write_byte(ReadAddr%256,0);   //���͵͵�ַ
	tls_i2c_wait_ack();	    
	//IIC_Start();  	 	   
	tls_i2c_write_byte(0XA1,1);           //�������ģʽ			   
	tls_i2c_wait_ack();	 
  temp=tls_i2c_read_byte(0,1);		   
  //IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  //IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		tls_i2c_write_byte(0XA0,1);	    //����д����
		tls_i2c_wait_ack();
		tls_i2c_write_byte(WriteAddr>>8,0);//���͸ߵ�ַ	  
	}
	else 
	{
		tls_i2c_write_byte(0XA0+((WriteAddr/256)<<1),1);   //����������ַ0XA0,д���� 
	}		
	tls_i2c_wait_ack();	   
  tls_i2c_write_byte(WriteAddr%256,0);   //���͵͵�ַ
	tls_i2c_wait_ack(); 	 										  		   
	tls_i2c_write_byte(DataToWrite,0);     //�����ֽ�							   
	tls_i2c_wait_ack();  		    	   
  tls_i2c_stop();//����һ��ֹͣ���� 
	//delay_ms(10);	 
	tls_os_time_delay(10);
}
//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
	u8 temp=0;
	u16 AT24Cxx_FlagAdd = 65535;
	u8 FlagNumber = 0x55;
	temp=AT24CXX_ReadOneByte(AT24Cxx_FlagAdd);//����ÿ�ο�����дAT24CXX	
	printf("AT24CXX is first or no first init:%d\n",(temp==0x55)?0:1);	
	if(temp==FlagNumber)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(AT24Cxx_FlagAdd,FlagNumber);
	  temp=AT24CXX_ReadOneByte(AT24Cxx_FlagAdd);	  
		if(temp==FlagNumber)return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}

void AT24CXX_init(void)
{
	wm_i2c_scl_config(WM_IO_PB_13);
	wm_i2c_sda_config(WM_IO_PB_14);
	tls_i2c_init(I2C_FREQ);
	//tls_os_time_delay(200);
	
	if(AT24CXX_Check()!=1)
	{
		printf("\nAT24CXX check success!...\n");
	}
	else
	{
		printf("\nAT24CXX check faild...\n");
	}
}


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
//Mini STM32开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C512

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址  
//返回值  :读到的数据
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  //IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		tls_i2c_write_byte(0XA0,1);	   //发送写命令
		tls_i2c_wait_ack();
		tls_i2c_write_byte(ReadAddr>>8,0);//发送高地址	    
	}
	else 
	{
		tls_i2c_write_byte(0XA0+((ReadAddr/256)<<1),1);   //发送器件地址0XA0,写数据 	   
	}
	tls_i2c_wait_ack(); 
  tls_i2c_write_byte(ReadAddr%256,0);   //发送低地址
	tls_i2c_wait_ack();	    
	//IIC_Start();  	 	   
	tls_i2c_write_byte(0XA1,1);           //进入接收模式			   
	tls_i2c_wait_ack();	 
  temp=tls_i2c_read_byte(0,1);		   
  //IIC_Stop();//产生一个停止条件	    
	return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址    
//DataToWrite:要写入的数据
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  //IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		tls_i2c_write_byte(0XA0,1);	    //发送写命令
		tls_i2c_wait_ack();
		tls_i2c_write_byte(WriteAddr>>8,0);//发送高地址	  
	}
	else 
	{
		tls_i2c_write_byte(0XA0+((WriteAddr/256)<<1),1);   //发送器件地址0XA0,写数据 
	}		
	tls_i2c_wait_ack();	   
  tls_i2c_write_byte(WriteAddr%256,0);   //发送低地址
	tls_i2c_wait_ack(); 	 										  		   
	tls_i2c_write_byte(DataToWrite,0);     //发送字节							   
	tls_i2c_wait_ack();  		    	   
  tls_i2c_stop();//产生一个停止条件 
	//delay_ms(10);	 
	tls_os_time_delay(10);
}
//在AT24CXX里面的指定地址开始写入长度为Len的数据
//该函数用于写入16bit或者32bit的数据.
//WriteAddr  :开始写入的地址  
//DataToWrite:数据数组首地址
//Len        :要写入数据的长度2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//在AT24CXX里面的指定地址开始读出长度为Len的数据
//该函数用于读出16bit或者32bit的数据.
//ReadAddr   :开始读出的地址 
//返回值     :数据
//Len        :要读出数据的长度2,4
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
//检查AT24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
u8 AT24CXX_Check(void)
{
	u8 temp=0;
	u16 AT24Cxx_FlagAdd = 65535;
	u8 FlagNumber = 0x55;
	temp=AT24CXX_ReadOneByte(AT24Cxx_FlagAdd);//避免每次开机都写AT24CXX	
	printf("AT24CXX is first or no first init:%d\n",(temp==0x55)?0:1);	
	if(temp==FlagNumber)return 0;		   
	else//排除第一次初始化的情况
	{
		AT24CXX_WriteOneByte(AT24Cxx_FlagAdd,FlagNumber);
	  temp=AT24CXX_ReadOneByte(AT24Cxx_FlagAdd);	  
		if(temp==FlagNumber)return 0;
	}
	return 1;											  
}

//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
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


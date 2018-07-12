/*****************************************************************************
*
* File Name : wm_gpio_demo.c
*
* Description: gpio demo function
*
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd.
* All rights reserved.
*
* Author : dave
*
* Date : 2014-6-2
*****************************************************************************/
#include "wm_include.h"
#include "wm_drivers.h"
#include "wm_i2c.h"
#include "wm_gpio_afsel.h"

/******************** Device Address ********************/
#define		RTC_Address     0x64 

/******************** Alarm register ********************/
#define		Alarm_SC				0x07
#define		Alarm_MN				0x08
#define		Alarm_HR				0x09
#define		Alarm_WK				0x0A
#define		Alarm_DY				0x0B
#define		Alarm_MO				0x0C
#define		Alarm_YR				0x0D
#define		Alarm_EN				0x0E

/******************** Control Register *******************/
#define		CTR1            0x0F
#define		CTR2            0x10
#define		CTR3            0x11

/***************** Timer Counter Register ****************/
#define		Timer_Counter1	0x13
#define		Timer_Counter2	0x14
#define		Timer_Counter3	0x15

/******************** Battery Register ********************/
#define		Chg_MG          0x18		//充电管理寄存器地址
#define		Bat_H8          0x1A		//电量最高位寄存器地址
#define		Bat_L8          0x1B		//电量低八位寄存器地址

/*********************** ID Register **********************/
#define		ID_Address			0x72		//ID号起始地址

/********************** 报警中断宏定义 *********************/
#define		sec_ALM					0x01
#define		min_ALM					0x02
#define		hor_ALM					0x04
#define		wek_ALM					0x08
#define		day_ALM					0x10
#define		mon_ALM					0x20
#define		yar_ALM					0x40

/********************** 中断使能宏定义 **********************/
#define		INTDE						0x04		//倒计时中断
#define		INTAE						0x02		//报警中断
#define		INTFE						0x01		//频率中断

/********************** 中断演示宏定义 **********************/
#define 	FREQUENCY				0				//频率中断
#define 	ALARM						1				//报警中断
#define 	COUNTDOWN				2				//倒计时中断
#define 	DISABLE					3				//禁止中断

enum Freq{F_0Hz, F32KHz, F4096Hz, F1024Hz, F64Hz, F32Hz, F16Hz, F8Hz, \
					F4Hz, F2Hz, F1Hz, F1_2Hz, F1_4Hz, F1_8Hz, F1_16Hz, F_1s};

enum clk_Souce{S_4096Hz, S_1024Hz, S_1s, S_1min};

/*此结构体定义了倒计时中断可供配置的频率源、IM和初值主要参数*/
typedef	struct
{
	enum clk_Souce d_clk;
	u8   IM;	//IM=1:周期性中断
	u32  init_val;
} CountDown_Def;

/*********************************************
 * 函数名：WriteRTC_Enable
 * 描  述：RTC写允许程序
 * 输  入：无
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
u8 WriteRTC_Enable(void)
{
    //if(FALSE == I2CStart()) return FALSE;
    tls_i2c_write_byte(RTC_Address,1); 
    if(tls_i2c_wait_ack()== WM_FAILED){tls_i2c_stop();return FALSE;}
    tls_i2c_write_byte(CTR2,0);      
    tls_i2c_wait_ack();	
    tls_i2c_write_byte(0x80,0);//置WRTC1=1      
    tls_i2c_wait_ack();
    tls_i2c_stop(); 
		printf("111");
    //I2CStart();
    tls_i2c_write_byte(RTC_Address,1);      
    tls_i2c_wait_ack();   
    tls_i2c_write_byte(CTR1,0);
    tls_i2c_wait_ack();	
    tls_i2c_write_byte(0x84,0);//置WRTC2,WRTC3=1      
    tls_i2c_wait_ack();
    tls_i2c_stop(); 
		printf("222");
    return TRUE;
}

/*********************************************
 * 函数名：WriteRTC_Disable
 * 描  述：RTC写禁止程序
 * 输  入：无
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
u8 WriteRTC_Disable(void)
{
    //if(FALSE == I2CStart()) return FALSE;
    tls_i2c_write_byte(RTC_Address,1); 
    if(tls_i2c_wait_ack()!=WM_SUCCESS){tls_i2c_stop(); return FALSE;}
		printf("333");
    tls_i2c_write_byte(CTR1,0);//设置写地址0FH      
    tls_i2c_wait_ack();	
    tls_i2c_write_byte(0x0,0) ;//置WRTC2,WRTC3=0      
    tls_i2c_wait_ack();
    tls_i2c_write_byte(0x0,0) ;//置WRTC1=0(10H地址)      
    tls_i2c_wait_ack();
    tls_i2c_stop(); 
    return TRUE; 
}

/*********************************************
 * 函数名：RTC_WriteDate
 * 描  述：写RTC实时数据寄存器
 * 输  入：时间结构体指针
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
u8 RTC_WriteDate(Time_Def	*psRTC)	//写时间操作要求一次对实时时间寄存器(00H~06H)依次写入，
{                               //不可以单独对7个时间数据中的某一位进行写操作,否则可能会引起时间数据的错误进位. 
                                //要修改其中某一个数据 , 应一次性写入全部 7 个实时时钟数据.

		//WriteRTC_Enable();				//使能，开锁

		//I2CStart();
		tls_i2c_write_byte(RTC_Address,1); 
		if(tls_i2c_wait_ack()!=WM_SUCCESS){tls_i2c_stop(); return FALSE;}
		tls_i2c_write_byte(0,0);			//设置写起始地址      
		tls_i2c_wait_ack();
		tls_i2c_write_byte(psRTC->second,0);		//second     
		tls_i2c_wait_ack();	
		tls_i2c_write_byte(psRTC->minute,0);		//minute      
		tls_i2c_wait_ack();	
		tls_i2c_write_byte(psRTC->hour|0x80,0);//hour ,同时设置小时寄存器最高位（0：为12小时制，1：为24小时制）
		tls_i2c_wait_ack();	
		tls_i2c_write_byte(psRTC->week,0);		//week      
		tls_i2c_wait_ack();	
		tls_i2c_write_byte(psRTC->day,0);		//day      
		tls_i2c_wait_ack();	
		tls_i2c_write_byte(psRTC->month,0);		//month      
		tls_i2c_wait_ack();	
		tls_i2c_write_byte(psRTC->year,0);		//year      
		tls_i2c_wait_ack();	
		tls_i2c_stop();

		//WriteRTC_Disable();				//关锁
		return	TRUE;
}
/*********************************************
 * 函数名：RTCFlag_WriteDate
 * 描  述：
 * 输  入：
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
u8 RTCFlag_WriteDate(u8	flag)	//RTC frist write flag to 2CH address
{                               
		u8 RTCflag = 0x2C;
	
		WriteRTC_Enable();				//使能，开锁

		//I2CStart();
		tls_i2c_write_byte(RTC_Address,1); 
		if(tls_i2c_wait_ack()!=WM_SUCCESS){tls_i2c_stop(); return FALSE;}
		tls_i2c_write_byte(RTCflag,0);			//设置写起始地址 2CH     
		tls_i2c_wait_ack();
		tls_i2c_write_byte(flag,0);		//flag     
		tls_i2c_wait_ack();	
		tls_i2c_stop();

		WriteRTC_Disable();				//关锁
		return	TRUE;
}
/*********************************************
 * 函数名：RTC_ReadDate
 * 描  述：读RTC实时数据寄存器
 * 输  入：时间结构体指针
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
u8 RTC_ReadDate(Time_Def	*psRTC)
{
		//I2CStart();
		tls_i2c_write_byte(RTC_Address,1);      
		if(tls_i2c_wait_ack()!=WM_SUCCESS){tls_i2c_stop(); return FALSE;}
		tls_i2c_write_byte(0,0);
		tls_i2c_wait_ack();
		//I2CStart();	
		tls_i2c_write_byte(RTC_Address+1,1);
		tls_i2c_wait_ack();
		psRTC->second=tls_i2c_read_byte(1,0);
		//I2CAck();
		psRTC->minute=tls_i2c_read_byte(1,0);
		//I2CAck();
		psRTC->hour=tls_i2c_read_byte(1,0) & 0x7F;
		//I2CAck();
		psRTC->week=tls_i2c_read_byte(1,0);
		//I2CAck();
		psRTC->day=tls_i2c_read_byte(1,0);
		//I2CAck();
		psRTC->month=tls_i2c_read_byte(1,0);
		//I2CAck();
		psRTC->year=tls_i2c_read_byte(0,1);		
		//I2CNoAck();
		//tls_i2c_stop(); 
		return	TRUE;
}
/*********************************************
 * 函数名：RTCflag_ReadDate
 * 描  述：
 * 输  入：
 * 输  出：TRUE:操作成功，FALSE:操作失败
 ********************************************/
u8 RTCFlag_ReadDate(void)
{
		u8 RTCflag = 0x2C;
		u8 flag;
		//I2CStart();
		tls_i2c_write_byte(RTC_Address,1);      
		if(tls_i2c_wait_ack()!=WM_SUCCESS){tls_i2c_stop(); return FALSE;}
		tls_i2c_write_byte(RTCflag,0);
		tls_i2c_wait_ack();
		//I2CStart();	
		tls_i2c_write_byte(RTC_Address+1,1);
		tls_i2c_wait_ack();
		flag = tls_i2c_read_byte(0,1);		
		//I2CNoAck();
		//tls_i2c_stop();
		
		return	flag;
}
/*********************************************
 * 函数名     ：I2CWriteSerial
 * 描  述     ：I2C在指定地址写一字节数据
 * Device_Addr：I2C设备地址
 * Address    ：内部地址
 * length     ：字节长度
 * ps         ：缓存区指针
 * 输出       ：TRUE 成功，FALSE 失败
 ********************************************/	
u8 I2CWriteSerial(u8 DeviceAddress, u8 Address, u8 length, u8 *ps)
{
		if(DeviceAddress == RTC_Address)  WriteRTC_Enable();

		//I2CStart();
		tls_i2c_write_byte(DeviceAddress,1);
		if(!tls_i2c_wait_ack()){tls_i2c_stop(); return FALSE;}
		tls_i2c_write_byte(Address,0);			
		tls_i2c_wait_ack();
		for(;(length--)>0;)
		{ 	
			tls_i2c_write_byte(*(ps++),0);		
			//I2CAck();			
		}
		tls_i2c_stop(); 

		if(DeviceAddress == RTC_Address)  WriteRTC_Disable();
		return	TRUE;
}

/*********************************************
 * 函数名     ：I2CReadSerial
 * 描  述     ：I2C在指定地址写一字节数据
 * Device_Addr：I2C设备地址
 * Address    ：内部地址
 * length     ：字节长度
 * ps         ：缓存区指针
 * 输出       ：TRUE 成功，FALSE 失败
 ********************************************/	
u8 I2CReadSerial(u8 DeviceAddress, u8 Address, u8 length, u8 *ps)
{
		//I2CStart();
		tls_i2c_write_byte(DeviceAddress,1);      
		if(!tls_i2c_wait_ack()){tls_i2c_stop(); return FALSE;}
		tls_i2c_write_byte(Address,0);
		tls_i2c_wait_ack();
		//I2CStart();	
		tls_i2c_write_byte(DeviceAddress+1,1);
		tls_i2c_wait_ack();
		for(;--length>0;ps++)
		{
			*ps = tls_i2c_read_byte(1,0);
			//I2CAck();
		}
		*ps = tls_i2c_read_byte(0,1);	
		//I2CNoAck();
		//tls_i2c_stop(); 
		return	TRUE;
}

/*********************************************
 * 函数名：Set_CountDown
 * 描  述：设置倒计时中断
 * 输  入：CountDown_Init 倒计时中断结构体指针 
 * 输  出：无
 ********************************************/
void Set_CountDown(CountDown_Def *CountDown_Init)					
{
		u8 buf[6];
		u8 tem=0xF0;
		buf[0] = (CountDown_Init->IM<<6)|0xB4;							//10H
		buf[1] = CountDown_Init->d_clk<<4;									//11H
		buf[2] = 0;																					//12H
		buf[3] = CountDown_Init->init_val & 0x0000FF;				//13H
		buf[4] = (CountDown_Init->init_val & 0x00FF00) >> 8;//14H
		buf[5] = (CountDown_Init->init_val & 0xFF0000) >> 16;//15H
		I2CWriteSerial(RTC_Address,CTR2,1,&tem);
		I2CWriteSerial(RTC_Address,CTR2,6,buf);
}

/*********************************************
 * 函数名：Set_Alarm
 * 描  述：设置报警中断（闹钟功能）
 * Enable_config：使能设置  
 * psRTC：报警时间的时间结构体指针
 * 输  出：无
 ********************************************/
void Set_Alarm(u8 Enable_config, Time_Def *psRTC)					
{
		u8 buf[10];
		buf[0] = psRTC->second;
		buf[1] = psRTC->minute;
		buf[2] = psRTC->hour;
		buf[3] = 0;
		buf[4] = psRTC->day;
		buf[5] = psRTC->month;
		buf[6] = psRTC->year;
		buf[7] = Enable_config;
		buf[8] = 0xFF;
		buf[9] = 0x92;	
		I2CWriteSerial(RTC_Address,Alarm_SC,10,buf);
}

/*********************************************
 * 函数名：SetFrq
 * 描  述：设置RTC频率中断，从INT脚输出频率方波
 * 输  入：频率值
 * 输  出：无
 ********************************************/
void SetFrq(enum Freq F_Out)					
{
		u8 buf[2];
		buf[0] = 0xA1;
		buf[1] = F_Out;
		I2CWriteSerial(RTC_Address,CTR2,2,buf);
}

/*********************************************
 * 函数名：ClrINT
 * 描  述：禁止中断
 * int_EN：中断类型 INTDE、INTDE、INTDE
 * 输  出：无
 ********************************************/
void ClrINT(u8 int_EN)         
{
		u8 buf;
		buf = 0x80 & (~int_EN);
		I2CWriteSerial(RTC_Address,CTR2,1,&buf);
}

void Save_time(Time_Def *SaveTime)
{
	u8 datatmp[7];
	datatmp[0] = SaveTime->year;
	datatmp[1] = SaveTime->month;
	datatmp[2] = SaveTime->day;
	datatmp[3] = SaveTime->hour;
	datatmp[4] = SaveTime->minute;
	datatmp[5] = SaveTime->second;
	datatmp[6] = SaveTime->week;
	
	AT24CXX_Write(0,(u8 *)datatmp,sizeof(datatmp));
}

void Read_time(Time_Def *ReadTime)
{
	u8 datatmp[7];
	AT24CXX_Read(0,datatmp,sizeof(datatmp));
	ReadTime->year = datatmp[0];
	ReadTime->month = datatmp[1];
	ReadTime->day = datatmp[2];
	ReadTime->hour = datatmp[3];
	ReadTime->minute = datatmp[4];
	ReadTime->second = datatmp[5];
	ReadTime->week = datatmp[6];
}

Time_Def time_init={0x00,0x00,0x12,0x01,0x25,0x06,0x18};	//初始化实时时间
Time_Def sysTime;
Time_Def ReadTime;

int exrtc_save(void)
{
	u8 wRTCFlag = 0x55;
	
//	wm_i2c_scl_config(WM_IO_PB_13);
//	wm_i2c_sda_config(WM_IO_PB_14);
//	tls_i2c_init(200000);
	
	/************* 给RTC写入计时的初始时间 ************/		
	if(RTCFlag_ReadDate() != wRTCFlag)
	{
		printf("This RTC is frist used! MCU is writing flag now...\r\n");
		RTCFlag_WriteDate(wRTCFlag);
		if(RTCFlag_ReadDate() == wRTCFlag)
			printf("RTC flag was writed correct!\r\n");
		else
			printf("RTC flag was writed wrong!\r\n");

		RTC_WriteDate(&time_init);
	}
	else
	{
		printf("This RTC is not frist used! The RTC flag is:%2X\r\n",RTCFlag_ReadDate());
	}
		RTC_ReadDate(&sysTime);
		printf("write time:\r\n");
		printf("%02X-%02X-%02X  %02X:%02X:%02X  week:%02X\r\n\r\n", \
		sysTime.year, sysTime.month, sysTime.day, sysTime.hour,\
		sysTime.minute, sysTime.second, sysTime.week);
	while(1)
	{
		tls_os_time_delay(200);
		RTC_ReadDate(&sysTime);
		printf("write time:\r\n");
		printf("%02X-%02X-%02X  %02X:%02X:%02X week:%02X\r\n\r\n", \
						sysTime.year, sysTime.month, sysTime.day, sysTime.hour,\
						sysTime.minute, sysTime.second, sysTime.week);
		
		Save_time(&sysTime);
		Read_time(&ReadTime);
		printf("read time:");
		printf("%02X-%02X-%02X  %02X:%02X:%02X  week:%02X\r\n\r\n", \
						ReadTime.year, ReadTime.month, ReadTime.day, ReadTime.hour,\
						ReadTime.minute, ReadTime.second, ReadTime.week);
	}

	return WM_SUCCESS;
}

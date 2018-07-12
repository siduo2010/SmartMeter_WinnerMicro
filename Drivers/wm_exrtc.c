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
#define		Chg_MG          0x18		//������Ĵ�����ַ
#define		Bat_H8          0x1A		//�������λ�Ĵ�����ַ
#define		Bat_L8          0x1B		//�����Ͱ�λ�Ĵ�����ַ

/*********************** ID Register **********************/
#define		ID_Address			0x72		//ID����ʼ��ַ

/********************** �����жϺ궨�� *********************/
#define		sec_ALM					0x01
#define		min_ALM					0x02
#define		hor_ALM					0x04
#define		wek_ALM					0x08
#define		day_ALM					0x10
#define		mon_ALM					0x20
#define		yar_ALM					0x40

/********************** �ж�ʹ�ܺ궨�� **********************/
#define		INTDE						0x04		//����ʱ�ж�
#define		INTAE						0x02		//�����ж�
#define		INTFE						0x01		//Ƶ���ж�

/********************** �ж���ʾ�궨�� **********************/
#define 	FREQUENCY				0				//Ƶ���ж�
#define 	ALARM						1				//�����ж�
#define 	COUNTDOWN				2				//����ʱ�ж�
#define 	DISABLE					3				//��ֹ�ж�

enum Freq{F_0Hz, F32KHz, F4096Hz, F1024Hz, F64Hz, F32Hz, F16Hz, F8Hz, \
					F4Hz, F2Hz, F1Hz, F1_2Hz, F1_4Hz, F1_8Hz, F1_16Hz, F_1s};

enum clk_Souce{S_4096Hz, S_1024Hz, S_1s, S_1min};

/*�˽ṹ�嶨���˵���ʱ�жϿɹ����õ�Ƶ��Դ��IM�ͳ�ֵ��Ҫ����*/
typedef	struct
{
	enum clk_Souce d_clk;
	u8   IM;	//IM=1:�������ж�
	u32  init_val;
} CountDown_Def;

/*********************************************
 * ��������WriteRTC_Enable
 * ��  ����RTCд�������
 * ��  �룺��
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
 ********************************************/
u8 WriteRTC_Enable(void)
{
    //if(FALSE == I2CStart()) return FALSE;
    tls_i2c_write_byte(RTC_Address,1); 
    if(tls_i2c_wait_ack()== WM_FAILED){tls_i2c_stop();return FALSE;}
    tls_i2c_write_byte(CTR2,0);      
    tls_i2c_wait_ack();	
    tls_i2c_write_byte(0x80,0);//��WRTC1=1      
    tls_i2c_wait_ack();
    tls_i2c_stop(); 
		printf("111");
    //I2CStart();
    tls_i2c_write_byte(RTC_Address,1);      
    tls_i2c_wait_ack();   
    tls_i2c_write_byte(CTR1,0);
    tls_i2c_wait_ack();	
    tls_i2c_write_byte(0x84,0);//��WRTC2,WRTC3=1      
    tls_i2c_wait_ack();
    tls_i2c_stop(); 
		printf("222");
    return TRUE;
}

/*********************************************
 * ��������WriteRTC_Disable
 * ��  ����RTCд��ֹ����
 * ��  �룺��
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
 ********************************************/
u8 WriteRTC_Disable(void)
{
    //if(FALSE == I2CStart()) return FALSE;
    tls_i2c_write_byte(RTC_Address,1); 
    if(tls_i2c_wait_ack()!=WM_SUCCESS){tls_i2c_stop(); return FALSE;}
		printf("333");
    tls_i2c_write_byte(CTR1,0);//����д��ַ0FH      
    tls_i2c_wait_ack();	
    tls_i2c_write_byte(0x0,0) ;//��WRTC2,WRTC3=0      
    tls_i2c_wait_ack();
    tls_i2c_write_byte(0x0,0) ;//��WRTC1=0(10H��ַ)      
    tls_i2c_wait_ack();
    tls_i2c_stop(); 
    return TRUE; 
}

/*********************************************
 * ��������RTC_WriteDate
 * ��  ����дRTCʵʱ���ݼĴ���
 * ��  �룺ʱ��ṹ��ָ��
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
 ********************************************/
u8 RTC_WriteDate(Time_Def	*psRTC)	//дʱ�����Ҫ��һ�ζ�ʵʱʱ��Ĵ���(00H~06H)����д�룬
{                               //�����Ե�����7��ʱ�������е�ĳһλ����д����,������ܻ�����ʱ�����ݵĴ����λ. 
                                //Ҫ�޸�����ĳһ������ , Ӧһ����д��ȫ�� 7 ��ʵʱʱ������.

		//WriteRTC_Enable();				//ʹ�ܣ�����

		//I2CStart();
		tls_i2c_write_byte(RTC_Address,1); 
		if(tls_i2c_wait_ack()!=WM_SUCCESS){tls_i2c_stop(); return FALSE;}
		tls_i2c_write_byte(0,0);			//����д��ʼ��ַ      
		tls_i2c_wait_ack();
		tls_i2c_write_byte(psRTC->second,0);		//second     
		tls_i2c_wait_ack();	
		tls_i2c_write_byte(psRTC->minute,0);		//minute      
		tls_i2c_wait_ack();	
		tls_i2c_write_byte(psRTC->hour|0x80,0);//hour ,ͬʱ����Сʱ�Ĵ������λ��0��Ϊ12Сʱ�ƣ�1��Ϊ24Сʱ�ƣ�
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

		//WriteRTC_Disable();				//����
		return	TRUE;
}
/*********************************************
 * ��������RTCFlag_WriteDate
 * ��  ����
 * ��  �룺
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
 ********************************************/
u8 RTCFlag_WriteDate(u8	flag)	//RTC frist write flag to 2CH address
{                               
		u8 RTCflag = 0x2C;
	
		WriteRTC_Enable();				//ʹ�ܣ�����

		//I2CStart();
		tls_i2c_write_byte(RTC_Address,1); 
		if(tls_i2c_wait_ack()!=WM_SUCCESS){tls_i2c_stop(); return FALSE;}
		tls_i2c_write_byte(RTCflag,0);			//����д��ʼ��ַ 2CH     
		tls_i2c_wait_ack();
		tls_i2c_write_byte(flag,0);		//flag     
		tls_i2c_wait_ack();	
		tls_i2c_stop();

		WriteRTC_Disable();				//����
		return	TRUE;
}
/*********************************************
 * ��������RTC_ReadDate
 * ��  ������RTCʵʱ���ݼĴ���
 * ��  �룺ʱ��ṹ��ָ��
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
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
 * ��������RTCflag_ReadDate
 * ��  ����
 * ��  �룺
 * ��  ����TRUE:�����ɹ���FALSE:����ʧ��
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
 * ������     ��I2CWriteSerial
 * ��  ��     ��I2C��ָ����ַдһ�ֽ�����
 * Device_Addr��I2C�豸��ַ
 * Address    ���ڲ���ַ
 * length     ���ֽڳ���
 * ps         ��������ָ��
 * ���       ��TRUE �ɹ���FALSE ʧ��
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
 * ������     ��I2CReadSerial
 * ��  ��     ��I2C��ָ����ַдһ�ֽ�����
 * Device_Addr��I2C�豸��ַ
 * Address    ���ڲ���ַ
 * length     ���ֽڳ���
 * ps         ��������ָ��
 * ���       ��TRUE �ɹ���FALSE ʧ��
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
 * ��������Set_CountDown
 * ��  �������õ���ʱ�ж�
 * ��  �룺CountDown_Init ����ʱ�жϽṹ��ָ�� 
 * ��  ������
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
 * ��������Set_Alarm
 * ��  �������ñ����жϣ����ӹ��ܣ�
 * Enable_config��ʹ������  
 * psRTC������ʱ���ʱ��ṹ��ָ��
 * ��  ������
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
 * ��������SetFrq
 * ��  ��������RTCƵ���жϣ���INT�����Ƶ�ʷ���
 * ��  �룺Ƶ��ֵ
 * ��  ������
 ********************************************/
void SetFrq(enum Freq F_Out)					
{
		u8 buf[2];
		buf[0] = 0xA1;
		buf[1] = F_Out;
		I2CWriteSerial(RTC_Address,CTR2,2,buf);
}

/*********************************************
 * ��������ClrINT
 * ��  ������ֹ�ж�
 * int_EN���ж����� INTDE��INTDE��INTDE
 * ��  ������
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

Time_Def time_init={0x00,0x00,0x12,0x01,0x25,0x06,0x18};	//��ʼ��ʵʱʱ��
Time_Def sysTime;
Time_Def ReadTime;

int exrtc_save(void)
{
	u8 wRTCFlag = 0x55;
	
//	wm_i2c_scl_config(WM_IO_PB_13);
//	wm_i2c_sda_config(WM_IO_PB_14);
//	tls_i2c_init(200000);
	
	/************* ��RTCд���ʱ�ĳ�ʼʱ�� ************/		
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

#ifndef __WM_DRIVERS_H__
#define __WM_DRIVERS_H__

#include "wm_include.h"

#define    RELAY_TASK_SIZE      1024
#define    LCD_TASK_SIZE        1024
#define    RTC_TASK_SIZE        1024
#define    UART1_TASK_SIZE      1024
#define    WIFI_TASK_SIZE       1024
#define    SOCK_TASK_SIZE       1024

// user prio 32 - 60
#define  RELAY_TASK_PRIO			 32
#define  LCD_TASK_PRIO			   (RELAY_TASK_PRIO+1)
#define  RTC_TASK_PRIO			   (LCD_TASK_PRIO+1)
#define  UART1_TASK_PRIO			 (RTC_TASK_PRIO+1)
#define  WIFI_TASK_PRIO				 50					// 用户任务优先级
#define  SOCKET_RCV_TASK_PRIO  51

#define RELAY_QUEUE_SIZE	32
#define LCD_QUEUE_SIZE	32
#define RTC_QUEUE_SIZE	32
#define UART1_QUEUE_SIZE	32
#define WIFI_QUEUE_SIZE		4					// 用户消息队列的size

static tls_os_queue_t 	*relay_q = NULL;
static tls_os_queue_t 	*lcd_q = NULL;
static tls_os_queue_t 	*rtc_q = NULL;
static tls_os_queue_t 	*uart1_q = NULL;
static tls_os_queue_t   *gsUserTaskQueue = NULL;

static OS_STK 			RelayTaskStk[RELAY_TASK_SIZE]; 
static OS_STK 			LCDTaskStk[LCD_TASK_SIZE]; 
static OS_STK 			RTCTaskStk[RTC_TASK_SIZE]; 
static OS_STK 			UART1TaskStk[UART1_TASK_SIZE]; 
static OS_STK       WifiTaskStk[WIFI_TASK_PRIO]; 
static OS_STK    		*sock_rcv_task_stk[SOCK_TASK_SIZE];

#define USER_LED_IO		WM_IO_PB_10

#define MSG_UART_RX_DATA   0x02  // uart rx

#define CMD_GET_MAC     1
#define CMD_SET_LED     2
#define CMD_GET_LED     3
#define CMD_REPORT_LED  4

typedef struct _uart1_struct{
    s16 rx_len;
    s16 tx_len;
    u8 *rx_buf;
    u8 *tx_buf;
}uart1_s;

typedef struct{
    u8 RelayStatus;
    u8 ScreenTime;
}MeterData;

/*此结构体定义了时间信息包括年、月、日、星期、时、分、秒*/
typedef	struct
{
	u8	second;
	u8	minute;
	u8	hour;
	u8	week;
	u8	day;
	u8	month;
	u8	year;
} Time_Def;

/*此结构体定义了...*/
typedef	struct
{
	s32	WARTI_Para;
	s32	WARTU_Para;
	s32	WAPT_Para;
	s32	WWAPT_Para;
	s32	EGYTH_Para;
	s32	CTH_Para;
	s32	WARTM_Para;
} Calib_Para;

/*此结构体定义了...*/
typedef struct user_sock_u{
    struct ip_mreq mreq;
    char *sock_rx;
    char *sock_tx;
    int conn_status;
    int socket_num;
    u32 remote_ip;
    u32 remote_port;
    u32 rx_len;
    u32 tx_len;
}ST_User_Sock_U;

/*此结构体定义了...*/
typedef struct{
	u8 WriteRegBuf[8];  //写Reg数组
	u8 ReadRegBuf[8];	 //读Reg数组
	u8 ReadRegBackBuf[8];    //保存读出的Reg数据，进行读写验证缓存
}MeterProtocol;

/*此数组定义了...*/
extern u8 UDPAck[54];
extern u8 UDPAckLen;

extern MeterProtocol meterprotocol;
extern Calib_Para    calib_para;

//UDP pro.
#define RelayAdd             0x01
#define TimeAdd              0x02
#define ActivePositiveAdd    0x03
#define ActiveNegativeAdd    0x04
#define ActiveTotalAdd       0x05
#define ReactivePositiveAdd  0x06
#define ReactiveNegativeAdd  0x07
#define ReactiveTotalAdd     0x08
#define PositiveTotalAdd     0x09
#define NegativeTotalAdd     0x0A
#define EnergyTotalAdd       0x0B
#define InstantCurrentAdd    0x0C
#define InstantVoltageAdd    0x0D
#define InstantPowerAdd      0x0E
#define ClearAll						 0x0F
#define ScreenTimeAdd    		 0x10
#define BillTimeAdd					 0x11

//校表参数协议地址
#define NominalVoltage 	     0xEF
#define NominalCurrent 	     0xF0
#define VotageGain 	         0xF1
#define Current1Gain 	       0xF2
#define Current2Gain 	       0xF3
#define SimulateVoltage      0xF4
#define SimulateCurrent1     0xF5
#define SimulateCurrent2     0xF6
#define PulseConstant				 0xF7
#define StartCurrent				 0xF8
#define Ib100_10						 0xF9
#define Ib100_05L					 	 0xFA
#define Ib1_10							 0xFB
#define InstantVoltage 			 0xFC
#define InstantCurrent1			 0xFD
#define InstantCurrent2			 0xFE
#define InstantPower				 0xFF

//wifi

#define KEY_SCAN_TIME					(configTICK_RATE_HZ/10)	// 一键配置按键扫描时间100ms
#define KEY_IO_ONESHOT				WM_IO_PA_00		// 一键配置使用的IO
#define KEY_LONG_COUNT				10					// 1s

#define MSG_NET_UP						0x01				// 加网成功消息
#define MSG_NET_DOWN					0x02				// 断网消息
#define MSG_ONESHOT						0x03				// 一键配置消息
#define MSG_KEY								0x04				// 用户按键
#define MSG_SOCKET_ERR    		0x05
#define MSG_SOCKET_SND      	0x06

#define UDP_LOCAL_PORT       	1000
#define USER_SOCK_BUF_SIZE   	1024

#define UART1									1

//Meter

/////////////////////////////////////*模拟控制寄存器*/
#define AnalogControlReg0Add 	0x0185
#define AnalogControlReg1Add 	0x0186
#define AnalogControlReg2Add 	0x0187
/////////////////////////////////////*系统配置寄存器*/
#define SystemConfigRegAdd   	0x0180
/////////////////////////////////////*计量控制寄存器*/
#define MeterControlReg0Add  	0x0183
#define MeterControlReg1Add  	0x0184
/////////////////////////////////////*计量数据寄存器*/
//直流分量寄存器
#define DCU 									0x0114
#define DCI 									0x0115
#define DCM 									0x0116
#define DCIM 									0x00F9
#define DCTM 									0x0101
//频率寄存器
#define FREQ 									0x019A
#define SAFREQ 								0x011D
#define AFREQ 								0x011E
//电压/电流/测量信号有效值寄存器
#define ARRTI 								0x0105
#define ARRTU 								0x0104
#define ARRTM 								0x0106
#define BRRTU 								0x0109
#define BRRTI 								0x010A
#define ARTI 									0x010E
#define ARTU 									0x010D
#define ARTM 									0x010F
#define ARTIM 								0x00F8
#define BRTU 									0x0112
#define BRTI 									0x0113
#define AARTU 								0x011B
#define AARTI 								0x011C
#define AARTM 								0x0117
#define ABRTU 								0x0121
#define ABRTI 								0x0122
//有功/无功功率寄存器
#define RAP 									0x0102
#define RAQ 									0x0103
#define RBP 									0x0107
#define RBQ 									0x0108
#define IAP 									0x010B
#define IAQ 									0x010C
#define IBP 									0x0110
#define IBQ 									0x0111
#define AAP 									0x0119
#define AAQ 									0x011A
#define ABP 									0x011F
#define ABQ 									0x0120
//有功/无功能量累加寄存器
#define PEGY 									0x01A1
#define NEGY 									0x01A2
//有功/无功CF脉冲计数器
#define PCFCNT 								0x01A3
#define NCFCNT 								0x01A4
//功率值寄存器
#define DATACP 								0x0189
/////////////////////////////////////*校表参数寄存器*/
//预设直流偏置值寄存器
#define ZZDCI 								0x0123
#define ZZDCU 								0x0124
//电压/电流/测量值有效校正寄存器
#define WARTI 								0x012C
#define WARTU 								0x0132
#define WARTM 								0x012D
#define WBRTI 								0x0126
#define WBRTU 								0x012B
//全波有功全波有功/无功率校正寄存器
#define WAPT 								  0x012E
#define WAQT 								  0x0130
#define WWAPT 								0x012F
#define WWAQT 								0x0131
#define WBPT 								  0x0127
#define WBQT 								  0x0129
#define WWBPT 								0x0128
#define WWBQT 								0x012A
//门限值寄存器
#define EGYTH 								0x0181
#define CTH 								  0x0182
#define IDETTH								0x0134
//带通滤波器系数寄存器
#define BPFPARA 							0x0125
/////////////////////////////////////*校表和寄存器*/
#define CKSUM 								0x0133
/////////////////////////////////////*额定电压标准值*/
#define NominalVoltage 				0x1B734F8D

//Regulate para

/////////////////////////////////////*校表参数保存寄存器*/






#define RacWrite              0x02
#define RacRead               0x01
//
void UserRelayCtrl(u8 status);
u8 GetRelayStatus(void);
	
//
void CreateRelayTask(void);
void CreateLCDTask(void);
void CreateRTCTask(void);
void CreateUart1Task(void);
void CreateWifiTask(void);

//
void AT24CXX_init(void);
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite);
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead);
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite);
u8 AT24CXX_ReadOneByte(u16 ReadAddr);

//ExRTC
void Read_time(Time_Def *ReadTime);
u8 RTC_WriteDate(Time_Def	*psRTC);
u8 RTC_ReadDate(Time_Def	*psRTC);

//LCD driver
void LCD_init(void);
void UserLcdBlink(u8 status);
void UserLcdBlinkProc(void);
void UserLcdCtrl(u8 status);

//led driver
void UserLedInit(void);
void UserLedCtrl(u8 status);
u8 UserLedGetStatus(void);
void UserLedOnOff(void);

//
void Uart1TaskProc(void* param);
void Uart1Init(void);

//Meter function
void Meter_init(void);
void ReadVoltage(void);
void ReadCurrent(void);
u8 CalibVoltage(u8 StandardVoltage);
u8 CalibCurrent(u8 StandardCurrent);

//
void Data_Handle(char *Data,u8 Length);

#endif

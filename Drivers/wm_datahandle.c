#include "wm_include.h"
#include "wm_drivers.h"
#include "string.h"
	
/*此结构体定义了串口的帧格式*/
typedef	struct
{
	u8	Head;
	u8	WR;
	u8	Function;
	u8	data;
	u8	Check;
	u8	End;
} DataFrame;

u8 UDPAck[54]={0};
u8 UDPAckLen=0;
const u8 Wflag=0xAA; 
const u8 Rflag=0x55;

static void Write_Meter(char *data);
static void Read_Meter(char *data);
static void Write_Ack(u8 DeviceAdd,u8 isOK);
static void Read_Ack(u8 DeviceAdd,u8 isOK,u8 *Data,u8 DataLen);

/*************************************************************************** 
* Description: 
*
* Auth: 
*
* Date: 
****************************************************************************/
void Data_Handle(char *Data,u8 Length)
{
  DataFrame dataframe;
	char *temp;
	u8 tempLength,i;
	u8 checkOut=0;
	
	dataframe.Head = 0x5A;
	dataframe.End = 0xA5;
	
	temp = Data;
	tempLength = Length;
	
	for(i=0;i<tempLength-2;i++)
  {
     checkOut += temp[i];
  }
   
  checkOut  = ~checkOut;
  checkOut  = checkOut + 0x33;
	printf("checkOut is:%x\r\n",checkOut);
	
	if(temp[0]==dataframe.Head && temp[tempLength-1]==dataframe.End && temp[tempLength-2]==checkOut)
	{	
			printf("this data is correct!\n");
			printf("this data is:");
			for(i=0;i<tempLength;i++)
				printf("%x\r\n",temp[i]);
			if(temp[1]==Wflag)
			{
				Write_Meter(Data);
			}
			if(temp[1]==Rflag)
			{
				Read_Meter(Data);
			}
	}	
	else
	{
		printf("this data is:");
		for(i=0;i<tempLength;i++)
				printf("%x\r\n",temp[i]);
		printf("this data is uncorrect!\n");
	}	
}

static void Write_Meter(char *data)
{
	char *temp;
	u8 DeviceAdd;
	temp = data;
	Time_Def time_write;
	u8 BillTime_write[7]={};
	
	DeviceAdd = temp[2];
	
	switch(DeviceAdd)
	{
		case RelayAdd:
			UserRelayCtrl(temp[3]);
			tls_os_time_delay(5);
			if(GetRelayStatus() == temp[3])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
		
		case TimeAdd:
			if(temp[3] > 0x59) time_write.second = 0x00;
			else time_write.second = temp[3];
			if(temp[4] > 0x59) time_write.minute = 0x00;
			else time_write.minute = temp[4];
			if(temp[5] > 0x23) time_write.hour   = 0x00;
			else time_write.hour   = temp[5];
			if(temp[6] > 0x07) time_write.week   = 0x01;
			else time_write.week   = temp[6];
			if(temp[7] > 0x31) time_write.day    = 0x01;
			else time_write.day    = temp[7];
			if(temp[8] > 0x12) time_write.month  = 0x01;
			else time_write.month  = temp[8];
			if(temp[9] > 0x50) time_write.year   = 0x00;
			else time_write.year   = temp[9];
		
			RTC_WriteDate(&time_write);
			Write_Ack(DeviceAdd,1);
		break;
		
		case ActivePositiveAdd:
			AT24CXX_Write(0x0041,0x00,4);
			printf("ActivePositive cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case ActiveNegativeAdd:
			AT24CXX_Write(0x0045,0x00,4);
			printf("ActiveNegative cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case ActiveTotalAdd:
			AT24CXX_Write(0x0049,0x00,4);
			printf("ActiveTotal cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case ReactivePositiveAdd:
			AT24CXX_Write(0x004d,0x00,4);
			printf("ReactivePositive cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case ReactiveNegativeAdd:
			AT24CXX_Write(0x0051,0x00,4);
			printf("ReactiveNegative cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case ReactiveTotalAdd:
			AT24CXX_Write(0x0055,0x00,4);
			printf("ReactiveTotal cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case PositiveTotalAdd:
			AT24CXX_Write(0x0059,0x00,4);
			printf("PositiveTotal cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case NegativeTotalAdd:
			AT24CXX_Write(0x005d,0x00,4);
			printf("NegativeTotal cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case EnergyTotalAdd:
			AT24CXX_Write(0x0061,0x00,4);
			printf("EnergyTotal cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case InstantCurrentAdd:
			AT24CXX_Write(0x0065,0x00,4);
			printf("InstantCurrent cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case InstantVoltageAdd:
			AT24CXX_Write(0x0069,0x00,4);
			printf("InstantVotage cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case InstantPowerAdd:
			AT24CXX_Write(0x006d,0x00,4);
			printf("InstantPower cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case ClearAll:
			AT24CXX_Write(0x0041,0x00,48);
			printf("InstantPower cleared!\r\n");
			Write_Ack(DeviceAdd,1);
		break;
		
		case ScreenTimeAdd:
			AT24CXX_WriteOneByte(0x0071,temp[3]);
			if(AT24CXX_ReadOneByte(0x0071) == temp[3])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
		
		case BillTimeAdd:
			if(temp[3] > 0x59) BillTime_write[0] = 0x00;
			else BillTime_write[0] = temp[3];
			if(temp[4] > 0x59) BillTime_write[1] = 0x00;
			else BillTime_write[1] = temp[4];
			if(temp[5] > 0x23) BillTime_write[2] = 0x00;
			else BillTime_write[2] = temp[5];
			if(temp[6] > 0x07) BillTime_write[3] = 0x01;
			else BillTime_write[3] = temp[6];
			if(temp[7] > 0x31) BillTime_write[4] = 0x01;
			else BillTime_write[4] = temp[7];
			if(temp[8] > 0x12) BillTime_write[5] = 0x01;
			else BillTime_write[5] = temp[8];
			if(temp[9] > 0x50) BillTime_write[6] = 0x00;
			else BillTime_write[6] = temp[9];
		
			AT24CXX_Write(0x0072,BillTime_write,7);
			Write_Ack(DeviceAdd,1);
		break;
		
		case NominalVoltage:
			AT24CXX_WriteOneByte(0x0090,temp[3]);
			AT24CXX_WriteOneByte(0x0091,temp[4]);
			if(AT24CXX_ReadOneByte(0x0090) == temp[3] && AT24CXX_ReadOneByte(0x0091) == temp[4])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
			
		case NominalCurrent:
			AT24CXX_WriteOneByte(0x0092,temp[3]);
			if(AT24CXX_ReadOneByte(0x0092) == temp[3])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
			
		case VotageGain:
			AT24CXX_WriteOneByte(0x0093,temp[3]);
			if(AT24CXX_ReadOneByte(0x0093) == temp[3])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
		
		case Current1Gain:
			AT24CXX_WriteOneByte(0x0094,temp[3]);
			if(AT24CXX_ReadOneByte(0x0094) == temp[3])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
			
		case Current2Gain:
			AT24CXX_WriteOneByte(0x0095,temp[3]);
			if(AT24CXX_ReadOneByte(0x0095) == temp[3])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
			
		case SimulateVoltage://需要转换
				
		break;
		
		case SimulateCurrent1://需要转换
			
		break;
		
		case SimulateCurrent2://需要转换
				
		break;
		
		case PulseConstant:
			AT24CXX_WriteOneByte(0x00A2,temp[3]);
			AT24CXX_WriteOneByte(0x00A3,temp[4]);
			if(AT24CXX_ReadOneByte(0x00A2) == temp[3] && AT24CXX_ReadOneByte(0x00A3) == temp[4])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
		
		case StartCurrent://需要转换
				
		break;
		
		case Ib100_10://需要转换
			
		break;
		
		case Ib100_05L://需要转换
				
		break;
		
		case Ib1_10://需要转换
			
		break;
		
		case InstantVoltage:
			AT24CXX_WriteOneByte(0x00B2,temp[3]);
			AT24CXX_WriteOneByte(0x00B3,temp[4]);
			if(AT24CXX_ReadOneByte(0x00B2) == temp[3] && AT24CXX_ReadOneByte(0x00B3) == temp[4])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
		
		case InstantCurrent1:
			AT24CXX_WriteOneByte(0x00B4,temp[3]);
			if(AT24CXX_ReadOneByte(0x00B4) == temp[3])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
		
		case InstantCurrent2:
			AT24CXX_WriteOneByte(0x00B5,temp[3]);
			if(AT24CXX_ReadOneByte(0x00B5) == temp[3])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
		
		case InstantPower:
			AT24CXX_WriteOneByte(0x00B6,temp[3]);
			AT24CXX_WriteOneByte(0x00B7,temp[4]);
			if(AT24CXX_ReadOneByte(0x00B6) == temp[3] && AT24CXX_ReadOneByte(0x00B7) == temp[4])
				Write_Ack(DeviceAdd,1);
			else
				Write_Ack(DeviceAdd,0);
		break;
		
		default:
		break;
	}
}

static void Read_Meter(char *data)
{
	Time_Def GetTime;
	char *temp;
	u8 tempData[48];
	u8 DeviceAdd;
	temp = data;
	Time_Def time_write;
	
	DeviceAdd = temp[2];
	
	switch(DeviceAdd)
	{
		case RelayAdd:
				tempData[0] = GetRelayStatus();
				Read_Ack(DeviceAdd,1,tempData,1);
		break;
		
		case TimeAdd:
			AT24CXX_Read(0x0000,tempData,7);
			Read_Ack(DeviceAdd,1,tempData,7);
		break;
		
		case ActivePositiveAdd:
			AT24CXX_Read(0x0041,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case ActiveNegativeAdd:
			AT24CXX_Read(0x0045,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case ActiveTotalAdd:
			AT24CXX_Read(0x0049,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case ReactivePositiveAdd:
			AT24CXX_Read(0x004d,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case ReactiveNegativeAdd:
			AT24CXX_Read(0x0051,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case ReactiveTotalAdd:
			AT24CXX_Read(0x0055,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case PositiveTotalAdd:
			AT24CXX_Read(0x0059,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case NegativeTotalAdd:
			AT24CXX_Read(0x005d,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case EnergyTotalAdd:
			AT24CXX_Read(0x0061,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case InstantCurrentAdd:
			AT24CXX_Read(0x0065,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case InstantVoltageAdd:
			AT24CXX_Read(0x0069,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case InstantPowerAdd:
			AT24CXX_Read(0x006d,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case ClearAll:
			AT24CXX_Read(0x0011,tempData,48);
			Read_Ack(DeviceAdd,1,tempData,48);
		break;
		
		case ScreenTimeAdd:
			AT24CXX_Read(0x0071,tempData,1);
			Read_Ack(DeviceAdd,1,tempData,1);
		break;
		
		case BillTimeAdd:
			AT24CXX_Read(0x0072,tempData,7);
			Read_Ack(DeviceAdd,1,tempData,7);
		break;
		
		case InstantVoltage:
			AT24CXX_Read(0x0069,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case InstantCurrent1:
			AT24CXX_Read(0x0065,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
		
		case InstantCurrent2:
			AT24CXX_Read(0x0065,tempData,4);
			Read_Ack(DeviceAdd,1,tempData,4);
		break;
			
		default:
		break;
	}
}

static void Write_Ack(u8 DeviceAdd,u8 isOK)
{
	u8 Check = 0;
	
	UDPAck[0] = 0x5A;
	UDPAck[1] = Wflag;
	UDPAck[2] = DeviceAdd;
	UDPAck[3] = isOK;
	
	Check = UDPAck[0] + UDPAck[1] + UDPAck[2] + UDPAck[3];
	Check = ~Check + 0x33;
	UDPAck[4] = Check;
	
	UDPAck[5] = 0xA5;
	
	UDPAckLen = 6;
}

static void Read_Ack(u8 DeviceAdd,u8 isOK,u8 *Data,u8 DataLen)
{
	u8 i=0,Check = 0;
	
	UDPAck[0] = 0x5A;
	UDPAck[1] = Rflag;
	UDPAck[2] = DeviceAdd;
	UDPAck[3] = isOK;
	
	for(i=0;i<DataLen;i++)
	{
		UDPAck[4+i] = Data[i];
	}
	
	for(i=0;i<DataLen+4;i++)
		Check += UDPAck[i];
	Check = ~Check + 0x33;
	UDPAck[DataLen+4] = Check;
	
	UDPAck[DataLen+5] = 0xA5;
	
	UDPAckLen = DataLen + 6;
}

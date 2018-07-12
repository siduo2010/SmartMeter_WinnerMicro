#include "wm_include.h"
#include "wm_drivers.h"
#include "math.h"
//#include "stdlib.h"

static void MeterControlRegPara_init(void);
static u8 CheckRegDataWrited(u8 *RegData);
static void WriteV9261fReg(u16 Addr,s32 Data);
static u8 *ReadV9261fReg(u16 Addr);

MeterProtocol meterprotocol;
Calib_Para   calib_para;

/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
static void MeterControlRegPara_init(void)
{	
	WriteV9261fReg(AnalogControlReg0Add,0xff201740);	//WriteAnalogControlReg0:������·ADC
	WriteV9261fReg(AnalogControlReg1Add,0x30000000);	//WriteAnalogControlReg1:defult para,-60ppm
																										//ѡ�����·�๦��ADC�������ߵ�������
																										//����λ����ѡ��Ĭ��ֵ
	WriteV9261fReg(AnalogControlReg2Add,0x00040000);  //WriteAnalogControlReg2:���������·����Ϊ�߹���״̬
																										//ѡ��Ĭ�Ͼ�����
	WriteV9261fReg(MeterControlReg1Add,0x0a0b0000);		//WriteMeterControlReg1:�ǲ�У��ֵ
																										//��������Ϊ1��ʹ�ܸ��ź�����
																										//ѡ��Aͨ������
	WriteV9261fReg(MeterControlReg0Add,0x0c000080);		//WriteMeterControlReg0:�üĴ��������ν�������
																										//��һ���ڲ����������������ó����3λ֮��ļĴ���
																										//�����λ�ǿ�������������CF���
																										//��Ҫ�����в���������ɣ�����ADC֮��ȴ�500ms�ٿ�����
																										//Bit7������1����������ǷѹʱCF���������
	WriteV9261fReg(SystemConfigRegAdd,0x01fb0004);		//SystemConfigReg:�üĴ�����bit[8:0]����ϵͳ�����Ĺؼ���Ϣ
																										//bit[24:16]�Ƕ�bit[8:0]�ı���
																										//Ϊ��ϵͳ�������������������Ƽ�ֵ��
	WriteV9261fReg(BPFPARA,0x811D2BA7);								//BPFPARAReg
	//WriteV9261fReg(MeterControlReg0Add,0xEC000080);		//WriteMeterControlReg0
}

/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
static void MeterErrorRegPara_init(void)
{	
	u8 Calib_Para[28];
	
	AT24CXX_Read(0x10,Calib_Para,28);	//��ȡУ�����
	//printf("WARTU_Para is:%x-%x-%x-%x\r\n",Calib_Para[4],Calib_Para[5],Calib_Para[6],Calib_Para[7]);
	
	calib_para.WARTI_Para =  (u32)((Calib_Para[0] & 0x000000ff)<<24) + \
													 (u32)((Calib_Para[1] & 0x000000ff)<<16) + \
													 (u32)((Calib_Para[2] & 0x000000ff)<<8 ) + \
													 (u32)(Calib_Para[3] & 0x000000ff);
	
	calib_para.WARTU_Para =  (u32)((Calib_Para[4] & 0x000000ff)<<24) + \
													 (u32)((Calib_Para[5] & 0x000000ff)<<16) + \
													 (u32)((Calib_Para[6] & 0x000000ff)<<8 ) + \
													 (u32)(Calib_Para[7] & 0x000000ff);
	
	calib_para.WAPT_Para =   (u32)((Calib_Para[8] & 0x000000ff)<<24) + \
													 (u32)((Calib_Para[9] & 0x000000ff)<<16) + \
													 (u32)((Calib_Para[10] & 0x000000ff)<<8 ) + \
													 (u32)(Calib_Para[11] & 0x000000ff);
	
	calib_para.WWAPT_Para =  (u32)((Calib_Para[12] & 0x000000ff)<<24) + \
													 (u32)((Calib_Para[13] & 0x000000ff)<<16) + \
													 (u32)((Calib_Para[14] & 0x000000ff)<<8 ) + \
													 (u32)(Calib_Para[15] & 0x000000ff);
													 
	calib_para.EGYTH_Para =  (u32)((Calib_Para[16] & 0x000000ff)<<24) + \
													 (u32)((Calib_Para[17] & 0x000000ff)<<16) + \
													 (u32)((Calib_Para[18] & 0x000000ff)<<8 ) + \
													 (u32)(Calib_Para[19] & 0x000000ff);

	calib_para.CTH_Para = 	 (u32)((Calib_Para[20] & 0x000000ff)<<24) + \
													 (u32)((Calib_Para[21] & 0x000000ff)<<16) + \
													 (u32)((Calib_Para[22] & 0x000000ff)<<8 ) + \
													 (u32)(Calib_Para[23] & 0x000000ff);	
													 
	calib_para.WARTM_Para =  (u32)((Calib_Para[24] & 0x000000ff)<<24) + \
													 (u32)((Calib_Para[25] & 0x000000ff)<<16) + \
													 (u32)((Calib_Para[26] & 0x000000ff)<<8 ) + \
													 (u32)(Calib_Para[27] & 0x000000ff);	
	
	
	WriteV9261fReg(WARTI,calib_para.WARTI_Para);		//ȫ������A��Чֵ�ıȲ�ֵ
	WriteV9261fReg(WARTU,calib_para.WARTU_Para);		//ȫ����ѹ��Чֵ�Ȳ�ֵ
	WriteV9261fReg(WAPT,calib_para.WAPT_Para);			//ȫ���й����ʱȲ�ֵ
	WriteV9261fReg(WWAPT,calib_para.WWAPT_Para);		//ȫ���й����ʶ��β���ֵ��ʧ��У����
	WriteV9261fReg(EGYTH,calib_para.EGYTH_Para);		//����������������ֵ
	WriteV9261fReg(CTH,calib_para.CTH_Para);				//������/Ǳ���ж�����ֵ
	//WriteV9261fReg(WARTM,calib_para.WARTM_Para);		//ȫ������B��Чֵ�ıȲ�ֵ
}

/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
static void WriteV9261fReg(u16 Addr,s32 Data)
{
	u8 i;
	
	//д��ͨ�˲���ϵ���Ĵ�����WriteRegBuf
	meterprotocol.WriteRegBuf[0] = 0xFE;
	meterprotocol.WriteRegBuf[1] = (u8)((Addr & 0x0f00)>>4) + RacWrite;
	meterprotocol.WriteRegBuf[2] = (u8)(Addr & 0x00ff);
	meterprotocol.WriteRegBuf[3] = (u8)(Data&0x000000ff);
	meterprotocol.WriteRegBuf[4] = (u8)((Data&0x0000ff00)>>8);
	meterprotocol.WriteRegBuf[5] = (u8)((Data&0x00ff0000)>>16);  	
	meterprotocol.WriteRegBuf[6] = (u8)((Data&0xff000000)>>24);   //�üĴ�����bit[8:0]����ϵͳ�����Ĺؼ���Ϣ
																									//��bit[24:16]�Ƕ�bit[8:0]�ı���
																									//Ϊ��ϵͳ�������������������Ƽ�ֵ��
	meterprotocol.WriteRegBuf[7] = 0x00;   //��ʼ������8��ֵ
	for(i = 0;i < 7;i++)
  {
     meterprotocol.WriteRegBuf[7] += meterprotocol.WriteRegBuf[i]; 
  }
	meterprotocol.WriteRegBuf[7] = (~meterprotocol.WriteRegBuf[7]) + 0x33;  //У��ֵ
	
	tls_uart_write(UART1, meterprotocol.WriteRegBuf, 8);//����д����
	
	tls_os_time_delay(20);//��ʱ200ms����֤���մ������
	
	//��ϵͳ���üĴ�����ReadRegBuf
	meterprotocol.ReadRegBuf[0] = 0xFE;
	meterprotocol.ReadRegBuf[1] = (u8)((Addr & 0x0f00)>>4) + RacRead;
	meterprotocol.ReadRegBuf[2] = (u8)(Addr & 0x00ff);
	meterprotocol.ReadRegBuf[3] = 0x01;
	meterprotocol.ReadRegBuf[4] = 0x00;
	meterprotocol.ReadRegBuf[5] = 0x00;  	
	meterprotocol.ReadRegBuf[6] = 0x00;   
	meterprotocol.ReadRegBuf[7] = 0x00;  //��ʼ������8��ֵ
	for(i = 0;i < 7;i++)
  {
     meterprotocol.ReadRegBuf[7] += meterprotocol.ReadRegBuf[i]; 
  }
	meterprotocol.ReadRegBuf[7] = (~meterprotocol.ReadRegBuf[7]) + 0x33;  //У��ֵ
	
	tls_uart_write(UART1, meterprotocol.ReadRegBuf, 8);//����д����
	
	tls_os_time_delay(20);//��ʱ200ms����֤��ȡ�������
	
	if(CheckRegDataWrited(meterprotocol.WriteRegBuf) == 0)
		printf("WriteRegBuf successed!!!\r\n");
	else
		printf("WriteRegBuf failed!!!\r\n");
}
/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
static u8 *ReadV9261fReg(u16 Addr)
{
	u8 i = 0;
	u8 *ReadDataBuf;
	
	//��ϵͳ���üĴ�����ReadRegBuf
	meterprotocol.ReadRegBuf[0] = 0xFE;
	meterprotocol.ReadRegBuf[1] = (u8)((Addr & 0x0f00)>>4) + RacRead;
	meterprotocol.ReadRegBuf[2] = (u8)(Addr & 0x00ff);
	meterprotocol.ReadRegBuf[3] = 0x01;
	meterprotocol.ReadRegBuf[4] = 0x00;
	meterprotocol.ReadRegBuf[5] = 0x00;  	
	meterprotocol.ReadRegBuf[6] = 0x00;   
	meterprotocol.ReadRegBuf[7] = 0x00;  //��ʼ������8��ֵ
	for(i = 0;i < 7;i++)
  {
     meterprotocol.ReadRegBuf[7] += meterprotocol.ReadRegBuf[i]; 
  }
	meterprotocol.ReadRegBuf[7] = (~meterprotocol.ReadRegBuf[7]) + 0x33;  //У��ֵ
	
	tls_uart_write(UART1, meterprotocol.ReadRegBuf, 8);//����д����
	
	tls_os_time_delay(20);//��ʱ200ms����֤��ȡ�������
	
	ReadDataBuf = meterprotocol.ReadRegBackBuf;
	
	return ReadDataBuf;
}
/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
static u8 CheckRegDataWrited(u8 *RegData)
{ 
	u8 i,err = 0;
	
	for(i = 3;i < 7;i++)
	{
		if(RegData[i] == meterprotocol.ReadRegBackBuf[i])
		{
			err += 0;
		}
		else
		{
			err += 1;
		}
		//printf("This is Regdata:%x\r\n",RegData[i]);
		//printf("This is Writedata:%x\r\n",meterprotocol.ReadRegBackBuf[i]);
	}
	
	if(err > 0) 
		return 1;
	else 
		return 0;
}

/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
void ReadVoltage(void)
{
	u8 i;
	u8 *MeterVoltage = NULL;
	u32 voltage=0;
	u32 err=0;
	
	MeterVoltage = ReadV9261fReg(AARTU);
	voltage = (u32)((MeterVoltage[6]&0x000000ff)<<24) + (u32)((MeterVoltage[5]&0x000000ff)<<16) + (u32)((MeterVoltage[4]&0x000000ff)<<8) + (u32)((MeterVoltage[3]&0x000000ff));
	voltage = voltage / 0x331B9;
	printf("Voltage is:%x\r\n",voltage);
}

/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
void ReadCurrent(void)
{
	u8 i;
	u8 *MeterCurrent = NULL;
	u32 current=0;
	u32 err=0;
	
	MeterCurrent = ReadV9261fReg(AARTI);
	current = (u32)((MeterCurrent[6]&0x000000ff)<<24) + (u32)((MeterCurrent[5]&0x000000ff)<<16) + (u32)((MeterCurrent[4]&0x000000ff)<<8) + (u32)((MeterCurrent[3]&0x000000ff));
	current = current / 0xda20;
	printf("Current is:%x\r\n",current);
}

/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
void ReadEnergy(void)
{
	u8 i;
	u8 *MeterEnergy = NULL;
	u32 energy=0;
	u32 err=0;
	
	MeterEnergy = ReadV9261fReg(AAP);
	energy = (u32)((MeterEnergy[6]&0x000000ff)<<24) + (u32)((MeterEnergy[5]&0x000000ff)<<16) + (u32)((MeterEnergy[4]&0x000000ff)<<8) + (u32)((MeterEnergy[3]&0x000000ff));
	energy = energy / 0xda20;
	printf("Energy is:%x\r\n",energy);
}

/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
u8 CalibVoltage(u8 StandardVoltage)
{
	u8 *MeterVoltage = NULL;
	u32 standardvoltage = 0;
	u32 voltage=0;
	s32 err=0,errRAM=0;
	u8  Voltage_err[4] = NULL;
	float precision = 0;
	
	MeterVoltage = ReadV9261fReg(AARTU);
	voltage = (u32)((MeterVoltage[6]&0x000000ff)<<24) + (u32)((MeterVoltage[5]&0x000000ff)<<16) + (u32)((MeterVoltage[4]&0x000000ff)<<8) + (u32)((MeterVoltage[3]&0x000000ff));
	//printf("voltage is:%x\r\n",voltage);
	standardvoltage = StandardVoltage * 2093372;
	//printf("standardvoltage is:%x\r\n",standardvoltage);
	err = standardvoltage - voltage;
	
	errRAM	= err + calib_para.WARTU_Para;	//0x1B734F8D;
	printf("err is:0x%x\r\n",err);
	
	Voltage_err[0] = (u8)((errRAM & 0xff000000)>>24);
	Voltage_err[1] = (u8)((errRAM & 0x00ff0000)>>16);
	Voltage_err[2] = (u8)((errRAM & 0x0000ff00)>>8);
	Voltage_err[3] = (u8)( errRAM & 0x000000ff);
	
	AT24CXX_Write(0x14,Voltage_err,4);
	
	precision = (float)(err/standardvoltage);
	printf("precision is:%f03\r\n",precision);
	if(fabs(precision) < 0.01)
		return 0;
	else
		return 1;
}

/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
u8 CalibCurrent(u8 StandardCurrent)
{
	u8 *MeterCurrent = NULL;
	u32 standardcurrent = 0;
	u32 current=0;
	s32 err=0,errRAM=0;
	u8  Current_err[4] = NULL;
	float precision = 0;
	
	MeterCurrent = ReadV9261fReg(AARTI);
	current = (u32)((MeterCurrent[6]&0x000000ff)<<24) + (u32)((MeterCurrent[5]&0x000000ff)<<16) + (u32)((MeterCurrent[4]&0x000000ff)<<8) + (u32)((MeterCurrent[3]&0x000000ff));
	//printf("voltage is:%x\r\n",voltage);
	standardcurrent = StandardCurrent * 55840000;
	//printf("standardvoltage is:%x\r\n",standardvoltage);
	err = standardcurrent - current;
	
	errRAM	= err + calib_para.WARTI_Para;
	printf("err is:0x%x\r\n",err);
	
	Current_err[0] = (u8)((errRAM & 0xff000000)>>24);
	Current_err[1] = (u8)((errRAM & 0x00ff0000)>>16);
	Current_err[2] = (u8)((errRAM & 0x0000ff00)>>8);
	Current_err[3] = (u8)( errRAM & 0x000000ff);
	
	AT24CXX_Write(0x10,Current_err,4);
	
	precision = (float)(err/standardcurrent);
	printf("precision is:%f03\r\n",precision);
	if(fabs(precision) < 0.01)
		return 0;
	else
		return 1;
}

/*************************************************************************** 
* Description:
*
* Auth: 
*
*Date: 
****************************************************************************/ 
void Meter_init(void)
{
	u8 i=0;
	
	MeterControlRegPara_init();		//���ƼĴ���������ʼ��
	MeterErrorRegPara_init();			//У��Ĵ�������д���ʼ��
	
	WriteV9261fReg(MeterControlReg0Add,0xEC000080);		//WriteMeterControlReg0
}

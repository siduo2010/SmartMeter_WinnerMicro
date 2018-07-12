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

#define       LCD_ON      0x03    
#define       LCD_OFF     0x02   
#define       RC32K       0x18
#define       SYS_EN      0x01
#define       Mode        0xE3
#define       DATA        WM_IO_PA_01
#define       WR          WM_IO_PB_06
#define       CS          WM_IO_PB_07
#define       BG          WM_IO_PB_09
#define       COM0        0
#define       COM1        1
#define       COM2        2
#define       COM3        3
#define       COM4        4
#define       COM5        5
#define       COM6        6
#define       COM7        7
u8         dispram[32];
static u8 gsBlinkStatus = 0;		// wifi ×´Ì¬Ö¸Ê¾µÆ

void  LCD_GPIO(void)
{
/*  PC_DDR = 0xb0;
    PC_CR1 = 0xb0;
    PC_ODR = 0xbf;
*/
		tls_gpio_cfg(WR, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLLOW); 
		tls_gpio_cfg(CS, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLLOW); 
		tls_gpio_cfg(DATA, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLLOW); 
	
		tls_gpio_cfg(BG, WM_GPIO_DIR_OUTPUT, WM_GPIO_ATTR_PULLLOW); 
}

void SendBit_HL(u8 dat,u8 cnt)
{
    u8 i;
    for(i=0; i<cnt;i++)
    {
        if(dat&0x80)
        {
            //DATA = 1;
						tls_gpio_write(DATA,1);
        }
        else
        {
            //DATA = 0;
						tls_gpio_write(DATA,0);
        }
        //WR = 0;
				tls_gpio_write(WR,0);
        asm("nop");
        asm("nop");
        //WR = 1;
				tls_gpio_write(WR,1);
        dat <<= 1;
    }
} 

void  SendBit_LH(u8  dat,u8 cnt)    
{
    u8 i;
    for(i=0; i<cnt;i++)
    {
        if(dat&0x01)
        {
            //DATA = 1;
						tls_gpio_write(DATA,1);
        }
        else
        {
            //DATA = 0;
						tls_gpio_write(DATA,0);
        }
        //WR = 0;
				tls_gpio_write(WR,0);
        asm("nop");
        asm("nop");
        //WR = 1;
				tls_gpio_write(WR,1);
        dat >>= 1;
    }
}
//????
void  Sendcmd(u8  command)
{
    //CS = 0;
		tls_gpio_write(CS,0);
    SendBit_HL(0x80,3);  
    SendBit_HL(command,9);  
    //CS = 1;
		tls_gpio_write(CS,1);
}
//?????
void W1622_q(u8 addr,u8 dat)
{
    //CS = 0;
		tls_gpio_write(CS,0);
    SendBit_HL(0xa0,3);  
    SendBit_HL(addr<<2,6); 
    SendBit_LH(dat,4);
    //CS = 1;
		tls_gpio_write(CS,1);
}
//?SEG??????,SEG( 0~31 )
void  W1622_byte(u8 SEG,u8 dat)
{
    u8 i,addr;
    addr = SEG * 2;
    for(i=0;i<2;i++)
    {
        W1622_q(addr,dat);
        addr += 1;
        dat >>= 4;
    }
}
//????? SEG(0~31)
void W1622_success(u8 SEG,u8 cnt,u8 *p)
{
    u8 i,addr;
    addr = SEG * 2;
    //CS = 0;
		tls_gpio_write(CS,0);
    SendBit_HL(0xa0,3);  
    SendBit_HL(addr<<2,6); 
    for(i =0; i<8;i++)
    {
        SendBit_LH(*p,4);
        SendBit_LH(*p>>4,4);
    }
    //CS = 1;
		tls_gpio_write(CS,1);
}
void disp_renew(void)   
{
    W1622_success(0,32,dispram);
}
//com(0-7)  SEG(0-31)
void  dispON(u8 com,u8 SEG)
{
    dispram[SEG] |= (1<<com);
    W1622_byte(SEG,dispram[SEG]);     
}
//com(0-7)  SEG(0-31)
void  dispOFF(u8 com,u8 SEG)
{
    dispram[SEG] &= ~(1<<com);
    W1622_byte(SEG,dispram[SEG]);    
}
void  cleandisp(void)
{
    u8  i;
    //CS = 0; 
		tls_gpio_write(CS,0);
    SendBit_HL(0xa0,3);  
    SendBit_HL(0,6); 
    for(i =0; i <32; i ++) 
    {
        SendBit_LH(0,4);
        SendBit_LH(0,4);
    }
    //CS = 1;
		tls_gpio_write(CS,1);
    for(i=0; i<32; i++)
    {
        dispram[i] = 0x00;
    }
}
void  disp_All(void)
{
    u8  i;
    //CS = 0;
		tls_gpio_write(CS,0);
    SendBit_HL(0xa0,3);
    SendBit_HL(0,6); 
    for(i =0; i <32; i ++) 
    {
        SendBit_LH(0xff,4);
        SendBit_LH(0xff,4);
    }
    //CS = 1;
		tls_gpio_write(CS,1);
    for(i=0; i<32; i++)
    {
        dispram[i] = 0xff;    
    }
}
void HT1622_Init(void)
{
    u8  j = 0;
		tls_gpio_write(BG,1);
    for(j=0; j<32; j++)
    {
        dispram[j] = 0;
    }
    Sendcmd(RC32K);   
    //Sendcmd(Mode);
		Sendcmd(SYS_EN);  
    Sendcmd(LCD_ON);  
}

void show1num(u8 num)
{
	u8 getNum = num;
	u8 pin = 22;
	switch(getNum)
	{
		case 0:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 1:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 2:dispON(2,pin);dispON(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 3:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 4:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 5:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 6:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 7:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 8:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 9:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 10:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 11:dispOFF(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 12:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 13:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 14:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 15:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 16:dispOFF(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;

	}
}

void show2num(u8 num)
{
	u8 getNum = num;
	u8 pin = 20;
	switch(getNum)
	{
		case 0:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 1:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 2:dispON(2,pin);dispON(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 3:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 4:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 5:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 6:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 7:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 8:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 9:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 10:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 11:dispOFF(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 12:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 13:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 14:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 15:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 16:dispOFF(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;

	}
}
void show3num(u8 num)
{
	u8 getNum = num;
	u8 pin = 18;
	switch(getNum)
	{
		case 0:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 1:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 2:dispON(2,pin);dispON(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 3:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 4:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 5:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 6:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 7:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 8:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 9:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 10:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 11:dispOFF(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 12:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 13:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 14:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 15:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 16:dispOFF(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;

	}
}
void show4num(u8 num)
{
	u8 getNum = num;
	u8 pin = 16;
	switch(getNum)
	{
		case 0:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 1:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 2:dispON(2,pin);dispON(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 3:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 4:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 5:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 6:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 7:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 8:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 9:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 10:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 11:dispOFF(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 12:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 13:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 14:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 15:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 16:dispOFF(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;

	}
}
void show5num(u8 num)
{
	u8 getNum = num;
	u8 pin = 14;
	switch(getNum)
	{
		case 0:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 1:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 2:dispON(2,pin);dispON(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 3:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 4:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 5:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 6:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 7:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispOFF(3,pin+1);break;
		case 8:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 9:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispOFF(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 10:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 11:dispOFF(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 12:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispOFF(3,pin+1);break;
		case 13:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;
		case 14:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 15:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispON(4,pin+1);dispON(2,pin+1);dispON(3,pin+1);break;
		case 16:dispOFF(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin+1);dispOFF(4,pin+1);dispOFF(2,pin+1);dispON(3,pin+1);break;

	}
}
void show6num(u8 num)
{
	u8 getNum = num;
	u8 pin = 7;
	switch(getNum)
	{
		case 0:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispOFF(3,pin-1);break;
		case 1:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispOFF(3,pin-1);break;
		case 2:dispON(2,pin);dispON(3,pin);dispOFF(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;
		case 3:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;
		case 4:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 5:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin-1);dispOFF(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 6:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 7:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispOFF(3,pin-1);break;
		case 8:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 9:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispOFF(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 10:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 11:dispOFF(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 12:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispOFF(3,pin-1);break;
		case 13:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;
		case 14:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 15:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 16:dispOFF(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;

	}
}
void show7num(u8 num)
{
	u8 getNum = num;
	u8 pin = 9;
	switch(getNum)
	{
		case 0:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispOFF(3,pin-1);break;
		case 1:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispOFF(3,pin-1);break;
		case 2:dispON(2,pin);dispON(3,pin);dispOFF(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;
		case 3:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;
		case 4:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 5:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin-1);dispOFF(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 6:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 7:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispOFF(3,pin-1);break;
		case 8:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 9:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispOFF(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 10:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 11:dispOFF(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 12:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispOFF(3,pin-1);break;
		case 13:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;
		case 14:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 15:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 16:dispOFF(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;

	}
}
void show8num(u8 num)
{
	u8 getNum = num;
	u8 pin = 11;
	switch(getNum)
	{
		case 0:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispOFF(3,pin-1);break;
		case 1:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispOFF(3,pin-1);break;
		case 2:dispON(2,pin);dispON(3,pin);dispOFF(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;
		case 3:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;
		case 4:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 5:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin-1);dispOFF(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 6:dispON(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 7:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispOFF(3,pin-1);break;
		case 8:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 9:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispOFF(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 10:dispON(2,pin);dispON(3,pin);dispON(4,pin);dispOFF(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 11:dispOFF(2,pin);dispOFF(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 12:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispOFF(3,pin-1);break;
		case 13:dispOFF(2,pin);dispON(3,pin);dispON(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;
		case 14:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispON(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 15:dispON(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin-1);dispON(4,pin-1);dispON(2,pin-1);dispON(3,pin-1);break;
		case 16:dispOFF(2,pin);dispOFF(3,pin);dispOFF(4,pin);dispOFF(5,pin-1);dispOFF(4,pin-1);dispOFF(2,pin-1);dispON(3,pin-1);break;

	}
}
void show9num(u8 num)
{
	u8 getNum = num;
	u8 pin = 1;
	switch(getNum)
	{
		case 0:dispON(1,pin);dispON(3,5);dispON(6,pin);dispON(5,pin);dispON(4,pin);dispON(2,pin);dispOFF(3,pin);break;
		case 1:dispOFF(1,pin);dispON(3,5);dispON(6,pin);dispOFF(5,pin);dispOFF(4,pin);dispOFF(2,pin);dispOFF(3,pin);break;
		case 2:dispON(1,pin);dispON(3,5);dispOFF(6,pin);dispON(5,pin);dispON(4,pin);dispOFF(2,pin);dispON(3,pin);break;
		case 3:dispON(1,pin);dispON(3,5);dispON(6,pin);dispON(5,pin);dispOFF(4,pin);dispOFF(2,pin);dispON(3,pin);break;
		case 4:dispOFF(1,pin);dispON(3,5);dispON(6,pin);dispOFF(5,pin);dispOFF(4,pin);dispON(2,pin);dispON(3,pin);break;
		case 5:dispON(1,pin);dispOFF(3,5);dispON(6,pin);dispON(5,pin);dispOFF(4,pin);dispON(2,pin);dispON(3,pin);break;
		case 6:dispON(1,pin);dispOFF(3,5);dispON(6,pin);dispON(5,pin);dispON(4,pin);dispON(2,pin);dispON(3,pin);break;
		case 7:dispON(1,pin);dispON(3,5);dispON(6,pin);dispOFF(5,pin);dispOFF(4,pin);dispOFF(2,pin);dispOFF(3,pin);break;
		case 8:dispON(1,pin);dispON(3,5);dispON(6,pin);dispON(5,pin);dispON(4,pin);dispON(2,pin);dispON(3,pin);break;
		case 9:dispON(1,pin);dispON(3,5);dispON(6,pin);dispON(5,pin);dispOFF(4,pin);dispON(2,pin);dispON(3,pin);break;
		case 10:dispON(1,pin);dispON(3,5);dispON(6,pin);dispOFF(5,pin);dispON(4,pin);dispON(2,pin);dispON(3,pin);break;
		case 11:dispOFF(1,pin);dispOFF(3,5);dispON(6,pin);dispON(5,pin);dispON(4,pin);dispON(2,pin);dispON(3,pin);break;
		case 12:dispON(1,pin);dispOFF(3,5);dispOFF(6,pin);dispON(5,pin);dispON(4,pin);dispON(2,pin);dispOFF(3,pin);break;
		case 13:dispOFF(1,pin);dispON(3,5);dispON(6,pin);dispON(5,pin);dispON(4,pin);dispOFF(2,pin);dispON(3,pin);break;
		case 14:dispON(1,pin);dispOFF(3,5);dispOFF(6,pin);dispON(5,pin);dispON(4,pin);dispON(2,pin);dispON(3,pin);break;
		case 15:dispON(1,pin);dispOFF(3,5);dispOFF(6,pin);dispOFF(5,pin);dispON(4,pin);dispON(2,pin);dispON(3,pin);break;

	}
}
void showLightMH(void)
{
		dispON(1,16);
		dispON(5,16);
}
void showRightMH(void)
{
		dispON(5,7);
		dispON(1,7);
}
void showClrtime(void)
{
	dispOFF(2,22);dispOFF(3,22);dispOFF(4,22);dispOFF(5,23);dispOFF(4,23);dispOFF(2,23);dispOFF(3,23);
	dispOFF(2,20);dispOFF(3,20);dispOFF(4,20);dispOFF(5,21);dispOFF(4,21);dispOFF(2,21);dispOFF(3,21);
	dispOFF(2,18);dispOFF(3,18);dispOFF(4,18);dispOFF(5,19);dispOFF(4,19);dispOFF(2,19);dispOFF(3,19);
	dispOFF(2,16);dispOFF(3,16);dispOFF(4,16);dispOFF(5,17);dispOFF(4,17);dispOFF(2,17);dispOFF(3,17);
	dispOFF(2,14);dispOFF(3,14);dispOFF(4,14);dispOFF(5,15);dispOFF(4,15);dispOFF(2,15);dispOFF(3,15);
	dispOFF(2,7);dispOFF(3,7);dispOFF(4,7);dispOFF(5,6);dispOFF(4,6);dispOFF(2,6);dispOFF(3,6);
	dispOFF(2,9);dispOFF(3,9);dispOFF(4,9);dispOFF(5,8);dispOFF(4,8);dispOFF(2,8);dispOFF(3,8);
	dispOFF(2,11);dispOFF(3,11);dispOFF(4,11);dispOFF(5,10);dispOFF(4,10);dispOFF(2,10);dispOFF(3,10);
	
	dispOFF(5,22);dispOFF(5,20);dispOFF(5,18);dispOFF(5,16);dispOFF(5,14);
	dispOFF(5,7);dispOFF(5,9);dispOFF(1,16);dispOFF(1,7);
}
void Show_time_flag(void)
{
		printf("This is the LCD first screen!\r\n");  
		dispON(1,9);
		dispON(1,10);  
}
void Show_time(u8 Hour,u8 Minute,u8 Sencond)
{
		u8 sencondH,minuteH,hourH;
		u8 sencondL,minuteL,hourL;
		
		showClrtime();
	
		sencondH = Sencond/10;
		sencondL = Sencond%10;
	
		minuteH = Minute/10;
		minuteL = Minute%10;
	
		hourH = Hour/10;
		hourL = Hour%10;
		
		show3num(hourH);
		show4num(hourL);
		showLightMH();		//:
		show5num(minuteH);
		show6num(minuteL);
		showRightMH();		//:
		show7num(sencondH);
		show8num(sencondL);
}
void Show_date(u8 year,u8 month,u8 day)
{
		u8 dayH,monthH,yearH;
		u8 dayL,monthL,yearL;
		
		showClrtime();
	
		dayH = day/10;
		dayL = day%10;
	
		monthH = month/10;
		monthL = month%10;
	
		yearH = year/10;
		yearL = year%10;
		
		show1num(yearH);
		show2num(yearL);
		show3num(16);		//:
		show4num(monthH);
		show5num(monthL);
		show6num(16);		//:
		show7num(dayH);
		show8num(dayL);
}
//
u8 HexToDec(u8 hex)
{
		u8 dec,Hhex,Lhex;
	
		Hhex = (hex&0xf0)>>4;
		Lhex = (hex&0x0f);
		dec = Hhex*10+Lhex;
	
		return dec;
}
/*************************************************************************** 
* Description: 
*
* Auth: 
*
*Date:  2018-3-30
****************************************************************************/ 
void UserLcdCtrl(u8 status)
{
	if(status)
		dispON(1,21);
	else
		dispOFF(1,21);
}
/*************************************************************************** 
* Description: 
*
* Auth: 
*
*Date:  2018-3-30
****************************************************************************/ 
void UserLcdBlink(u8 status)
{
	gsBlinkStatus = status;
}
/***********************************************************************
* Description : LEDµÆÉÁË¸¿ØÖÆ
*
* Auth: 
*
*Date:  2018-3-30
 ***********************************************************************/
void UserLcdBlinkProc(void)
{
	static u8 div_blink = 0;
	static u8 mode_blink = 0;
	u8 mode = 0;
	u8 blink[][2] = {{10, 5}, {2, 1}};

	if(0 == gsBlinkStatus)
	{
		return;
	}
	switch(gsBlinkStatus)
	{
		case 1:
			mode = 0;
			break;
			
		case 2:	
			mode = 1;
			break;
	}
	if(mode != mode_blink)
	{
		mode_blink = mode;
		div_blink = 0;
	}
	if(blink[mode_blink][0] == 0)
	{
		if (blink[mode_blink][1] == 0)
		{
			dispOFF(1,21);
		}
		else
		{
			dispON(1,21);
		}
	}
	else
	{
		++div_blink;
		if (div_blink <= blink[mode_blink][1])
		{
			dispON(1,21);
		}
		else if (div_blink <= blink[mode_blink][0])
		{
			dispOFF(1,21);
		}
		else
		{
			div_blink = 0;
		}
	}
}

void LCD_init(void)
{
	LCD_GPIO();
  HT1622_Init();
  cleandisp(); 
}
//LCD
int LCD_display(void)
{	
	Time_Def LCDReadTime;

	for(;;)
	{
		Read_time(&LCDReadTime);
		Show_time_flag();
		Show_time(HexToDec(LCDReadTime.hour),HexToDec(LCDReadTime.minute),HexToDec(LCDReadTime.second));
		tls_os_time_delay(0x00000100);
		Show_date(HexToDec(LCDReadTime.year),HexToDec(LCDReadTime.month),HexToDec(LCDReadTime.day));
		tls_os_time_delay(0x00000100);
	}

	return WM_SUCCESS;
}

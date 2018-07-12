/*****************************************************************************
*
* File Name : wm_adc.c
*
* Description: adc Driver Module
*
* Copyright (c) 2014 Winner Microelectronics Co., Ltd.
* All rights reserved.
*
* Author : dave
*
* Date : 2014-8-15
*****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_irq.h"
#include "wm_regs.h"
#include "wm_adc.h"
#include "wm_dma.h"
#include "wm_io.h"


ST_ADC gst_adc;

static void ADC_IRQHandler(void)
{
	u16 adcvalue;
	int reg;
	
	reg = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	if(reg & CONFIG_ADC_INT)      //ADC中断
	{
	    tls_adc_clear_irq(ADC_INT_TYPE_ADC);
	    adcvalue = tls_read_adc_result();
	    if(gst_adc.adc_cb)
			gst_adc.adc_cb(&adcvalue,1);
	}
	if(reg & CONFIG_ADC_INT_CMP)		//ADC比较模式
	{
	    tls_adc_clear_irq(ADC_INT_TYPE_ADC_CMP);
	    if(gst_adc.adc_cmp_cb)
			gst_adc.adc_cmp_cb(NULL, 0);
	}
}

static void adc_dma_isr(void *arg)
{
	if(gst_adc.adc_dma_cb)
		gst_adc.adc_dma_cb((u16 *)(ADC_DEST_BUFFER_DMA), gst_adc.valuelen);	
}


/**
 * @brief          	This function is used to initial adc
 *
 * @param[in]      	ifusedma		whether or not use dma
 * @param[in]      	dmachannel	choose the dma channel  0~7
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_init(u8 ifusedma, u8 dmachannel)
{
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, 0);

	tls_irq_enable(ADC_INT);

	//注册中断和channel有关，所以需要先请求
	if(ifusedma)
	{
		gst_adc.dmachannel = tls_dma_request(dmachannel, NULL);	//请求dma，不要直接指定，因为请求的dma可能会被别的任务使用
		tls_dma_irq_register(gst_adc.dmachannel, adc_dma_isr, NULL, TLS_DMA_IRQ_TRANSFER_DONE);
	}
}

/**
 * @brief          	This function is used to clear adc interrupt status
 *
 * @param[in]      	inttype	the adc interrupt type
 *				
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_clear_irq(int inttype)
{
	int reg;
	reg = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	if(ADC_INT_TYPE_ADC == inttype)
	{
	    reg |= CONFIG_ADC_INT;
	    tls_reg_write32(HR_SD_ADC_CONFIG_REG, reg);
	}
	else if(ADC_INT_TYPE_ADC_CMP== inttype)
	{
	    reg |= CONFIG_ADC_INT_CMP;
	    tls_reg_write32(HR_SD_ADC_CONFIG_REG, reg);
	}
	else if(ADC_INT_TYPE_DMA == inttype)
	{
	    tls_dma_irq_clr(gst_adc.dmachannel, TLS_DMA_IRQ_TRANSFER_DONE);
	}

}

/**
 * @brief           	This function is used to register the adc interrupt callback function
 *
 * @param[in]      	inttype           the adc interrupt type
 *				ADC_INT_TYPE_ADC		adc interrupt,user get adc result from the callback function.
 *				ADC_INT_TYPE_DMA		dma interrupt,dma transfer the adc result to the user's buffer.
 * @param[in]      	callback         the adc interrupt callback function
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_irq_register(int inttype, void (*callback)(u16 *buf, u16 len))
{
	if(ADC_INT_TYPE_ADC == inttype)
	{
		gst_adc.adc_cb = callback;
	}
	else if(ADC_INT_TYPE_DMA == inttype)
	{
		gst_adc.adc_dma_cb = callback;
	}
	else if(ADC_INT_TYPE_ADC_CMP == inttype)
	{
	    gst_adc.adc_cmp_cb = callback;
	}
}

/**
 * @brief         	This function is used to read adc convert result
 *
 * @param		None	
 *
 * @retval       	adc result[0~16383]
 *
 * @note           	None
 */
u16 tls_read_adc_result(void)
{
	u32 value;
	u16 ret;

	value = tls_reg_read32(HR_SD_ADC_RESULT_REG);
	ret = value&0x3fff;

	return ret;
}


/**
 * @brief         	This function is used to set adc io config
 *
 * @@param[in]    	Channel          adc channel,from 0 to 8 is single input; 8 to 11 is differential input.
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_io_config(int Channel)
{
        switch(Channel)
        {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                tls_io_cfg_set((enum tls_io_name)(WM_IO_PB_19+Channel), WM_IO_OPTION6);
                break;
            case 8:
            case 9:
            case 10:
            case 11:
                tls_io_cfg_set((enum tls_io_name)(WM_IO_PB_19+(Channel-8)*2), WM_IO_OPTION6);
                tls_io_cfg_set((enum tls_io_name)(WM_IO_PB_19+(Channel-8)*2+1), WM_IO_OPTION6);
                break;

            default:
                break;                
        }
}


/**
 * @brief         	This function is used to start adc convert with cpu mode
 *
 * @@param[in]    	Channel          adc channel,from 0 to 8 is single input; 8 to 11 is differential input.
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_start_with_cpu(int Channel)
{
	u32 value;
	
	Channel &= CONFIG_ADC_CHL_MASK;
        
	/* Stop adc first */
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_DMA_MASK;
	value &= ~CONFIG_ADC_START;
	value |= Channel;

	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_START;
	value |= CONFIG_ADC_INT_MASK;
	if(Channel < 8)
	{
		value |= CONFIG_ADC_G_CTRL12;
		value &= ~ CONFIG_ADC_VCM(0x3Ful);
		value |= CONFIG_ADC_VCM(0x1Ful);					//单端1.6V为中心点
	}
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);		/*start adc*/
}


/**
 * @brief         	This function is used to start adc convert with dma mode
 *
 * @param[in]    	Channel           adc channel,from 0 to 8 is single input; 8 to 11 is differential input.
 * @param[in]    	Length             need convert adc data length
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_start_with_dma(int Channel, int Length)
{
	u32 value;
	int len;

	if(Channel < 0 || Channel > 11)
		return;
        
	if(Length > ADC_DEST_BUFFER_SIZE)
		len = ADC_DEST_BUFFER_SIZE;
	else
		len = Length;

	gst_adc.valuelen = Length;

	Channel &= CONFIG_ADC_CHL_MASK;

	/* Stop adc first */
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_START;       //stop
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	
	/* Stop dma if necessary */
	while(DMA_CHNLCTRL_REG(gst_adc.dmachannel) & 1)
	{
		DMA_CHNLCTRL_REG(gst_adc.dmachannel) = 2;
	}

	DMA_SRCADDR_REG(gst_adc.dmachannel) = HR_SD_ADC_RESULT_REG;
	DMA_DESTADDR_REG(gst_adc.dmachannel) = ADC_DEST_BUFFER_DMA;
	/* Hard, Normal, adc_req */
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);		
	if (Channel == 8){
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (0+6)<<2);
		value |= (0x1 << 11); 		
	}
	else if (Channel == 9){
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (2+6)<<2);
		value |= (0x1 << 13); 		
	}
	else if (Channel == 10){
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (4+6)<<2);
		value |= (0x1 << 15); 		
	}
	else if (Channel == 11){
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (6+6)<<2);
		value |= (0x1 << 17); 		
	}
	else{
		DMA_MODE_REG(gst_adc.dmachannel) = (0x01 | (Channel+6)<<2);
		value |= (0x1 << (11 + Channel)); 		
	}	
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	/* Dest_add_inc, halfword,  */
	DMA_CTRL_REG(gst_adc.dmachannel) = (1<<3)|(1<<5)|((len*2)<<8);
	DMA_INTMASK_REG &= ~(0x01 << (gst_adc.dmachannel *2 + 1));
	DMA_CHNLCTRL_REG(gst_adc.dmachannel) = 1;		/* Enable dma */

	//tls_reg_write32(HR_SD_ADC_CONFIG_REG, Channel);

	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_START;
	value |= Channel;
	if(Channel < 8)
	{
		value |= CONFIG_ADC_G_CTRL12;
		value &= ~ CONFIG_ADC_VCM(0x3Ful);
		value |= CONFIG_ADC_VCM(0x1Ful);	
	}
	printf("config value==%x\n", value);
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);		/*start adc*/
}


/**
 * @brief         	This function is used to stop adc convert
 *
 * @param[in]    	ifusedma     	whether or not use dma	
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_stop(int ifusedma)
{
	u32 value;

	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_START;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);

	if(ifusedma)
		tls_dma_free(gst_adc.dmachannel);
}

/**
 * @brief         	This function is used to config adc compare mode
 *
 * @param[in]    	cmp_data     	adc compare value	
 * @param[in]    	cmp_pol     	(0)  bigger than cmp_data interrupt  (other) littler than cmp_data interrupt
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_config_cmp_reg(int cmp_data, int cmp_pol)
{
    int value;

    value = tls_reg_read32(HR_SD_ADC_RESULT_REG);
    value |= (cmp_data<<14);
    tls_reg_write32(HR_SD_ADC_RESULT_REG, value);

    value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
    value |= CONFIG_ADC_CMP_INT_MASK;
	if(cmp_pol)
	{
		value |= CONFIG_ADC_CMP_POL;
	}
	else
	{
		value &= ~CONFIG_ADC_CMP_POL;
	}
    tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
}

/**
 * @brief         	This function is used to choose adc reference source
 *
 * @param[in]    	ref    (0) choose the reference internal  	(other) choose the reference inside external
 *
 * @return         	None
 *
 * @note           	None
 */
void tls_adc_reference_sel(int ref)
{
    int value;
    
    value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
    if(ADC_REFERENCE_EXTERNAL == ref)
    {
        value &= ~CONFIG_ADC_REF_SEL;
		value |= CONFIG_ADC_BUF_BYPASS;
    }
    else
    {
        value |= CONFIG_ADC_REF_SEL;
		value &= ~CONFIG_ADC_BUF_BYPASS;
    }
    tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);    
}


/**
 * @brief         	This function is used to star adc calibration
 *
 * @param		None	
 *
 * @return       	None
 *
 * @note           	None
 */
void tls_adc_calibration_start(void)
{
	u32 value;
        
	/* Stop adc first */
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_DMA_MASK;
	value &= ~CONFIG_ADC_START;
	value |= CONFIG_ADC_CHL_OFFSET;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_START;
	value |= CONFIG_ADC_INT_MASK;
	value |= CONFIG_ADC_EN_CAL;
	value |= CONFIG_ADC_G_CTRL12; 
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);		/*start adc*/
}


/**
 * @brief         	This function is used to stop adc calibration
 *
 * @param		None	
 *
 * @return       	None
 *
 * @note           	None
 */
void tls_adc_calibration_stop(void)
{
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, 0);
}


/**
 * @brief         	This function is used to start test VCC  voltage
 *
 * @param		None	
 *
 * @return       	None
 *
 * @note           	None
 */
void tls_adc_voltage_start(void)
{
	u32 value;
        
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value &= ~CONFIG_ADC_DMA_MASK;
	value &= ~CONFIG_ADC_START;
	value |= CONFIG_ADC_CHL_VOLT;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);
	
	value = tls_reg_read32(HR_SD_ADC_CONFIG_REG);
	value |= CONFIG_ADC_START;
	value |= CONFIG_ADC_INT_MASK;
	value |= CONFIG_ADC_G_CTRL12;
	tls_reg_write32(HR_SD_ADC_CONFIG_REG, value);		/*start adc*/
}




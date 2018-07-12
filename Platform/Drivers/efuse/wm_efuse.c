/**************************************************************************
 * File Name                   : tls_efuse.c
 * Author                       :
 * Version                      :
 * Date                          :
 * Description                 : Use Flash Addr as virtual efuse
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd. 
 * All rights reserved.
 *
 ***************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wm_debug.h"
#include "wm_regs.h"
#include "wm_efuse.h"
#include "wm_config.h"
#include "list.h"
#include "wm_internal_flash.h"
#include "wm_crypto_hard.h"



#define FLASH_BASE_ADDR			0x8000000UL
#define FT_MAGICNUM_ADDR		(FLASH_BASE_ADDR)
#define MAGICNUM_LEN			(4)
#define FT_CHECKSUM_ADDR		(FT_MAGICNUM_ADDR + MAGICNUM_LEN)
#define CHECKSUM_LEN			(4)
#define FT_DATALEN_ADDR			(FT_CHECKSUM_ADDR + CHECKSUM_LEN)
#define FT_DATALEN_LEN			(4)
#define FT_MAC_ADDR        			(FT_DATALEN_ADDR  + FT_DATALEN_LEN)
#define MAC_ADDR_LEN			(8)
#define FT_TX_DC_OFFSET_ADDR	(FT_MAC_ADDR + MAC_ADDR_LEN)
#define TX_DC_OFFSET_LEN		(4)
#define FT_RX_DC_OFFSET_ADDR	(FT_TX_DC_OFFSET_ADDR + TX_DC_OFFSET_LEN)
#define RX_DC_OFFSET_LEN		(4)
#define FT_TX_IQ_GAIN_ADDR		(FT_RX_DC_OFFSET_ADDR + RX_DC_OFFSET_LEN)
#define TX_IQ_GAIN_LEN			(4)
#define FT_RX_IQ_GAIN_ADDR		(FT_TX_IQ_GAIN_ADDR + TX_IQ_GAIN_LEN)
#define RX_IQ_GAIN_LEN			(4)
#define FT_TX_IQ_PHASE_ADDR		(FT_RX_IQ_GAIN_ADDR + RX_IQ_GAIN_LEN)
#define TX_IQ_PHASE_LEN			(4)
#define FT_RX_IQ_PHASE_ADDR		(FT_TX_IQ_PHASE_ADDR + TX_IQ_PHASE_LEN)
#define RX_IQ_PHASE_LEN			(4)
#define FT_GAIN_ADDR        			(FT_RX_IQ_PHASE_ADDR  + RX_IQ_PHASE_LEN)
#define FT_GAIN_LEN				(84)
#define FT_FIX_DATA_LEN (MAC_ADDR_LEN + TX_DC_OFFSET_LEN + RX_DC_OFFSET_LEN + TX_IQ_GAIN_LEN + RX_IQ_GAIN_LEN \
	+ TX_IQ_PHASE_LEN \
	+ RX_IQ_PHASE_LEN)
#define SIGNATURE_WORD       0xA0FFFF9F


typedef struct FT_PARAM
{
	unsigned int		magic_no;
	unsigned int 		checksum;
	unsigned int		data_len;
	unsigned char		mac_addr[MAC_ADDR_LEN];
	unsigned int		tx_dcoffset;
	unsigned int		rx_dcoffset;	
	unsigned int		tx_iq_gain;
	unsigned int		rx_iq_gain;	
	unsigned int		tx_iq_phase;
	unsigned int		rx_iq_phase;	
	unsigned char		tx_gain[FT_GAIN_LEN];
}FT_PARAM_ST;



int tls_ft_param_get(unsigned int opnum, void *data, unsigned int rdlen)
{
	FT_PARAM_ST ft;

	tls_fls_read(FT_MAGICNUM_ADDR, (unsigned char *)&ft, sizeof(ft));
	switch (opnum)
	{
		case CMD_MAC:	/*MAC*/
			memcpy(data, ft.mac_addr, MAC_ADDR_LEN);
		break;
		
		case CMD_TX_DC: /*tx_dcoffset*/
			*(unsigned int *)data = ft.tx_dcoffset;
		break;	
		
		case CMD_RX_DC: /*rx_dcoffset*/
			*(unsigned int *)data = ft.rx_dcoffset;
		break;	
		
		case CMD_TX_IQ_GAIN:
			*(unsigned int *)data = ft.tx_iq_gain;
		break;	
		
		case CMD_RX_IQ_GAIN:
			*(unsigned int *)data = ft.rx_iq_gain;
		break;
		
		case CMD_TX_IQ_PHASE:
			*(unsigned int *)data = ft.tx_iq_phase;
		break;	
		
		case CMD_RX_IQ_PHASE:
			*(unsigned int *)data = ft.rx_iq_phase;
		break;
		
		case CMD_TX_GAIN: /*gain*/
			if (rdlen < FT_GAIN_LEN)
			{
				memcpy(data, ft.tx_gain, rdlen);
			}
			else
			{
				memcpy(data, ft.tx_gain, FT_GAIN_LEN);
			}
		break;
		
		default:
		return -1;
	}
	return 0;
}

int tls_ft_param_set(unsigned int opnum, void *data, unsigned int len)
{
	FT_PARAM_ST ft;
	psCrcContext_t ctx;
	unsigned int writelen = 0;



	if (!data || !len)
	{
		return -1;
	}
	tls_fls_read(FT_MAGICNUM_ADDR, (unsigned char *)&ft, sizeof(ft));
	switch (opnum)
	{
		case CMD_MAC:	/*MAC*/
			memcpy(ft.mac_addr, (unsigned char *)data, MAC_ADDR_LEN);
		break;

		case CMD_TX_DC:	/*tx_dcoffset*/
			ft.tx_dcoffset = *(unsigned int *)data;
		break;	
		
		case CMD_RX_DC:	/*rx_dcoffset*/
			ft.rx_dcoffset = *(unsigned int *)data;
		break;	
		
		case CMD_TX_IQ_GAIN:
			ft.tx_iq_gain = *(unsigned int *)data;
		break;	
		
		case CMD_RX_IQ_GAIN:
			ft.rx_iq_gain = *(unsigned int *) data;			
		break;	
		
		case CMD_TX_IQ_PHASE:
			ft.tx_iq_phase = *(unsigned int *)data;
		break;	
		
		case CMD_RX_IQ_PHASE:
			ft.rx_iq_phase = *(unsigned int *) data;			
		break;	
		
		case CMD_TX_GAIN: /*gain*/
			if (len >= FT_GAIN_LEN)
			{
				writelen = FT_GAIN_LEN;
			}
			else
			{
				writelen = len;
			}
			memcpy(ft.tx_gain, data, writelen);
		break;
		
		default:
		return -1;
	}

	tls_crypto_init();
	tls_crypto_crc_init(&ctx, 0xFFFFFFFF, CRYPTO_CRC_TYPE_32, INPUT_REFLECT | OUTPUT_REFLECT);
	ft.magic_no = SIGNATURE_WORD;
	tls_crypto_crc_update(&ctx, (unsigned char *)&ft + 12, sizeof(ft) -12);
	tls_crypto_crc_final(&ctx, &ft.checksum);
	tls_flash_unlock();
	tls_fls_write(FT_MAGICNUM_ADDR, (unsigned char *)&ft, sizeof(ft));
	tls_flash_lock();
	return 0;
}


/**********************************************************************************************************
* Description: 	This function is used to get mac addr.
*
* Arguments  : 	mac		mac addr,6 byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_get_mac_addr(u8 *mac)
{
	return tls_ft_param_get(CMD_MAC, mac, 6);
}

/**********************************************************************************************************
* Description: 	This function is used to set mac addr.
*
* Arguments  : 	mac		mac addr,6 byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_set_mac_addr(u8 *mac)
{
	return tls_ft_param_set(CMD_MAC, mac, 6);
}

/**********************************************************************************************************
* Description: 	This function is used to get tx gain.
*
* Arguments  : 	txgain		tx gain,12 byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_get_tx_gain(u8 *txgain)
{
	return tls_ft_param_get(CMD_TX_GAIN, txgain, 29);
}

/**********************************************************************************************************
* Description: 	This function is used to set tx gain.
*
* Arguments  : 	txgain		tx gain,17byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_gain(u8 *txgain)
{
	return tls_ft_param_set(CMD_TX_GAIN, txgain, 29);
}

/**********************************************************************************************************
* Description: 	This function is used to get tx lod.
*
* Arguments  : 	*txlo
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_get_tx_lo(u8 *txlo)
{
	return tls_ft_param_get(CMD_TX_DC, txlo, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set tx lo.
*
* Arguments  : 	txlo
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_lo(u8 *txlo)
{
	return tls_ft_param_set(CMD_TX_DC, txlo, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to get tx iq gain.
*
* Arguments  : 	txGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_tx_iq_gain(u8 *txGain)
{
	return tls_ft_param_get(CMD_TX_IQ_GAIN, txGain, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set tx iq gain.
*
* Arguments  : 	txGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_iq_gain(u8 *txGain)
{
	return tls_ft_param_set(CMD_TX_IQ_GAIN, txGain, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to get rx iq gain.
*
* Arguments  : 	rxGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_rx_iq_gain(u8 *rxGain)
{
	return tls_ft_param_get(CMD_RX_IQ_GAIN, rxGain, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set rx iq gain.
*
* Arguments  : 	rxGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_rx_iq_gain(u8 *rxGain)
{
	return tls_ft_param_set(CMD_RX_IQ_GAIN, rxGain, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to get tx iq phase.
*
* Arguments  : 	txPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_tx_iq_phase(u8 *txPhase)
{
	return tls_ft_param_get(CMD_TX_IQ_PHASE, txPhase, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set tx iq phase.
*
* Arguments  : 	txPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_iq_phase(u8 *txPhase)
{
	return tls_ft_param_set(CMD_TX_IQ_PHASE, txPhase, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to get rx iq phase.
*
* Arguments  : 	rxPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_rx_iq_phase(u8 *rxPhase)
{
	return tls_ft_param_get(CMD_RX_IQ_PHASE, rxPhase, 4);
}

/**********************************************************************************************************
* Description: 	This function is used to set tx iq phase.
*
* Arguments  : 	rxPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_rx_iq_phase(u8 *rxPhase)
{
	return tls_ft_param_set(CMD_RX_IQ_PHASE, rxPhase, 4);
}


int tls_freq_err_op(u8 *freqerr, u8 flag)
{
	tls_flash_unlock();
	if (flag){
		tls_fls_write(FT_MAGICNUM_ADDR + sizeof(FT_PARAM_ST), freqerr, 4);
	}
	else
	{
		tls_fls_read(FT_MAGICNUM_ADDR + sizeof(FT_PARAM_ST), freqerr, 4);
	}
	tls_flash_lock();
	return TLS_EFUSE_STATUS_OK;
}

int tls_rf_vcg_ctrl_op(u8 *vcg, u8 flag)
{
	tls_flash_unlock();
	if (flag){
		tls_fls_write(FT_MAGICNUM_ADDR + sizeof(FT_PARAM_ST)+4, vcg, 4);
	}
	else
	{
		tls_fls_read(FT_MAGICNUM_ADDR + sizeof(FT_PARAM_ST)+4, vcg, 4);
	}
	tls_flash_lock();	
	return TLS_EFUSE_STATUS_OK;
}



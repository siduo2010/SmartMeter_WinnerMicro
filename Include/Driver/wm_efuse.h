/***************************************************************************** 
* 
* File Name : wm_efuse.h
* 
* Description: virtual efuse Driver Module 
* 
* Copyright (c) 2014 Winner Microelectronics Co., Ltd. 
* All rights reserved. 
* 
* Author : dave 
* 
* Date : 2014-6-26
*****************************************************************************/ 
#ifndef WM_EFUSE_H
#define WM_EFUSE_H

#define TLS_EFUSE_STATUS_OK      (0)
#define TLS_EFUSE_STATUS_EINVALID      (1)
#define TLS_EFUSE_STATUS_EIO      (2)

enum {
	CMD_MAC = 0x01,
	CMD_TX_DC,
	CMD_RX_DC,
	CMD_TX_IQ_GAIN,
	CMD_RX_IQ_GAIN,
	CMD_TX_IQ_PHASE,
	CMD_RX_IQ_PHASE,
	CMD_TX_GAIN,
	CMD_ALL,
};


/**********************************************************************************************************
* Description: 	This function is used to write ft_param.
*
* Arguments  : 	opnum:  ft cmd
*               		data: data pointer
*               		len:  len to write data
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO			set failed
**********************************************************************************************************/
int tls_ft_param_set(unsigned int opnum, void *data, unsigned int len);

/**********************************************************************************************************
* Description: 	This function is used to read ft_param.
*
* Arguments  : 	opnum:  ft cmd
*              		data: data pointer to save
*               		len:  len to read data
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_ft_param_get(unsigned int opnum, void *data, unsigned int rdlen);


/**********************************************************************************************************
* Description: 	This function is used to get mac addr.
*
* Arguments  : 	mac		mac addr,6 byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_get_mac_addr(u8 *mac);

/**********************************************************************************************************
* Description: 	This function is used to set mac addr.
*
* Arguments  : 	mac		mac addr,6 byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_set_mac_addr(u8 *mac);

/**********************************************************************************************************
* Description: 	This function is used to get tx gain.
*
* Arguments  : 	txgain		tx gain,12 byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_get_tx_gain(u8 *txgain);

/**********************************************************************************************************
* Description: 	This function is used to set tx gain.
*
* Arguments  : 	txgain		tx gain,12 byte
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_gain(u8 *txgain);

/**********************************************************************************************************
* Description: 	This function is used to get tx lod.
*
* Arguments  : 	*txlo
*
* Returns    : 	TLS_EFUSE_STATUS_OK			get success
* 			TLS_EFUSE_STATUS_EIO		get failed
**********************************************************************************************************/
int tls_get_tx_lo(u8 *txlo);


/**********************************************************************************************************
* Description: 	This function is used to set tx lo.
*
* Arguments  : 	txlo
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_lo(u8 *txlo);

/**********************************************************************************************************
* Description: 	This function is used to get tx iq gain.
*
* Arguments  : 	txGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_tx_iq_gain(u8 *txGain);

/**********************************************************************************************************
* Description: 	This function is used to set tx iq gain.
*
* Arguments  : 	txGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_iq_gain(u8 *txGain);

/**********************************************************************************************************
* Description: 	This function is used to get rx iq gain.
*
* Arguments  : 	rxGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_rx_iq_gain(u8 *rxGain);

/**********************************************************************************************************
* Description: 	This function is used to set rx iq gain.
*
* Arguments  : 	rxGain
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_rx_iq_gain(u8 *rxGain);

/**********************************************************************************************************
* Description: 	This function is used to get tx iq phase.
*
* Arguments  : 	txPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_tx_iq_phase(u8 *txPhase);

/**********************************************************************************************************
* Description: 	This function is used to set tx iq phase.
*
* Arguments  : 	txPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_tx_iq_phase(u8 *txPhase);

/**********************************************************************************************************
* Description: 	This function is used to get rx iq phase.
*
* Arguments  : 	rxPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_get_rx_iq_phase(u8 *rxPhase);

/**********************************************************************************************************
* Description: 	This function is used to set tx iq phase.
*
* Arguments  : 	rxPhase
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_set_rx_iq_phase(u8 *rxPhase);
/**********************************************************************************************************
* Description: 	This function is used to set/get freq err.
*
* Arguments  : 	freqerr
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/

int tls_freq_err_op(u8 *freqerr, u8 flag);

/**********************************************************************************************************
* Description: 	This function is used to set/get vcg ctrl.
*
* Arguments  : 	vcg
*
* Returns    : 	TLS_EFUSE_STATUS_OK			set success
* 			TLS_EFUSE_STATUS_EIO		set failed
**********************************************************************************************************/
int tls_rf_vcg_ctrl_op(u8 *vcg, u8 flag);

#endif /* WM_EFUSE_H */


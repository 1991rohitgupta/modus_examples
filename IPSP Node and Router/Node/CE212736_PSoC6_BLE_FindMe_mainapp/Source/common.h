/*******************************************************************************
* File Name: BLEFindMe.h
*
* Version: 1.00
*
* Description:
*  Contains the function prototypes and constants for the BLE functionality.
*
********************************************************************************
* Copyright 2018-2019, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#ifndef BLEFINDME_H

    #define BLEFINDME_H

	#include "cy_device_headers.h"
	#include "cycfg.h"
	#include "cycfg_ble.h"
	#include "cy_syspm.h"
    #include "debug.h"
    #include "LED.h"

	/* IPSP defines */
	#define LE_DATA_CREDITS_IPSP         (1000u)
	/* Set the watermark to half of the total credits to be used */
	#define LE_WATER_MARK_IPSP           (LE_DATA_CREDITS_IPSP/2u)
	#define L2CAP_MAX_LEN                (CY_BLE_L2CAP_MTU - 2u)
	#define ADV_TIMER_HANDLE             (0u)
	#define ADV_TIMER_TIMEOUT            (1u)              /* counts in s */
	#define CONN_COUNT                  (1u)             /* up to CY_BLE_CONN_COUNT */
    /***************************************
    *       Function Prototypes
    ***************************************/
    void HostInit(void);
    void BleIPSPNode_Process(void);

#endif

/* [] END OF FILE */

/***************************************************************************//**
* \file main.c
* \version 1.0
*
* \brief
* Minimal new application template for 150MHz PSoC 6 devices. Debug is enabled 
* and platform clocks are set for high performance (144MHz CLK_FAST for CM4 
* core and 72MHz CLK_SLOW for CM0+) but with a peripheral-friendly CLK_PERI 
* frequency (72MHz).
*
********************************************************************************
* \copyright
* Copyright 2017-2019 Cypress Semiconductor Corporation
* SPDX-License-Identifier: Apache-2.0
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/


#include "cy_device_headers.h"
#include "cycfg.h"
#include "cy_syslib.h"
#include "cy_sysint.h"
#include "cy_ble_hal_pvt.h"
#include "cycfg_ble.h"
#include "stdio.h"

#define PEER_BD_ADDR 			{0x78, 0x88, 0xA4, 0x50, 0xA0, 0x00} //char board
//#define PEER_BD_ADDR 			{0xDF, 0x00, 0x01, 0x50, 0xA0, 0x00} // pioneer kit

cy_stc_scb_uart_context_t uartContext;

typedef enum
{
	DEVICE_IDLE,
	STACK_ON_COMPLETE,
    PEER_DISCONNECTED,
	INITIATE_CONNECTION,
    PEER_FOUND,
	WAIT_FOR_INIT,
    PEER_CONNECTED,
	MTU_RSP_RCVD,
	DISCOVERY_COMPLETE
}conn_state_t;

cy_stc_ble_gap_bd_addr_t connectToAddr;
uint8 myAddr[CY_BLE_GAP_BD_ADDR_SIZE] = PEER_BD_ADDR;
cy_stc_ble_conn_handle_t        appConnHandle;
uint8 connState = DEVICE_IDLE;

/* BLESS interrupt configuration structure */
const cy_stc_sysint_t  blessIsrCfg =
{
    /* The BLESS interrupt */
    .intrSrc       = bless_interrupt_IRQn,

    /* The interrupt priority number */
    .intrPriority  = 1u
};

void Handle_ConnState(void)
{
	cy_en_ble_api_result_t apiResult;
    cy_stc_ble_gatt_xchg_mtu_param_t MTUParams;
    uint8_t   config[2u];

	switch(connState)
	{
	case STACK_ON_COMPLETE:
	case PEER_DISCONNECTED:
		printf("Starting Scan\r\n");
        Cy_BLE_GAPC_StartScan(CY_BLE_SCANNING_FAST, CY_BLE_CENTRAL_CONFIGURATION_0_INDEX);
        connState = DEVICE_IDLE;
		break;
	case PEER_FOUND:
		printf("Found Peer! Stopping Scan\r\n");
		Cy_BLE_GAPC_StopScan();
		connState = WAIT_FOR_INIT;
		break;
	case INITIATE_CONNECTION:
		printf("Initiating Connection\r\n");
		Cy_BLE_GAPC_ConnectDevice(&connectToAddr, CY_BLE_CENTRAL_CONFIGURATION_0_INDEX);
		break;
	case PEER_CONNECTED:
		printf("Peer Connected. Exchanging MTU size\r\n");
        MTUParams.connHandle = appConnHandle;
        MTUParams.mtu = 512;
        Cy_BLE_GATTC_ExchangeMtuReq(&MTUParams);
        connState = DEVICE_IDLE;
        break;
	case MTU_RSP_RCVD:
		printf("Start Discovery\r\n");
		Cy_BLE_GATTC_StartDiscovery(appConnHandle);
		connState = DEVICE_IDLE;
		break;
	case DISCOVERY_COMPLETE:
		/* Enable Notification  */
        config[1u] = 0u;
        config[0u] = 1u;
        printf("Discovery Complete. Enable Notification\r\n");
        apiResult = Cy_BLE_BASC_SetCharacteristicDescriptor(appConnHandle,
    			0u,CY_BLE_BAS_BATTERY_LEVEL,CY_BLE_BAS_BATTERY_LEVEL_CCCD,sizeof(config), config);
        if(apiResult == CY_BLE_SUCCESS)
        {
        	printf("Notifications enabled Successfully\r\n");
        }
    	connState = DEVICE_IDLE;
		break;
	default:
		break;
	}

}

void StackEventHandler(uint32 event, void * eventParam)
{
    const cy_stc_ble_set_suggested_phy_info_t phyInfo =
    {
        .allPhyMask = CY_BLE_PHY_NO_PREF_MASK_NONE,
        .txPhyMask = CY_BLE_PHY_MASK_LE_2M,
        .rxPhyMask = CY_BLE_PHY_MASK_LE_2M
    };
    cy_stc_ble_gapc_adv_report_param_t  *advReport;
    cy_stc_ble_bless_clk_cfg_params_t clkParam;
    switch(event)
    {
    case CY_BLE_EVT_STACK_ON:
        Cy_BLE_SetDefaultPhy(&phyInfo);
        /* Set Clock accuracy to 20 ppm */
        clkParam.bleLlSca = CY_BLE_LL_SCA_000_TO_020_PPM;
        Cy_BLE_SetBleClockCfgParam(&clkParam);
        connState = STACK_ON_COMPLETE;
    	break;

    case CY_BLE_EVT_SET_DEFAULT_PHY_COMPLETE:
    	printf("Set Default PHY Completed");
        printf("status=%x \r\n",     ((cy_stc_ble_events_param_generic_t *)eventParam)->status);
        break;

    case CY_BLE_EVT_SET_PHY_COMPLETE:
    	printf("CY_BLE_EVT_SET_PHY_COMPLETE ");
    	printf("status=%x \r\n",     ((cy_stc_ble_events_param_generic_t *)eventParam)->status);
        break;

    case CY_BLE_EVT_PHY_UPDATE_COMPLETE:
    	printf("CY_BLE_EVT_PHY_UPDATE_COMPLETE: ");
    	printf("status=%x, ",
                    ((cy_stc_ble_events_param_generic_t *)eventParam)->status);
    	printf("bdHandle=%x, ",  ((cy_stc_ble_phy_param_t *)
                                     ((cy_stc_ble_events_param_generic_t *)eventParam)->eventParams)->bdHandle);
    	printf("rxPhyMask=%x, ", ((cy_stc_ble_phy_param_t *)
                                     ((cy_stc_ble_events_param_generic_t *)eventParam)->eventParams)->rxPhyMask);
    	printf("txPhyMask=%x, ", ((cy_stc_ble_phy_param_t *)
                                     ((cy_stc_ble_events_param_generic_t *)eventParam)->eventParams)->txPhyMask);
    	printf("\r\n");
        break;

    case CY_BLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
    	advReport = (cy_stc_ble_gapc_adv_report_param_t *)eventParam;
    	if(advReport->eventType == 0u)
    	{
    		if(!(memcmp(&myAddr, advReport->peerBdAddr, CY_BLE_GAP_BD_ADDR_SIZE)))
    		{
    			connState = PEER_FOUND;
    		}
    	}
    	break;
    case CY_BLE_EVT_GAPC_SCAN_START_STOP:
    	if(connState == WAIT_FOR_INIT)
    	{
    		connState = INITIATE_CONNECTION;
    	}
    	break;
    case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
        connState = PEER_CONNECTED;
        break;
    case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
    	connState = PEER_DISCONNECTED;
    	break;
    case CY_BLE_EVT_GATT_CONNECT_IND:
        appConnHandle = *(cy_stc_ble_conn_handle_t *)eventParam;
        break;
    case CY_BLE_EVT_GATTC_XCHNG_MTU_RSP:
    	connState = MTU_RSP_RCVD;
    	break;
    case CY_BLE_EVT_GATTC_DISCOVERY_COMPLETE:
    	connState = DISCOVERY_COMPLETE;
    	break;
    default:
    	break;
    }
}

int main(void)
{
    /* Set up the device based on configurator selections */
    init_cycfg_all();

    __enable_irq();
 
    /* Initialize and enable the Debug Uart peripheral */
    Cy_SCB_UART_Init(DEBUG_UART_HW, &DEBUG_UART_config, &uartContext);
    Cy_SCB_UART_Enable(DEBUG_UART_HW);

    /* Initialize the BLESS interrupt */
    cy_ble_config.hw->blessIsrConfig = &blessIsrCfg;
    (void) Cy_SysInt_Init(cy_ble_config.hw->blessIsrConfig, Cy_BLE_BlessIsrHandler);
    /* Register the generic event handler */
    Cy_BLE_RegisterEventCallback(StackEventHandler);

    /* Initialize and enable the BLE controller */
    (void) Cy_BLE_Init(&cy_ble_config);

    Cy_BLE_EnableLowPowerMode();

    (void) Cy_BLE_Enable();

	uint8 myAddr[CY_BLE_GAP_BD_ADDR_SIZE] = PEER_BD_ADDR;
	memcpy(&connectToAddr.bdAddr, myAddr, CY_BLE_GAP_BD_ADDR_SIZE);

    for(;;)
    {
    	/* Process pending BLE events */
    	Cy_BLE_ProcessEvents();
    	Handle_ConnState();
    }
}

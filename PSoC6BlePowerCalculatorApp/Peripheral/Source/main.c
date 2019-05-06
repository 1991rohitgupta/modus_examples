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

#include "bas.h"

uint8 bdHandle 	=0;
uint8 connectionId = 0;
uint16 negotiatedMTU = 0;
uint8 dataBuffer[245];
extern cy_stc_ble_stack_params_t stackParam;

#define MEASURE_DLE_POWER		        0u
#define TWO_MBPS_MEASUREMENT			0u
#define ENABLE_DATA_TXFR				0u
#define ENABLE_BASE_MEASUREMENT			0u
#define ENABLE_NON_CONN_ADV 		    0u
#define ADV_INTERVAL_MS			       100u

#if MEASURE_DLE_POWER
	#define MAX_NUMBER_OF_BYTES_PER_PACKET 244u
#else
	#define MAX_NUMBER_OF_BYTES_PER_PACKET 20u
#endif
/* BLESS interrupt configuration structure */
const cy_stc_sysint_t  blessIsrCfg =
{
    /* The BLESS interrupt */
    .intrSrc       = bless_interrupt_IRQn,

    /* The interrupt priority number */
    .intrPriority  = 1u
};

void Clear_HSIOM()
{
	HSIOM->PRT[0].PORT_SEL0 = 0;
	HSIOM->PRT[0].PORT_SEL1 = 0;
	HSIOM->PRT[1].PORT_SEL0 = 0;
	HSIOM->PRT[1].PORT_SEL1 = 0;
	HSIOM->PRT[2].PORT_SEL0 = 0;
	HSIOM->PRT[2].PORT_SEL1 = 0;
	HSIOM->PRT[3].PORT_SEL0 = 0;
	HSIOM->PRT[3].PORT_SEL1 = 0;
	HSIOM->PRT[4].PORT_SEL0 = 0;
	HSIOM->PRT[5].PORT_SEL0 = 0;
	HSIOM->PRT[5].PORT_SEL1 = 0;
	HSIOM->PRT[6].PORT_SEL0 = 0;
	HSIOM->PRT[6].PORT_SEL1 = 0;
	HSIOM->PRT[7].PORT_SEL0 = 0;
	HSIOM->PRT[7].PORT_SEL1 = 0;
	HSIOM->PRT[8].PORT_SEL0 = 0;
	HSIOM->PRT[8].PORT_SEL1 = 0;
	HSIOM->PRT[9].PORT_SEL0 = 0;
	HSIOM->PRT[9].PORT_SEL1 = 0;
	HSIOM->PRT[10].PORT_SEL0 = 0;
	HSIOM->PRT[10].PORT_SEL1 = 0;
	HSIOM->PRT[11].PORT_SEL0 = 0;
	HSIOM->PRT[11].PORT_SEL1 = 0;
	HSIOM->PRT[12].PORT_SEL0 = 0;
	HSIOM->PRT[12].PORT_SEL1 = 0;
	HSIOM->PRT[13].PORT_SEL0 = 0;
	HSIOM->PRT[13].PORT_SEL1 = 0;
	HSIOM->PRT[14].PORT_SEL0 = 0;
}

/*******************************************************************************
* Function Name: StackEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  uint8_t event:        Event from the BLE component
*  void* eventParams:    A structure instance for corresponding event type. The
*                        list of event structure is described in the component
*                        datasheet.
*
* Return:
*  None
*
*******************************************************************************/
void StackEventHandler(uint32_t event, void * eventParam)
{
	cy_stc_ble_gatt_xchg_mtu_param_t mtu;
#if TWO_MBPS_MEASUREMENT
    const cy_stc_ble_set_suggested_phy_info_t phyInfo =
    {
        .allPhyMask = CY_BLE_PHY_NO_PREF_MASK_NONE,
        .txPhyMask = CY_BLE_PHY_MASK_LE_2M,
        .rxPhyMask = CY_BLE_PHY_MASK_LE_2M
    };
#endif
    switch(event)
    {
    case CY_BLE_EVT_STACK_ON:
#if TWO_MBPS_MEASUREMENT
    	Cy_BLE_SetDefaultPhy(&phyInfo);
#endif
    	/* Enter into discoverable mode so that remote can search it. */

            cy_ble_configPtr->gappAdvParams[CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX].fastAdvIntervalMin = (uint16)((ADV_INTERVAL_MS*8ul)/5ul);
            cy_ble_configPtr->gappAdvParams[CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX].fastAdvIntervalMax = (uint16)((ADV_INTERVAL_MS*8ul)/5ul);
            cy_ble_configPtr->discoveryModeInfo[CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX].advParam->advChannelMap = 0x07;
#if ENABLE_NON_CONN_ADV
            cy_ble_configPtr->discoveryModeInfo[CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX].advParam->advType = CY_BLE_GAPP_NON_CONNECTABLE_UNDIRECTED_ADV;
            cy_ble_configPtr->discoveryModeInfo[CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX].advParam->advFilterPolicy = CY_BLE_GAPP_SCAN_CONN_WHITELIST_ONLY;
#else
            cy_ble_configPtr->discoveryModeInfo[CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX].advParam->advType = CY_BLE_GAPP_CONNECTABLE_UNDIRECTED_ADV;
            cy_ble_configPtr->discoveryModeInfo[CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX].advParam->advFilterPolicy = CY_BLE_GAPP_SCAN_ANY_CONN_ANY;
#endif
    	Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);

    	break;
    case CY_BLE_EVT_GAP_DEVICE_CONNECTED:
    	if(((cy_stc_ble_gap_connected_param_t*)eventParam)->status == 0)
    	{
    		bdHandle = ((cy_stc_ble_gap_connected_param_t*)eventParam)->bdHandle;
    	}
    	break;
    case CY_BLE_EVT_GAP_DEVICE_DISCONNECTED:
    	Cy_BLE_GAPP_StartAdvertisement(CY_BLE_ADVERTISING_FAST, CY_BLE_PERIPHERAL_CONFIGURATION_0_INDEX);
    	break;
    case CY_BLE_EVT_GATT_CONNECT_IND:
    	connectionId = (*(cy_stc_ble_conn_handle_t *)eventParam).attId;
    	break;
    case CY_BLE_EVT_GATT_DISCONNECT_IND:
    	negotiatedMTU = 0;
    	if(BasNotificationEnabled())
    	{
    		Disable_BattereyLevelNotification();
    	}
    	break;
    case CY_BLE_EVT_GATTS_XCNHG_MTU_REQ:
        mtu.connHandle = ((cy_stc_ble_gatt_xchg_mtu_param_t *)eventParam)->connHandle;
        Cy_BLE_GATT_GetMtuSize(&mtu);
        negotiatedMTU = mtu.mtu;
    	break;
    default:
        break;
    }
}

/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Main function.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main(void)
{
    /* Set up the device based on configurator selections */
    init_cycfg_all();

    __enable_irq();

#if ENABLE_BASE_MEASUREMENT
    Clear_HSIOM();
    while(1)
    {
    	Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
    }
#endif

    /* Initialize the BLESS interrupt */
    cy_ble_config.hw->blessIsrConfig = &blessIsrCfg;
    (void) Cy_SysInt_Init(cy_ble_config.hw->blessIsrConfig, Cy_BLE_BlessIsrHandler);

    /* Register the generic event handler */
    Cy_BLE_RegisterEventCallback(StackEventHandler);

#if MEASURE_DLE_POWER
    stackParam.dleMaxRxCapability = 251;
    stackParam.dleMaxTxCapability = 251;
#else
    stackParam.dleMaxRxCapability = 27;
    stackParam.dleMaxTxCapability = 27;
#endif

#if !(TWO_MBPS_MEASUREMENT)
    stackParam.featureMask &= ~(CY_BLE_PHY_UPDATE_FEATURE_MASK);
#endif
    /* Initialize and enable the BLE controller */
    (void) Cy_BLE_Init(&cy_ble_config);

    Cy_BLE_EnableLowPowerMode();

    (void) Cy_BLE_Enable();

    BasInit();


    for(;;)
    {
    	/* Process pending BLE events */
    	Cy_BLE_ProcessEvents();
    	Cy_SysPm_CpuEnterDeepSleep(CY_SYSPM_WAIT_FOR_INTERRUPT);
#if ENABLE_DATA_TXFR
    	if(Cy_BLE_GetNumOfActiveConn() > 0 && negotiatedMTU != 0 && BasNotificationEnabled())
    	{
            cy_stc_ble_gatts_handle_value_ntf_t ntfReqParam =
            {
                /* Fill all fields of the Write request structure ... */
                .handleValPair.attrHandle = cy_ble_bassConfigPtr->attrInfo[0].batteryLevelHandle,
                .handleValPair.value.val  = &dataBuffer[0],
#if MEASURE_DLE_POWER
                .handleValPair.value.len  = (negotiatedMTU > MAX_NUMBER_OF_BYTES_PER_PACKET ? MAX_NUMBER_OF_BYTES_PER_PACKET : negotiatedMTU),
#else
				.handleValPair.value.len  = MAX_NUMBER_OF_BYTES_PER_PACKET,
#endif
				.connHandle               = cy_ble_connHandle[connectionId]
            };
            /* Send notification to the Client */
           Cy_BLE_GATTS_Notification(&ntfReqParam);
    	}
#endif
    }
}

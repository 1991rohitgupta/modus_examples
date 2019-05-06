/*******************************************************************************
* File Name: bas.c
*
* Version: 1.0
*
* Description:
* This file contains the code for the BAS.
*
* Hardware Dependency:
*  CY8CKIT-062 PSoC6 BLE Pioneer Kit
*
********************************************************************************
* Copyright 2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/
#include "bas.h"

/* Static global variables */
static uint8_t batteryLevel = SIM_BATTERY_MIN;
static uint8 batteryLevelNtfStatus = 0;

/*******************************************************************************
* Function Name: BasInit
********************************************************************************
*
* Summary:
*   Initializes the battery service.
*
*******************************************************************************/
void BasInit(void)
{
    /* Register service specific callback function */
    Cy_BLE_BAS_RegisterAttrCallback(BasCallBack);

}

/*******************************************************************************
* Function Name: BasCallBack
********************************************************************************
*
* Summary:
*   This is an event callback function to receive service-specific events from
*   Battery Service.
*
* Parameters:
*   event      - the event code
*   eventParam - the event parameters
*
********************************************************************************/
void BasCallBack(uint32_t event, void *eventParam)
{
	(void)eventParam;
    switch(event)
    {
        case CY_BLE_EVT_BASS_NOTIFICATION_ENABLED:
        	batteryLevelNtfStatus=1;
            break;

        case CY_BLE_EVT_BASS_NOTIFICATION_DISABLED:
        	batteryLevelNtfStatus =0;
            break;

        default:
            break;
    }
}

/*******************************************************************************
* Function Name: BasGetBatteryLevel
********************************************************************************
*
* Summary:
*   This function returns simulated battery voltage value.
*
* Returns:
*  uint8_t - battery voltage value.
*******************************************************************************/
uint8_t BasGetBatteryLevel(void)
{
    return(batteryLevel);
}

uint8_t BasNotificationEnabled(void)
{
    return(batteryLevelNtfStatus);
}

void Disable_BattereyLevelNotification(void)
{

    cy_stc_ble_gatts_db_attr_val_info_t dbAttrValInfo;
    batteryLevelNtfStatus =0;
    dbAttrValInfo.connHandle.bdHandle        = 0u;
    dbAttrValInfo.connHandle.attId           = 0u;
    dbAttrValInfo.handleValuePair.attrHandle = cy_ble_bassConfigPtr->attrInfo[0].cccdHandle;
    dbAttrValInfo.handleValuePair.value.len  = 2;
    dbAttrValInfo.handleValuePair.value.val  = 0;
    dbAttrValInfo.offset                     = 0u;
    dbAttrValInfo.flags                      = CY_BLE_GATT_DB_LOCALLY_INITIATED;

    Cy_BLE_GATTS_WriteAttributeValueCCCD(&dbAttrValInfo);
}
/* [] END OF FILE */

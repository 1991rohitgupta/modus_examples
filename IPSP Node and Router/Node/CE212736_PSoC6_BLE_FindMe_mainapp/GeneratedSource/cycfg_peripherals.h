/*******************************************************************************
* File Name: cycfg_peripherals.h
*
* Description:
* Peripheral Hardware Block configuration
* This file was automatically generated and should not be modified.
* 
********************************************************************************
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
********************************************************************************/

#if !defined(CYCFG_PERIPHERALS_H)
#define CYCFG_PERIPHERALS_H

#include "cycfg_notices.h"
#include "cy_sysclk.h"
#include "cy_csd.h"
#include "cy_scb_ezi2c.h"
#include "cy_scb_uart.h"
#include "cy_mcwdt.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define CY_BLE_CORE_CORTEX_M4 4U
#define CY_BLE_CORE_CORTEX_M0P 0U
#define CY_BLE_CORE_DUAL 255U
#ifndef CY_BLE_CORE
	#define CY_BLE_CORE 4U
#endif
#define CY_BLE_IRQ bless_interrupt_IRQn
#define CY_CAPSENSE_CORE 4u
#define CY_CAPSENSE_CPU_CLK 12500000u
#define CY_CAPSENSE_PERI_CLK 6250000u
#define CY_CAPSENSE_VDDA_MV 3300u
#define CY_CAPSENSE_PERI_DIV_TYPE CY_SYSCLK_DIV_8_BIT
#define CY_CAPSENSE_PERI_DIV_INDEX 1u
#define Cmod_PORT GPIO_PRT7
#define Button0_Sns0_PORT GPIO_PRT8
#define Button1_Sns0_PORT GPIO_PRT8
#define LinearSlider0_Sns0_PORT GPIO_PRT8
#define LinearSlider0_Sns1_PORT GPIO_PRT8
#define LinearSlider0_Sns2_PORT GPIO_PRT8
#define LinearSlider0_Sns3_PORT GPIO_PRT8
#define LinearSlider0_Sns4_PORT GPIO_PRT8
#define Cmod_PIN 7u
#define Button0_Sns0_PIN 1u
#define Button1_Sns0_PIN 2u
#define LinearSlider0_Sns0_PIN 3u
#define LinearSlider0_Sns1_PIN 4u
#define LinearSlider0_Sns2_PIN 5u
#define LinearSlider0_Sns3_PIN 6u
#define LinearSlider0_Sns4_PIN 7u
#define Cmod_PORT_NUM 7u
#define CapSense_HW CSD0
#define CapSense_IRQ csd_interrupt_IRQn
#define KIT_I2C_HW SCB3
#define KIT_I2C_IRQ scb_3_interrupt_IRQn
#define KIT_UART_HW SCB5
#define KIT_UART_IRQ scb_5_interrupt_IRQn
#define MCWDT_HW MCWDT_STRUCT0
#define MCWDT_IRQ srss_interrupt_mcwdt_0_IRQn

extern cy_stc_csd_context_t cy_csd_0_context;
extern const cy_stc_scb_ezi2c_config_t KIT_I2C_config;
extern const cy_stc_scb_uart_config_t KIT_UART_config;
extern const cy_stc_mcwdt_config_t MCWDT_config;

void init_cycfg_peripherals(void);

#if defined(__cplusplus)
}
#endif


#endif /* CYCFG_PERIPHERALS_H */
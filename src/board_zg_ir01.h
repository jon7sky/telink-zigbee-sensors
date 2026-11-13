/********************************************************************************************************
 * @file    board_zg_ir01.h
 *
 * @brief   HOBEIAN ZG-IR01 (_TZE200_33rdmvgw): battery Zigbee IR blaster
 *          with temperature / humidity sensor, 2 x AAA
 *
 *******************************************************************************************************/
#ifndef _BOARD_ZG_IR01_H_
#define _BOARD_ZG_IR01_H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZG_IR01)

#define DEV_SERVICES (SERVICE_ZIGBEE | SERVICE_OTA | SERVICE_THS | SERVICE_LED)

/*
TLSR8258 (marked "Z2"), 1M Flash (GD25, mid 0x1460c8)

Pin map recovered from the stock firmware ("CNTOP ZG_ZIGBEE_SDK V1.2"):

GPIO_PA1 - KEY (to GND, internal pull-up 10K)
GPIO_PA7 - SWS, test pad 3
GPIO_PB1 - TX (115200, debug / factory test)
GPIO_PB5 - SDA (SHT40 @ 0x44, bit-banged)
GPIO_PB6 - SCL (SHT40 @ 0x44, bit-banged)
GPIO_PB7 - RX
GPIO_PC0 - LED to +Vbat
GPIO_PC1 - IR LED driver, PWM0 38 kHz, active high
GPIO_PC2 - IR receiver output
GPIO_PC4 - IR receiver power, active high

Test pads (pad 1 is the square one):
1 - PB7 (RX), 2 - PB1 (TX), 3 - SWS, 4 - GND, 5 - +3V.
Pads 1 and 2 were checked with a USB serial adapter: pulses on PB1 arrive
at the adapter's RX, and the adapter's TX drives PB7.

The IR pins are parked in a safe state here and driven by ir_blaster.c.
*/

// IR learn / send, codes are stored in the upper half of the 1M flash
#define USE_IR_BLASTER		1

#define BLE_MODEL_STR		"ZG-IR01"
#define BLE_MAN_STR			"HOBEIAN"

#define ZCL_BASIC_MFG_NAME     {7,'H','O','B','E','I','A','N'} // HOBEIAN
#define ZCL_BASIC_MODEL_ID	   {9,'Z','G','-','I','R','0','1','-','z'} // ZG-IR01-z

// Battery & RF Power
#define USE_BATTERY 	BATTERY_2AAA

// DISPLAY
#define	USE_DISPLAY			0

// KEY, BUTTON
#define BUTTON1				GPIO_PA1
#define BUTTON1_ON			0
#define PA1_INPUT_ENABLE	1
#define PA1_DATA_OUT		0
#define PA1_OUTPUT_ENABLE	0
#define PA1_FUNC			AS_GPIO
#define PULL_WAKEUP_SRC_PA1 PM_PIN_PULLUP_10K

// LED
#define LED_ON				0
#define LED_OFF				1
#define GPIO_LED			GPIO_PC0
#define PC0_FUNC	  		AS_GPIO
#define PC0_OUTPUT_ENABLE	1
#define PC0_INPUT_ENABLE	1
#define PC0_DATA_OUT		LED_OFF
// Not joined: flash twice a second for 3 minutes, then once every 10 s
#define PAIRING_FAST_BLINKS	360

// I2C Sensor
#define	USE_I2C_DRV			I2C_DRV_SOFT // PB5/PB6 is not a hardware I2C pin group
#define	I2C_CLOCK			400000

// I2C - SCL
#define I2C_SCL 			GPIO_PB6
#define PB6_FUNC			AS_GPIO
#define PB6_INPUT_ENABLE	1
#define PB6_OUTPUT_ENABLE	0
#define PB6_DATA_OUT		0
#define PULL_WAKEUP_SRC_PB6	PM_PIN_PULLUP_10K

// I2C - SDA
#define I2C_SDA 			GPIO_PB5
#define PB5_FUNC			AS_GPIO
#define PB5_INPUT_ENABLE	1
#define PB5_OUTPUT_ENABLE	0
#define PB5_DATA_OUT		0
#define PULL_WAKEUP_SRC_PB5	PM_PIN_PULLUP_10K

// Sensor T&H
#define USE_SENSOR_CHT8305		0
#define USE_SENSOR_CHT8215		0
#define USE_SENSOR_AHT20_30		0
#define USE_SENSOR_SHT4X		1
#define USE_SENSOR_SHTC3		0
#define USE_SENSOR_SHT30		0

// IR LED driver: off (low), pull-down keeps it off in deep sleep
#define GPIO_IR_TX			GPIO_PC1
#define PC1_FUNC			AS_GPIO
#define PC1_INPUT_ENABLE	0
#define PC1_OUTPUT_ENABLE	1
#define PC1_DATA_OUT		0
#define PULL_WAKEUP_SRC_PC1	PM_PIN_PULLDOWN_100K

// IR receiver power: off (low)
#define GPIO_IR_RX_POWER	GPIO_PC4
#define PC4_FUNC			AS_GPIO
#define PC4_INPUT_ENABLE	0
#define PC4_OUTPUT_ENABLE	1
#define PC4_DATA_OUT		0
#define PULL_WAKEUP_SRC_PC4	PM_PIN_PULLDOWN_100K

// IR receiver output: not used, pulled down as in the stock firmware
#define GPIO_IR_RX			GPIO_PC2
#define PC2_FUNC			AS_GPIO
#define PC2_INPUT_ENABLE	0
#define PC2_OUTPUT_ENABLE	0
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLDOWN_100K

// VBAT
#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR825x
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // (BOARD == BOARD_ZG_IR01)
#endif // _BOARD_ZG_IR01_H_

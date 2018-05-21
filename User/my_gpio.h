/*
 * my_gpio.h
 *
 *  Created on: 9 de jan de 2018
 *      Author: Bruno
 */

#include "em_gpio.h"

#ifndef USER_MY_GPIO_H_
#define USER_MY_GPIO_H_

#define VIBRA_CALL_GPIO_PORT	gpioPortB
#define VIBRA_CALL_PIN		11

#define LED_DEBUG_GPIO_PORT	gpioPortC
#define LED_DEBUG_PIN		10

#define LED_DEBUG_GPIO_PORT	gpioPortC
#define LED_DEBUG1_PIN		11

#define BUTTON_GPIO_PORT	gpioPortF
#define BUTTON_PIN		3

//#define BUTTON_1_GPIO_PORT	gpioPortF
//#define BUTTON_1_PIN		4

#define LED_VERMELHO_PORT	LED_DEBUG_GPIO_PORT
#define LED_VERMELHO_PIN	LED_DEBUG_PIN
#define LED_AMARELO_PORT	LED_DEBUG_GPIO_PORT
#define LED_AMARELO_PIN		LED_DEBUG1_PIN

#endif /* USER_MY_GPIO_H_ */

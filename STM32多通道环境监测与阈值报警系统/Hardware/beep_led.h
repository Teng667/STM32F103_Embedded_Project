#ifndef __BEEP_LED_H
#define __BEEP_LED_H
#include "stm32f10x.h"

#define BEEP_ON     GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define BEEP_OFF    GPIO_SetBits(GPIOB,GPIO_Pin_12)

#define LED_ON      GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define LED_OFF     GPIO_ResetBits(GPIOB,GPIO_Pin_13)

void BeepLed_Init(void);

#endif

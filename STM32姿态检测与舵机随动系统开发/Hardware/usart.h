#ifndef __USART_H
#define __USART_H

#include <stdio.h>
#include "stm32f10x.h"
void Serial_SendByte(u8 Byte);
void USART1_Init(u32 baud);

#endif

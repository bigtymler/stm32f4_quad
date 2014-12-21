#ifndef USART_H_
#define USART_H_

#include "stm32f4xx.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"


#endif //ending usart define


#define MAX_STRING_LENGTH 200

void init_usart1(int b);
void init_usart2(int b);
void init_usart3(int b);
void init_usart4(int b);
void USART_puts(USART_TypeDef* USARTx, volatile char *s);

char received_string[200];
char send_string[MAX_STRING_LENGTH];
extern char nc_flag; //used to say theres a new command waiting to be parsed

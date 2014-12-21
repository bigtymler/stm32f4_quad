#include "uart.h"

void init_usart3(int baudrate) {

  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* enable peripheral clock for USART2 */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  /* GPIOA clock enable */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

  /* GPIOA Configuration:  USART2 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect USART1 pins*/
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_USART3);

  USART_InitStructure.USART_BaudRate = baudrate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl =
  USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(USART3, &USART_InitStructure);

  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  // we want to configure the USART1 interrupts
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // this sets the priority group of the USART1 interrupts
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  // this sets the subpriority inside the group
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // the USART1 interrupts are globally enabled
  NVIC_Init(&NVIC_InitStructure);

  USART_Cmd(USART3, ENABLE);
}

void USART_puts(USART_TypeDef* USARTx, volatile char *s) {

  while (*s) {
    // wait until data register is empty
    while (!(USARTx->SR & 0x00000040))
      ;
    USART_SendData(USARTx, *s);
    *s++;
  }
}

void USART3_IRQHandler(void) {
  // check if the USART1 receive interrupt flag was set
  if (USART_GetITStatus(USART3, USART_IT_RXNE)) {
	    static uint8_t cnt = 0;  // this counter is used to determine the string length
	    char t = USART3->DR;  // the character from the USART1 data register is saved in t
	    /* check if the received character is not the LF character (used to determine end of string)
	     * or the if the maximum string length has been been reached
	     */
	    if ((t != '\n') && (cnt < MAX_STRING_LENGTH)) {
	      received_string[cnt] = t;
	      ++cnt;
	    } else {  // otherwise reset the character counter and print the received string
	      cnt = 0;
	      nc_flag = 1;
	    }
  }
}


int _write(int file, char *ptr, int len) {
  /* Write "len" of char from "ptr" to file id "fd"
   * Return number of char written.
   * Need implementing with UART here. */
  int counter;
  counter = len;
  for (; counter > 0; counter--) {
    if (*ptr == 0)
      break;
    USART_SendData(USART3, (uint16_t) (*ptr));
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET) {
    }
    ptr++;
  }
  return len;
}

//int _read(int fd, char *ptr, int len) {
//  /* Read "len" of char to "ptr" from file id "fd"
//   * Return number of char read.
//   * Need implementing with UART here. */
//  return len;
//}

void _ttywrch(int ch) {
  /* Write one char "ch" to the default console
   * Need implementing with UART here. */
}

///* SystemInit will be called before main */
//void SystemInit() {
//  retarget_init();
//}

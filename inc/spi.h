/*
 * spi.h
 *
 *  Created on: Mar 13, 2014
 *      Author: tyler
 */

#ifndef SPI_H_
#define SPI_H_

#include <stm32f4xx_spi.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

void spi_init(void);
uint8_t spi_send(uint8_t data);

#endif /* SPI_H_ */

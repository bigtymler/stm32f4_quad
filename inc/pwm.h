#ifndef PWM_H_
#define PWM_H_

#include "stm32f4xx.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"

void init_pwm(void);
void allPwm(int pwm);
// increase this value to name roll go -
void nRollPwm(int pwm);
// increase this value to make roll go +
void pRollPwm(int pwm);
// increase this value to make pitch go -
void nPitchPwm(int pwm);
// increase this value to make pitch go +
void pPitchPwm(int pwm);
#endif /* PWM_H_ */

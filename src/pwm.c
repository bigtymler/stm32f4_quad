#include "./pwm.h"

void init_pwm(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;

  /* TIM config */

  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10
      | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_TIM4);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_TIM2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_TIM2);

  //PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 21000000) - 1;
  TIM_TimeBaseStructure.TIM_Period = 8400;
  TIM_TimeBaseStructure.TIM_Prescaler = 4;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

  // PWM1 Mode configuration: Channel3
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC3Init(TIM2, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);

  //PWM1 Mode configuration: Channel4
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OC4Init(TIM2, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM4, ENABLE);
  TIM_ARRPreloadConfig(TIM2, ENABLE);

  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);
  TIM_Cmd(TIM2, ENABLE);
}

void allPwm(int pwm){
	TIM4->CCR4 = pwm;
	TIM2->CCR3 = pwm;
	TIM2->CCR4 = pwm;
	TIM4->CCR3 = pwm;
}

// B props are roll
// A props are pitch
// increase this value to make roll go -
void nRollPwm(int pwm){
    TIM4->CCR4 = pwm; //negative
}

// increase this value to make roll go +
void pRollPwm(int pwm){
	TIM2->CCR3 = pwm;
}

// increase this value to make pitch go -
void nPitchPwm(int pwm){
	TIM2->CCR4 = pwm;
}

// increase this value to make pitch go +
void pPitchPwm(int pwm){
	TIM4->CCR3 = pwm; //pos
}

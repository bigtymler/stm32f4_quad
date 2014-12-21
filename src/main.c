/**
 ******************************************************************************
 * @file    GPIO/GPIO_IOToggle/main.c
 * @author  MCD Application Team
 * @version V1.4.0
 * @date    04-August-2014
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "uart.h"
#include "spi.h"
#include "imu.h"
#include "controller.h"
#include "io.h"
#include "motor.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char received_string[200];

char nc_flag = 0;
Imu i;
Control rollCon, pitchCon, altitudeCon;
Motor nPitch, pPitch, nRoll, pRoll;
uint32_t us_count = 0;
int motor_const = 0;
char usCountEn = 0;
char balance = 0;
char alt = 0;
char calibrate = 0;
float avg_pitch = 0;
float avg_roll = 0;
float avg_aZ = 0;
int cal_count = 0;

int main(void) {
	SystemInit();
	init_leds();
	init_miscPins();

	init_usart3(115200);
	spi_init();

	imu_init(&i);
	initMotor(&nPitch);
	initMotor(&pPitch);
	initMotor(&nRoll);
	initMotor(&pRoll);

	char token[200];
	int key;
	uint16_t test = 57;
	uint16_t spi_ret = 0;
	uint16_t data[2];

	// init systick interrupt for 1us
	while (SysTick_Config(SystemCoreClock / 1000000) != 0)
		;

	config_accel(&i);
	usCountEn = 1;
	while (us_count < 1000)
		;
	config_gyro(&i);
	usCountEn = 0;
	usCountEn = 1;
	while (us_count < 1000)
		;
	usCountEn = 0;

	initControlTimer();

	while (1) {
		if (nc_flag == 1) {
			sscanf(received_string, "%s %i", &token, &key);
			if (strcmp(token, "pwm") == 0) {
				nPitch.const_pwm = key;
				pPitch.const_pwm = key;
				nRoll.const_pwm = key;
				pRoll.const_pwm = key;
				printf("pwm set\r\n");
			} else if (strcmp(token, "rst") == 0) {
				NVIC_SystemReset();
			}else if (strcmp(token, "pwr") == 0) {
				allPwm(key);
			} else if (strcmp(token, "b") == 0) {
				controlInit(&pitchCon, 2, 0.4, 10, IBAND);
				controlInit(&rollCon, 2, 0.4, 10, IBAND);
				controlInit(&altitudeCon, 80, 1, 20, 1);
				setTarget(&rollCon, 0);
				setTarget(&pitchCon, 0);
				setTarget(&altitudeCon, 9.8);
				initControlTimer();
				if (key) {
					balance = 1;
					printf("motors initalized!\r\n");
				} else {
					balance = 0;
				}
			} else if (strcmp(token, "cal") == 0) {
				initControlTimer();
				calibrate = 1;
				printf("calibrating please wait...\r\n");
				while(calibrate);
				printf("calibration done\r\n");
				TIM_Cmd(TIM5, DISABLE);
			} else if (strcmp(token, "e") == 0) {
				init_pwm();
				printf("pwm on\r\n");
			} else if (strcmp(token, "d") == 0) {
				if (key)
					debugTimer();
				else
					TIM_Cmd(TIM3, DISABLE);
			} else {
				printf("command not found\r\n");
			}
			memset(received_string, '\0', sizeof(received_string));
			memset(token, '\0', sizeof(token));
			key = 0;
			nc_flag = 0;
		}
	}
}

void SysTick_Handler(void) {
	if (usCountEn) {
		if (us_count >= (1000000 - 1))
			us_count = 0;
		else
			++us_count;
	} else
		us_count = 0;
}

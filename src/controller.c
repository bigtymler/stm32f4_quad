#include "controller.h"

void controlInit(Control *c, float p, float i, float d, float band) {
	c->kp = p;
	c->ki = i;
	c->kd = d;
	c->set_point = 0;
	c->last_err = 0;
	c->output = 0;
	c->iBand = IBAND;
	c->p = 0;
	c->i = 0;
	c->d = 0;
}

// updates pid values based on imu feedback
// controller is multi-dimensional so feedback is pitch, roll, or altitude
void updateControl(Control *c, float feedback) {
	c->err = c->set_point - feedback;
	c->p = (c->err) * c->kp;
	c->d = (c->last_err - c->err) * c->kd;
	if (c->err > -c->iBand && c->err < c->iBand) {
		c->i += (c->err) * c->ki;
	} else {
		c->i = 0;
	}
	if (c->i < -1000)
		c->i = -1000;
	if (c->i > 1000)
		c->i = 1000;
	c->output = c->p + c->i + c->d;
	c->last_err = c->err;
}

// sets target point for pid controller
void setTarget(Control *c, float tar) {
	c->set_point = tar;
}

void debugTimer(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 420;
	TIM_TimeBaseStructure.TIM_Prescaler = 100;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	/* TIM IT enable */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	/* TIM2 enable counter */
	TIM_Cmd(TIM3, ENABLE);
}

void initControlTimer(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* Enable the TIM5 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* TIM1 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 420;
	TIM_TimeBaseStructure.TIM_Prescaler = 100;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	/* TIM IT enable */
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	/* TIM2 enable counter */
	TIM_Cmd(TIM5, ENABLE);
}

extern Imu i;
extern Control rollCon, pitchCon, altitudeCon;
Motor nPitch, pPitch, nRoll, pRoll;
uint16_t counter = 0;
extern char balance;
extern char calibrate;
extern int cal_count;
extern float avg_pitch, avg_roll, avg_aZ;
//control isr
void TIM5_IRQHandler(void) {
	get_accel(&i);
	calc_pitchRoll(&i);
	get_gyro(&i);
	compFilt_pitchRoll(&i);
	avgFilt_zAccel(&i);
	//this should shut down the quad if it flips over
//	if(i.avgAZ >= 1.5){
//		allPwm(0);
//		balance = 0;
//		NVIC_SystemReset();
//	}
	if (balance) {
		updateControl(&rollCon, i.compFiltRoll);
		updateControl(&pitchCon, i.compFiltPitch);
		updateControl(&altitudeCon, -1 * i.avgAZ);
		prPwm(PITCH, &pPitch, &nPitch, pitchCon.output);
		prPwm(ROLL, &pRoll, &nRoll, rollCon.output);
		altitudePwm(&pPitch, &nPitch, &pRoll, &nRoll, altitudeCon.output);
		calcOutput(&pPitch);
		calcOutput(&nPitch);
		calcOutput(&pRoll);
		calcOutput(&nRoll);
		pPitchPwm(pPitch.output);
		nPitchPwm(nPitch.output);
		pRollPwm(pRoll.output);
		nRollPwm(nRoll.output);
	}else if(calibrate){
		if(cal_count < CALSAMPLES){
			avg_pitch += i.compFiltPitch;
			avg_roll += i.compFiltRoll;
			avg_aZ += i.avgAZ;
		}
		if(cal_count >= CALSAMPLES){
			calibrate = 0;
			cal_count = 0;
			printf("%.2f %.2f %.2f\r\n",avg_roll/CALSAMPLES, avg_pitch/CALSAMPLES, avg_aZ/CALSAMPLES);
			i.aZOffset = 9.8 + avg_aZ/CALSAMPLES;
			avg_roll = avg_pitch = avg_aZ = 0;
			printf("offset = %f\r\n",i.aZOffset);
		}
		cal_count++;
	}
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
	}
}

uint16_t debugCounter = 0;
float gyroZ = 0;
void TIM3_IRQHandler(void) {
	if (debugCounter >= 100) {
//		gyroZ += ((float) i.gRawZ * i.gScale) * 0.01;
//		printf("%f %f %f %f %f\r\n", i.avgAZ, altitudeCon.p, altitudeCon.i,
//				altitudeCon.d, altitudeCon.output);
//		printf("%f %f %f\r\n",altitudeCon.p,altitudeCon.d,altitudeCon.output);
		printf("%f %f %f\r\n", i.compFiltPitch, i.compFiltRoll, i.avgAZ);
//		printf("%i %i %i %i\r\n",pPitch.output, nPitch.output, pRoll.output, nRoll.output);
		debugCounter = 0;
	} else
		debugCounter++;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

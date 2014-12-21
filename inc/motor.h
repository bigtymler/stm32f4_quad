#ifndef MOTOR_H
#define MOTOR_H

#include "pwm.h"

#define PITCH 0
#define ROLL 1

typedef struct {
	int pitch, roll, yaw, altitude, output;
	int const_pwm;
}Motor;

void initMotor(Motor *m);
void prPwm(int type, Motor *pos, Motor *neg, float feedback);
void yawPwm();
void altitudePwm(Motor *m1, Motor *m2, Motor *m3, Motor *m4, float feedback);
void calcOutput(Motor *m);

#endif

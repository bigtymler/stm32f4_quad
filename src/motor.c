#include "motor.h"

// processes the pitch and roll pwm
void initMotor(Motor *m) {
	m->pitch = 0;
	m->roll = 0;
	m->yaw = 0;
	m->altitude = 0;
	m->output = 0;
	m->const_pwm = 500;
}

void prPwm(int type, Motor *pos, Motor *neg, float feedback) {
	if (type == PITCH) {
		if (feedback >= 0) {
			neg->pitch = (int) feedback * -1;
			pos->pitch = (int) feedback ;
		} else {
			pos->pitch = (int) feedback;
			neg->pitch = (int) feedback * -1;
		}
	} else if (type == ROLL) {
		if (feedback >= 0) {
			neg->roll = (int) feedback * -1;
			pos->roll = (int) feedback;
		} else {
			pos->roll = (int) feedback;
			neg->roll = (int) feedback * -1;
		}
	}
}

void yawPwm() {

}

void altitudePwm(Motor *m1, Motor *m2, Motor *m3, Motor *m4, float feedback) {
	m1->altitude = (int) feedback;
	m2->altitude = (int) feedback;
	m3->altitude = (int) feedback;
	m4->altitude = (int) feedback;
}

void calcOutput(Motor *m) {
	m->output = m->pitch + m->roll + m->yaw + m->altitude + m->const_pwm;
	if(m->output < 0)
		m->output = 0;
}

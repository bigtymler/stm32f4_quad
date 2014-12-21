#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <stdio.h>
#include <math.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>
#include <imu.h>
#include "motor.h"

typedef struct {
    float p,i,d;
    float kp,ki,kd;
    float output;
    float current_err,last_err;
    float set_point;
    float iBand;
    float err;
}Control;

#define CALSAMPLES 1000
#define RKP 0
#define RKI 0
#define RKD 0
#define PKP 0
#define PKI 0
#define PKD 0
#define IBAND 8.0 //defines the area where the i term builds up, this is +/-

void controlInit(Control *c, float p, float i, float d, float band);
void updateControl(Control *c, float feedback);
void setTarget(Control *c, float tar);


#endif

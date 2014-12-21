#ifndef IMU_H
#define IMU_H

#include <stdio.h>
#include <math.h>
#include <stm32f4xx_gpio.h>
#include <stm32f4xx_rcc.h>

#define IMUDT 0.0001
#define FILTERWEIGHT 0.98
#define SAMPLES 3

//float to byte or vice versa
typedef union{
    float data;
    char b[sizeof(float)];
}F2b ;

typedef struct {
	int16_t gRawX,gRawY,gRawZ,aRawX,aRawY,aRawZ;
	float aLogZ[SAMPLES];
	float avgAZ;
    float aGRawX,aGRawY,aGRawZ;
    float aRawPitch, aRawRoll, gRawPitch, gRawRoll;
    float aScale, gScale;
    float compFiltPitch, compFiltRoll;
    float aZOffset;
    uint16_t sampleCount;
}Imu;


#include "imu.h"
#include "spi.h"

void imu_init(Imu *i);
void gyro_cs(char state);
void accel_cs(char state);
uint8_t wai_g(void);
uint8_t wai_a(void);
void config_accel(Imu *i);
void config_gyro(Imu *i);
void get_accel(Imu *i);
void get_gyro(Imu *i);
void calc_pitchRoll(Imu *i);
void compFilt_pitchRoll(Imu *i);
void avgFilt_zAccel(Imu *i);
void disp_aRaw(Imu *i);
void disp_gRaw(Imu *i);
void disp_agRaw(Imu *i);
#endif

#include "imu.h"
#include "spi.h"

void imu_init(Imu *i) {
	int x;
	i->gRawPitch = 0;
	i->gRawRoll = 0;
	i->compFiltPitch = 0;
	i->compFiltRoll = 0;
//	i->sampleCount = 0;
//	for(x = 0; x < SAMPLES; x++){
//		i->aLogX[x] = 0;
//		i->aLogY[x] = 0;
//		i->aLogZ[x] = 0;
//	}
}

void gyro_cs(char state) {
	if (state == 1)
		//disable gyro reading
		GPIO_WriteBit(GPIOC, GPIO_Pin_7, 1);
	else
		//enable gyro reading
		GPIO_WriteBit(GPIOC, GPIO_Pin_7, 0);
}

void accel_cs(char state) {
	if (state == 1)
		//disable accel reading
		GPIO_WriteBit(GPIOB, GPIO_Pin_12, 1);
	else
		//enable accel reading
		GPIO_WriteBit(GPIOB, GPIO_Pin_12, 0);
}

uint8_t wai_g(void) {
	uint8_t data;
	gyro_cs(0);
	data = spi_send(0b10001111);
	data = data << 8;
	data |= spi_send(0);
	gyro_cs(1);
	return data;
}

uint8_t wai_a(void) {
	uint8_t data;
	accel_cs(0);
	data = spi_send(0b10001111);
	data = data << 8;
	data |= spi_send(0b00000000);
	accel_cs(1);
	return data;
}

void config_accel(Imu *i) {
	accel_cs(0);
	spi_send(0x60);
	spi_send(0b10100111); //1600hz data rate
	spi_send(0b11001000); // +/- 4g scale
	accel_cs(1);
	//i->aScale = 0.061; // for +/- 2g
	i->aScale = 0.00122; // for +/- 4g
	//i->aScale = 0.244; // for +/- 8g
	//i->aScale = 0.732; // for +/- 16g
}

void config_gyro(Imu *i) {
	gyro_cs(0);
	spi_send(0x60);
	spi_send(0b00001111); // ctrl reg 1 odr = 760 hz, cutoff = 100
	spi_send(0b00100000); //ctrl reg 2
	spi_send(0b00000000); // ctrl reg 3
	spi_send(0b00010000); //ctrl reg 4 500 degrees per second
	gyro_cs(1);
	//i->gScale = 8.75; // for +/- 245 dps
	i->gScale = 0.175; // for +/- 500 dps
	//i->gScale = 70.0; // for +/- 2000 dps
}

void get_accel(Imu *i) {
	uint8_t temp[6];
	accel_cs(0);
	spi_send(0xe8); // accel read register
	temp[0] = spi_send(0x00); //x low
	temp[1] = spi_send(0x00); //x high
	temp[2] = spi_send(0x00); //y low
	temp[3] = spi_send(0x00); //y high
	temp[4] = spi_send(0x00); //z low
	temp[5] = spi_send(0x00); //z high
	accel_cs(1);
	i->aRawX = (temp[1] << 8) | temp[0];
	i->aRawY = (temp[3] << 8) | temp[2];
	i->aRawZ = (temp[5] << 8) | temp[4];

//	i->aLogX[i->sampleCount] = (temp[1] << 8) | temp[0];
//	i->aLogY[i->sampleCount] = (temp[3] << 8) | temp[2];
//	i->aLogZ[i->sampleCount] = (temp[5] << 8) | temp[4];
//	i->sampleCount++;
//	if(i->sampleCount >= 3)
//		i->sampleCount = 0;
//	i->aRawX = i->aLogX[0] + i->aLogX[1] + i->aLogX[2];
//	i->aRawY = i->aLogY[0] + i->aLogY[1] + i->aLogY[2];
//	i->aRawZ = i->aLogZ[0] + i->aLogZ[1] + i->aLogZ[2];
//	i->aRawX = i->aRawX/SAMPLES;
//	i->aRawY = i->aRawY/SAMPLES;
//	i->aRawZ = i->aRawZ/SAMPLES;
}

void get_gyro(Imu *i) {
	uint8_t temp[6];
	gyro_cs(0);
	spi_send(0xe8);
	temp[0] = spi_send(0x00); //x low
	temp[1] = spi_send(0x00); //x high
	temp[2] = spi_send(0x00); //y low
	temp[3] = spi_send(0x00); //y high
	temp[4] = spi_send(0x00); //z low
	temp[5] = spi_send(0x00); //z high
	gyro_cs(1);
	i->gRawX = (temp[1] << 8) | temp[0];
	i->gRawY = (temp[3] << 8) | temp[2];
	i->gRawZ = (temp[5] << 8) | temp[4];
	i->gRawPitch = ((float) i->gRawX * i->gScale) * IMUDT;
	i->gRawRoll = ((float) i->gRawY * i->gScale) * IMUDT;
}

void calc_pitchRoll(Imu *i) {
	i->aGRawX = i->aRawX * i->aScale;
	i->aGRawY = i->aRawY * i->aScale;
	i->aGRawZ = i->aRawZ * i->aScale;
	i->aRawPitch = (atan2(-i->aGRawY, i->aGRawZ) * 180.0) / 3.14;
	i->aRawRoll = (atan2(i->aGRawX,
			sqrt(i->aGRawY * i->aGRawY + i->aGRawZ * i->aGRawZ)) * 180.0)
			/ 3.14;
	i->aRawPitch = (i->aRawPitch >= 0) ? (180 - i->aRawPitch) : (-i->aRawPitch - 180);
}

void compFilt_pitchRoll(Imu *i) {
	i->compFiltPitch = (i->aRawPitch * 0.02) + ((i->compFiltPitch + i->gRawPitch) * 0.98);
	i->compFiltRoll = (i->aRawRoll * 0.02) + ((i->compFiltRoll + i->gRawRoll) * 0.98);
}

void avgFilt_zAccel(Imu *i){
	i->aLogZ[i->sampleCount] = i->aGRawZ;
	i->sampleCount++;
	if(i->sampleCount > 2)
		i->sampleCount = 0;
	i->avgAZ = (i->aLogZ[0] + i->aLogZ[1] + i->aLogZ[2])/3;
}

void disp_aRaw(Imu *i) {
	printf("%i %i %i\r\n", i->aRawX, i->aRawY, i->aRawZ);
}

void disp_gRaw(Imu *i) {
	printf("%i %i %i\r\n", i->gRawX, i->gRawY, i->gRawZ);
}

void disp_aConv(Imu *i) {
	printf("%.2f %.2f %.2f\r\n", i->gRawX * 0.122, i->gRawY * 0.122,
			i->gRawZ * 0.122);
}

void disp_agRaw(Imu *i) {
	printf("%i %i %i %i %i %i\r\n", i->aRawX, i->aRawY, i->aRawZ, i->gRawX,
			i->gRawY, i->gRawZ);
}

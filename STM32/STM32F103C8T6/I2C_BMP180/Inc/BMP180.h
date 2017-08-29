#ifndef __BMP180__H__
#define __BMP180__H__

#include "stm32f1xx_hal.h"

#define BMP180_I2CADDR (0x77 << 1) // 1110111x
#define BMP180_TIMEOUT 100

#define BMP180_READTEMPCMD 0x2E

#define BMP180_CAL_AC1 0xAA // R   Calibration data (16 bits)
#define BMP180_CAL_AC2 0xAC // R   Calibration data (16 bits)
#define BMP180_CAL_AC3 0xAE // R   Calibration data (16 bits)
#define BMP180_CAL_AC4 0xB0 // R   Calibration data (16 bits)
#define BMP180_CAL_AC5 0xB2 // R   Calibration data (16 bits)
#define BMP180_CAL_AC6 0xB4 // R   Calibration data (16 bits)
#define BMP180_CAL_B1 0xB6  // R   Calibration data (16 bits)
#define BMP180_CAL_B2 0xB8  // R   Calibration data (16 bits)
#define BMP180_CAL_MB 0xBA  // R   Calibration data (16 bits)
#define BMP180_CAL_MC 0xBC  // R   Calibration data (16 bits)
#define BMP180_CAL_MD 0xBE  // R   Calibration data (16 bits)

#define BMP180_WHO_AM_I 0xD0 // WHO_AM_I id of BMP180, should return 0x55
#define BMP180_RESET 0xE0

#define BMP180_CONTROL 0xF4
#define BMP180_TEMPDATA 0xF6
#define BMP180_PRESSUREDATA 0xF6

#define BMP180_READPRESSURECMD 0x34
#define BMP180_RESETCMD 0xB6
#define BMP180_CHIPID 0x55

enum OSS
{              // BMP-085 sampling rate
    OSS_0 = 0, // 4.5 ms conversion time
    OSS_1,     // 7.5
    OSS_2,     // 13.5
    OSS_3      // 25.5
};

//uint8_t OSS = OSS_3; // maximum pressure resolution
HAL_StatusTypeDef BMP180_Reset(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef BMP180_CheckId(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef BMP180_Calibration(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef BMP180_GetTemperature(I2C_HandleTypeDef *hi2c, int16_t *pTemperature);
HAL_StatusTypeDef BMP180_GetPressure(I2C_HandleTypeDef *hi2c, long *pPressure);

#endif

//=============================================================================
// File Name    : BMP180.c
//
// Title        : BMP180 気圧・気温計モジュールドライバ
// Revision     : 0.1
// Notes        :
// Target MCU   :
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2017/05/05   ばんと      作成開始
// 2017/05/15   ばんと      作成完了
//=============================================================================

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include "BMP180.h"

/* local typedef -------------------------------------------------------------*/
/* local define --------------------------------------------------------------*/
/* local macro ---------------------------------------------------------------*/
/* local variables -----------------------------------------------------------*/
static int16_t ac1;
static int16_t ac2;
static int16_t ac3;
static uint16_t ac4;
static uint16_t ac5;
static uint16_t ac6;
static int16_t b1;
static int16_t b2;
static int16_t mb;
static int16_t mc;
static int16_t md;

static uint8_t oss = OSS_3; // maximum pressure resolution

/* local function prototypes -------------------------------------------------*/

/*======================================*/
/*                                      */
/*======================================*/
static HAL_StatusTypeDef BMP180_CalcB5(I2C_HandleTypeDef *hi2c, int16_t *b5)
{
    int16_t ut;
    uint8_t cmd;
    uint8_t buf[2];
    long x1, x2;
    HAL_StatusTypeDef retVal;

    cmd = BMP180_READTEMPCMD;
    retVal = HAL_I2C_Mem_Write(hi2c, BMP180_I2CADDR, BMP180_CONTROL, I2C_MEMADD_SIZE_8BIT, &cmd, 1, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;

    HAL_Delay(5); /* 5ms wait */

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_TEMPDATA, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;

    ut = ((int16_t)buf[0] << 8) | buf[1];

    x1 = (((long)ut - (long)ac6) * (long)ac5) >> 15;
    x2 = ((long)mc << 11) / (x1 + md);
    *b5 = x1 + x2;

    return retVal;
}

/*======================================*/
/*                                      */
/*======================================*/
HAL_StatusTypeDef BMP180_Reset(I2C_HandleTypeDef *hi2c)
{
    uint8_t cmd;

    cmd = BMP180_RESETCMD;
    return HAL_I2C_Mem_Write(hi2c, BMP180_I2CADDR, BMP180_RESET, I2C_MEMADD_SIZE_8BIT, &cmd, 1, BMP180_TIMEOUT);
}

/*======================================*/
/*                                      */
/*======================================*/
HAL_StatusTypeDef BMP180_CheckId(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef retVal;
    uint8_t buf;

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_WHO_AM_I, I2C_MEMADD_SIZE_8BIT, &buf, 1, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;

    if (buf != BMP180_CHIPID)
        retVal = HAL_ERROR;

    return retVal;
}

/*======================================*/
/*                                      */
/*======================================*/
HAL_StatusTypeDef BMP180_Calibration(I2C_HandleTypeDef *hi2c)
{
    HAL_StatusTypeDef retVal;
    uint8_t buf[2];

    retVal = BMP180_CheckId(hi2c);
    if (retVal != HAL_OK)
        return retVal;

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_AC1, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        ac1 = ((int16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_AC2, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        ac2 = ((int16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_AC3, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        ac3 = ((int16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_AC4, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        ac4 = ((uint16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_AC5, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        ac5 = ((uint16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_AC6, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        ac6 = ((uint16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_B1, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        b1 = ((int16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_B2, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        b2 = ((int16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_MB, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        mb = ((int16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_MC, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        mc = ((int16_t)buf[0] << 8) | buf[1];

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_CAL_MD, I2C_MEMADD_SIZE_8BIT, buf, 2, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;
    else
        md = ((int16_t)buf[0] << 8) | buf[1];

    return retVal;
}

// Temperature returned will be in units of 0.1 deg C
HAL_StatusTypeDef BMP180_GetTemperature(I2C_HandleTypeDef *hi2c, int16_t *pTemperature)
{
    int16_t b5;
    HAL_StatusTypeDef retVal;

    retVal = BMP180_CalcB5(hi2c, &b5);
    if (retVal != HAL_OK)
        return retVal;

    *pTemperature = (b5 + 8) >> 4;

    return retVal;
}

// Calculate pressure read calibration values
// b5 is also required so BMP180GetTemperature() must be called first.
// Value returned will be pressure in units of Pa.
HAL_StatusTypeDef BMP180_GetPressure(I2C_HandleTypeDef *hi2c, long *pPressure)
{
    long up;
    long x1, x2, x3, b3, b6, p;
    int16_t b5;
    unsigned long b4, b7;
    uint8_t cmd;
    uint8_t buf[3];
    HAL_StatusTypeDef retVal;

    cmd = BMP180_READPRESSURECMD | (oss << 6);
    retVal = HAL_I2C_Mem_Write(hi2c, BMP180_I2CADDR, BMP180_CONTROL, I2C_MEMADD_SIZE_8BIT, &cmd, 1, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;

    if (oss == OSS_0)
        HAL_Delay(5);
    else if (oss == OSS_1)
        HAL_Delay(8);
    else if (oss == OSS_2)
        HAL_Delay(14);
    else
        HAL_Delay(26);

    retVal = HAL_I2C_Mem_Read(hi2c, BMP180_I2CADDR, BMP180_PRESSUREDATA, I2C_MEMADD_SIZE_8BIT, buf, 3, BMP180_TIMEOUT);
    if (retVal != HAL_OK)
        return retVal;

    up = (((long)buf[0] << 16) | ((long)buf[1] << 8) | buf[2]) >> (8 - oss);

    // Calculate B5 & B6
    retVal = BMP180_CalcB5(hi2c, &b5);
    if (retVal != HAL_OK)
        return retVal;

    b6 = b5 - 4000;

    // Calculate B3
    x1 = (b2 * (b6 * b6) >> 12) >> 11;
    x2 = (ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = (((((long)ac1) * 4 + x3) << oss) + 2) >> 2;

    // Calculate B4
    x1 = (ac3 * b6) >> 13;
    x2 = (b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (ac4 * (unsigned long)(x3 + 32768)) >> 15;

    b7 = ((unsigned long)(up - b3) * (50000 >> oss));
    if (b7 < 0x80000000)
        p = (b7 << 1) / b4;
    else
        p = (b7 / b4) << 1;

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p += (x1 + x2 + 3791) >> 4;

    *pPressure = p;

    return retVal;
}

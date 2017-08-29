//=============================================================================
// File Name    : L3GD20.c
//
// Title        : L3GD20(SPI) ジャイロセンサ・モジュールドライバ
// Revision     : 0.1
// Notes        :
// Target MCU   :
// Tool Chain   :
//
// Revision History:
// When         Who         Description of change
// -----------  ----------- -----------------------
// 2017/05/16   ばんと      作成開始
// 2017/05/18   ばんと      作成完了
//=============================================================================

/* Includes ------------------------------------------------------------------*/
#include "L3GD20.h"

/* local typedef -------------------------------------------------------------*/
/* local define --------------------------------------------------------------*/
/* local macro ---------------------------------------------------------------*/
/* local variables -----------------------------------------------------------*/
static L3GD20_RANGE_t range = L3DS20_RANGE_250DPS;

/* local function prototypes -------------------------------------------------*/

HAL_StatusTypeDef L3GD20_ReadRegister(SPI_HandleTypeDef *hspi, uint8_t regAddr,
                                      uint8_t *readValue)
{
    HAL_StatusTypeDef retVal;
    uint8_t address = 0;
    //    uint8_t readValue = 0;

    HAL_GPIO_WritePin(L3GD20_CS_GPIO_Port, L3GD20_CS_Pin, GPIO_PIN_RESET);
    address = regAddr | L3GD20_REG_READ;
    retVal = HAL_SPI_Transmit(hspi, &address, 1, L3GD20_SPI_TIMEOUT);
    if (retVal == HAL_OK)
    {
        address = 0x00; // 00000000(DUMMY)
        retVal = HAL_SPI_TransmitReceive(hspi, &address, readValue, 1,
                                         L3GD20_SPI_TIMEOUT);
    }
    HAL_GPIO_WritePin(L3GD20_CS_GPIO_Port, L3GD20_CS_Pin, GPIO_PIN_SET);

    return retVal;
}

HAL_StatusTypeDef L3GD20_WriteRegister(SPI_HandleTypeDef *hspi, uint8_t RegAddr,
                                       uint8_t data)
{
    HAL_StatusTypeDef retVal;
    uint8_t address = 0;
    uint8_t sendValue = 0;

    HAL_GPIO_WritePin(L3GD20_CS_GPIO_Port, L3GD20_CS_Pin, GPIO_PIN_RESET);
    address = RegAddr | L3GD20_REG_WRITE;
    retVal = HAL_SPI_Transmit(hspi, &address, 1, L3GD20_SPI_TIMEOUT);
    if (retVal == HAL_OK)
    {
        sendValue = data;
        retVal = HAL_SPI_Transmit(hspi, &sendValue, 1, L3GD20_SPI_TIMEOUT);
    }
    HAL_GPIO_WritePin(L3GD20_CS_GPIO_Port, L3GD20_CS_Pin, GPIO_PIN_SET);

    return retVal;
}

HAL_StatusTypeDef L3GD20_Init(SPI_HandleTypeDef *hspi, L3GD20_RANGE_t _range)
{
    HAL_StatusTypeDef retVal;
    uint8_t id;

    id = 0;
    retVal = HAL_OK;
    range = _range;
    // IDチェック
    if ((retVal = L3GD20_ReadRegister(hspi, L3GD20_WHO_AM_I, &id)) != HAL_OK)
    {
        return retVal;
    }
    if (L3GD20_ID != id)
    {
        return HAL_ERROR;
    }

    /* Set CTRL_REG1 (0x20)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
   7-6  DR1/0     Output data rate                                   00
   5-4  BW1/0     Bandwidth selection                                00
     3  PD        0 = Power-down mode, 1 = normal/sleep mode          0
     2  ZEN       Z-axis enable (0 = disabled, 1 = enabled)           1
     1  YEN       Y-axis enable (0 = disabled, 1 = enabled)           1
     0  XEN       X-axis enable (0 = disabled, 1 = enabled)           1 */

    /* Switch to normal mode and enable all three channels */
    if ((retVal = L3GD20_WriteRegister(hspi, L3GD20_CTRL_REG1, 0x0F)) != HAL_OK)
    {
        return retVal;
    }

    /* ------------------------------------------------------------------ */

    /* Set CTRL_REG2 (0x21)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
   5-4  HPM1/0    High-pass filter mode selection                    00
   3-0  HPCF3..0  High-pass filter cutoff frequency selection      0000 */

    /* Nothing to do ... keep default values */
    /* ------------------------------------------------------------------ */

    /* Set CTRL_REG3 (0x22)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
     7  I1_Int1   Interrupt enable on INT1 (0=disable,1=enable)       0
     6  I1_Boot   Boot status on INT1 (0=disable,1=enable)            0
     5  H-Lactive Interrupt active config on INT1 (0=high,1=low)      0
     4  PP_OD     Push-Pull/Open-Drain (0=PP, 1=OD)                   0
     3  I2_DRDY   Data ready on DRDY/INT2 (0=disable,1=enable)        0
     2  I2_WTM    FIFO wtrmrk int on DRDY/INT2 (0=dsbl,1=enbl)        0
     1  I2_ORun   FIFO overrun int on DRDY/INT2 (0=dsbl,1=enbl)       0
     0  I2_Empty  FIFI empty int on DRDY/INT2 (0=dsbl,1=enbl)         0 */

    /* Nothing to do ... keep default values */
    /* ------------------------------------------------------------------ */

    /* Set CTRL_REG4 (0x23)
   ====================================================================
   BIT  Symbol    Description                                   Default
   ---  ------    --------------------------------------------- -------
     7  BDU       Block Data Update (0=continuous, 1=LSB/MSB)         0
     6  BLE       Big/Little-Endian (0=Data LSB, 1=Data MSB)          0
   5-4  FS1/0     Full scale selection                               00
                                  00 = 250 dps
                                  01 = 500 dps
                                  10 = 2000 dps
                                  11 = 2000 dps
     0  SIM       SPI Mode (0=4-wire, 1=3-wire)                       0 */

    /* Adjust resolution if requested */
    switch (range)
    {
        case L3DS20_RANGE_250DPS:
            retVal = L3GD20_WriteRegister(hspi, L3GD20_CTRL_REG4, 0x00);
            break;
        case L3DS20_RANGE_500DPS:
            retVal = L3GD20_WriteRegister(hspi, L3GD20_CTRL_REG4, 0x10);
            break;
        case L3DS20_RANGE_2000DPS:
            retVal = L3GD20_WriteRegister(hspi, L3GD20_CTRL_REG4, 0x20);
            break;
    }
    /* ------------------------------------------------------------------ */

    return retVal;
}

HAL_StatusTypeDef L3GD20_readXYZ(SPI_HandleTypeDef *hspi, float *x, float *y,
                                 float *z)
{
    int16_t out_x;
    int16_t out_y;
    int16_t out_z;
    uint8_t reg_h;
    uint8_t reg_l;
    HAL_StatusTypeDef retVal;

    retVal = HAL_OK;

    if ((retVal = L3GD20_ReadRegister(hspi, L3GD20_OUT_X_L, &reg_l)) != HAL_OK)
    {
        return retVal;
    }
    if ((retVal = L3GD20_ReadRegister(hspi, L3GD20_OUT_X_H, &reg_h)) != HAL_OK)
    {
        return retVal;
    }
    out_x = (reg_h << 8) | reg_l;

    if ((retVal = L3GD20_ReadRegister(hspi, L3GD20_OUT_Y_L, &reg_l)) != HAL_OK)
    {
        return retVal;
    }
    if ((retVal = L3GD20_ReadRegister(hspi, L3GD20_OUT_Y_H, &reg_h)) != HAL_OK)
    {
        return retVal;
    }
    out_y = (reg_h << 8) | reg_l;

    if ((retVal = L3GD20_ReadRegister(hspi, L3GD20_OUT_Z_L, &reg_l)) != HAL_OK)
    {
        return retVal;
    }
    if ((retVal = L3GD20_ReadRegister(hspi, L3GD20_OUT_Z_H, &reg_h)) != HAL_OK)
    {
        return retVal;
    }
    out_z = (reg_h << 8) | reg_l;

    switch (range)
    {
        case L3DS20_RANGE_250DPS:
            *x = (float)out_x * L3GD20_SENSITIVITY_250DPS;
            *y = (float)out_y * L3GD20_SENSITIVITY_250DPS;
            *z = (float)out_z * L3GD20_SENSITIVITY_250DPS;
            break;
        case L3DS20_RANGE_500DPS:
            *x = (float)out_x * L3GD20_SENSITIVITY_500DPS;
            *y = (float)out_y * L3GD20_SENSITIVITY_500DPS;
            *z = (float)out_z * L3GD20_SENSITIVITY_500DPS;
            break;
        case L3DS20_RANGE_2000DPS:
            *x = (float)out_x * L3GD20_SENSITIVITY_2000DPS;
            *y = (float)out_y * L3GD20_SENSITIVITY_2000DPS;
            *z = (float)out_z * L3GD20_SENSITIVITY_2000DPS;
            break;
    }

    return retVal;
}

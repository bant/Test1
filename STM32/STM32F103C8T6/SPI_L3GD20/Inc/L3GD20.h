#ifndef __L3GD20__H__
#define __L3GD20__H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#define L3GD20_CS_Pin GPIO_PIN_4
#define L3GD20_CS_GPIO_Port GPIOA

typedef enum {
    L3DS20_RANGE_250DPS = 0, // RANGE 250PS
    L3DS20_RANGE_500DPS,     //       500PS
    L3DS20_RANGE_2000DPS     //       20000PS
} L3GD20_RANGE_t;

#define L3GD20_WHO_AM_I 0x0F
#define L3GD20_CTRL_REG1 0x20
#define L3GD20_CTRL_REG2 0x21
#define L3GD20_CTRL_REG3 0x22
#define L3GD20_CTRL_REG4 0x23
#define L3GD20_CTRL_REG5 0x24
#define L3GD20_REFERENCE 0x25
#define L3GD20_OUT_TEMP 0x26
#define L3GD20_STATUS_REG 0x27
#define L3GD20_OUT_X_L 0x28
#define L3GD20_OUT_X_H 0x29
#define L3GD20_OUT_Y_L 0x2A
#define L3GD20_OUT_Y_H 0x2B
#define L3GD20_OUT_Z_L 0x2C
#define L3GD20_OUT_Z_H 0x2D
#define L3GD20_FIFO_CTRL_REG 0x2E
#define L3GD20_FIFO_SRC_REG 0x2F
#define L3GD20_INT1_CFG 0x30
#define L3GD20_INT1_SRC 0x31
#define L3GD20_INT1_TSH_XH 0x32
#define L3GD20_INT1_TSH_XL 0x33
#define L3GD20_INT1_TSH_YH 0x34
#define L3GD20_INT1_TSH_YL 0x35
#define L3GD20_INT1_TSH_ZH 0x36
#define L3GD20_INT1_TSH_ZL 0x37
#define L3GD20_INT1_DURATION 0x38

#define L3GD20_ID 0xD4

#define L3GD20_REG_READ 0x80
#define L3GD20_REG_WRITE 0x00
#define L3GD20_SPI_TIMEOUT 100

#define L3GD20_SENSITIVITY_250DPS \
    (0.00875F) // Roughly 22/256 for fixed point match
#define L3GD20_SENSITIVITY_500DPS (0.0175F) // Roughly 45/256
#define L3GD20_SENSITIVITY_2000DPS (0.070F) // Roughly 18/256
#define L3GD20_DPS_TO_RADS (0.017453293F)   // degress/s to rad/s multiplier

HAL_StatusTypeDef L3GD20_ReadRegister(SPI_HandleTypeDef *hspi, uint8_t regAddr,
                                      uint8_t *readValue);
HAL_StatusTypeDef L3GD20_WriteRegister(SPI_HandleTypeDef *hspi, uint8_t RegAddr,
                                       uint8_t data);

HAL_StatusTypeDef L3GD20_Init(SPI_HandleTypeDef *hspi, L3GD20_RANGE_t _range);
HAL_StatusTypeDef L3GD20_readXYZ(SPI_HandleTypeDef *hspi, float *x, float *y,
                                 float *z);

#ifdef __cplusplus
}
#endif

#endif /* __L3GD20__H__ */

/*
 * BMP180.h
 *
 *  Created on: Jan 20, 2021
 *      Author: liyang
 */

#ifndef INC_BMP180_H_
#define INC_BMP180_H_
#include "stdint.h"
#include "i2c.h"
#include "stm32f1xx_hal_i2c.h"
//地址定义
#define BMP180_I2C_ADDR					0x77
#define BMP180_I2C_TIMEOUT				1000
#define BMP180_CONTROL_REG				0xf4
#define BMP180_MSB_REG					0xf6
#define BMP180_LSB_REG					0xf7
#define BMP180_XLSB_REG					0xf8
#define BMP180_SOFT_RESET_REG			0xe0
#define BMP180_SOFT_RESET				0xb6
//超采样四个选项 结构体
typedef enum BMP180_OSS {
	BMP180_LOW, BMP180_STANDARD, BMP180_HIGH, BMP180_ULTRA,
} BMP180_OSS;

//校正参数枚举
typedef enum BMP180_CalIndex {
	BMP180_INDEX_AC1,
	BMP180_INDEX_AC2,
	BMP180_INDEX_AC3,
	BMP180_INDEX_AC4,
	BMP180_INDEX_AC5,
	BMP180_INDEX_AC6,
	BMP180_INDEX_B1,
	BMP180_INDEX_B2,
	BMP180_INDEX_MB,
	BMP180_INDEX_MC,
	BMP180_INDEX_MD,
} BMP180_CalIndex;

//校正参数结构体
typedef struct BMP180_EEPROM {
	short BMP180_AC1;
	short BMP180_AC2;
	short BMP180_AC3;
	unsigned short BMP180_AC4;
	unsigned short BMP180_AC5;
	unsigned short BMP180_AC6;
	short BMP180_B1;
	short BMP180_B2;
	short BMP180_MB;
	short BMP180_MC;
	short BMP180_MD;
} BMP180_EEPROM;

//EEPROM结构体
extern BMP180_EEPROM _bmp180_eeprom;

//超采样 结构体
extern BMP180_OSS _bmp180_oss;

//校正参数地址高八位
extern const uint8_t BMP180_EEPROM_ADDR_MSB[11];
//校正参数地址高八位
extern const uint8_t BMP180_EEPROM_ADDR_LSB[11];
//温度命令
extern const uint8_t BMP180_CMD_TEMP;
//温度等待
extern const uint8_t BMP180_DELAY_TEMP;
//气压命令
extern const uint8_t BMP180_CMD_PRES[4];
//气压等待
extern const uint8_t BMP180_DELAY_PRES[4];

//超采样参数设置
void BMP180_SetOversampling(BMP180_OSS oss);

//更新校正数据
void BMP180_UpdateCalibrationData(void);

//向寄存器写入命令
void BMP180_WriteReg(uint8_t reg, uint8_t cmd);

//读取寄存器数据
uint8_t BMP180_ReadReg(uint8_t reg);

//获取温度数据
int32_t BMP180_GetRawTemperature(void);

//获取气压数据
int32_t BMP180_GetPressure(void);

//计算摄氏度温度值
float BMP180_GetTemperature(void);

//获取UT
int32_t BMP180_GetUT(void);
//获取UP
int32_t BMP180_GetUP(void);












#endif /* INC_BMP180_H_ */

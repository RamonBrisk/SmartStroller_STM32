/*
 * BMP180.c
 *
 *  Created on: Jan 20, 2021
 *      Author: liyang
 */

#include "BMP180.h"
#include "main.h"
#include "stdio.h"

BMP180_EEPROM _bmp180_eeprom;
BMP180_OSS _bmp180_oss;

//校正参数地址高八位
const uint8_t BMP180_EEPROM_ADDR_MSB[11] =
{ 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe };

//校正参数地址低八位
const uint8_t BMP180_EEPROM_ADDR_LSB[11] =
{ 0xab, 0xad, 0xaf, 0xb1, 0xb3, 0xb5, 0xb7, 0xb9, 0xbb, 0xbd, 0xbf };

//温度超采样设置值
const uint8_t BMP180_CMD_TEMP = 0x2e;
const uint8_t BMP180_DELAY_TEMP = 5;
//压力值超采样的四个设置值
const uint8_t BMP180_CMD_PRES[4] = { 0x34, 0x74, 0xb4, 0xf4 };
const uint8_t BMP180_DELAY_PRES[4] = { 5, 8, 14, 26 };


//设置超采样
void BMP180_SetOversampling(BMP180_OSS oss) {
	_bmp180_oss = oss;
}


//更新校正数据
void BMP180_UpdateCalibrationData(void) {
	_bmp180_eeprom.BMP180_AC1 = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_AC1]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_AC1]);
	_bmp180_eeprom.BMP180_AC2 = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_AC2]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_AC2]);
	_bmp180_eeprom.BMP180_AC3 = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_AC3]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_AC3]);
	_bmp180_eeprom.BMP180_AC4 = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_AC4]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_AC4]);
	_bmp180_eeprom.BMP180_AC5 = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_AC5]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_AC5]);
	_bmp180_eeprom.BMP180_AC6 = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_AC6]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_AC6]);
	_bmp180_eeprom.BMP180_B1 = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_B1]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_B1]);
	_bmp180_eeprom.BMP180_B2 = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_B2]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_B2]);
	_bmp180_eeprom.BMP180_MB = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_MB]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_MB]);
	_bmp180_eeprom.BMP180_MC = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_MC]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_MC]);
	_bmp180_eeprom.BMP180_MD = (BMP180_ReadReg(BMP180_EEPROM_ADDR_MSB[BMP180_INDEX_MD]) << 8) | BMP180_ReadReg(BMP180_EEPROM_ADDR_LSB[BMP180_INDEX_MD]);
}


//写入寄存器
void BMP180_WriteReg(uint8_t reg, uint8_t cmd) {
	uint8_t arr[2] = { reg, cmd };
	HAL_I2C_Master_Transmit(&hi2c1, BMP180_I2C_ADDR << 1, arr, 2, BMP180_I2C_TIMEOUT);
}

//读取寄存器
uint8_t BMP180_ReadReg(uint8_t reg) {
	//使用HAL_I2C_Mem_Write等于先使用HAL_I2C_Master_Transmit传输第一个寄存器地址，
	//再用HAL_I2C_Master_Transmit传输写入第一个寄存器的数据。可以传输多个数据
	//以下先把要接受的寄存器地址传给从机
	//&reg 本来就要求传一个指向一个地址值的指针
	HAL_I2C_Master_Transmit(&hi2c1, BMP180_I2C_ADDR << 1, &reg, 1, BMP180_I2C_TIMEOUT);
	uint8_t result;
	HAL_I2C_Master_Receive(&hi2c1, BMP180_I2C_ADDR << 1, &result, 1, BMP180_I2C_TIMEOUT);



	//HAL_I2C_Mem_Read(I2C1, BMP180_I2C_ADDR << 1, &reg, 1,result, 1, BMP180_I2C_TIMEOUT);
	return result;


}

/**
 * @brief Measures and calculates temperature.
 * @return Temperature in 0.1 (1/10) degrees Celsius.
 */
int32_t BMP180_GetRawTemperature(void) {
	//write 0x2E to reg 0xF4
	//设置温度4.5ms超采样
	BMP180_WriteReg(BMP180_CONTROL_REG, BMP180_CMD_TEMP);
	//等待4.5ms 以上
	HAL_Delay(BMP180_DELAY_TEMP);

	int32_t ut = (BMP180_ReadReg(BMP180_MSB_REG) << 8) | BMP180_ReadReg(BMP180_LSB_REG);
	int32_t x1 = (ut - _bmp180_eeprom.BMP180_AC6) * _bmp180_eeprom.BMP180_AC5 / (1 << 15);
	int32_t x2 = (_bmp180_eeprom.BMP180_MC * (1 << 11)) / (x1 + _bmp180_eeprom.BMP180_MD);
	int32_t b5 = x1 + x2;
	return (b5 + 8) / (1 << 4);
}



/**
 * @brief Measures and calculates temperature.
 * @return Temperature in degrees Celsius.
 */
float BMP180_GetTemperature(void) {
	int32_t temp = BMP180_GetRawTemperature();
	return temp / 10.0;
}

/**
 * @brief Measures and calculates pressure.
 * @return Pressure in Pascal(Pa).
 */
int32_t BMP180_GetPressure(void) {
	//往控制寄存器0xf4里写0x2e，设置温度超采样
	BMP180_WriteReg(BMP180_CONTROL_REG, BMP180_CMD_TEMP);
	//等待转换时间
	HAL_Delay(BMP180_DELAY_TEMP);
	int32_t ut = BMP180_GetUT();
	BMP180_WriteReg(BMP180_CONTROL_REG, BMP180_CMD_PRES[_bmp180_oss]);
	HAL_Delay(BMP180_DELAY_PRES[_bmp180_oss]);
	int32_t up = BMP180_GetUP();
	int32_t x1 = (ut - _bmp180_eeprom.BMP180_AC6) * _bmp180_eeprom.BMP180_AC5 / (1 << 15);
	int32_t x2 = (_bmp180_eeprom.BMP180_MC * (1 << 11)) / (x1 + _bmp180_eeprom.BMP180_MD);
	int32_t b5 = x1 + x2;
	int32_t b6 = b5 - 4000;
	x1 = (_bmp180_eeprom.BMP180_B2 * (b6 * b6 / (1 << 12))) / (1 << 11);
	x2 = _bmp180_eeprom.BMP180_AC2 * b6 / (1 << 11);
	int32_t x3 = x1 + x2;
	int32_t b3 = (((_bmp180_eeprom.BMP180_AC1 * 4 + x3) << _bmp180_oss) + 2) / 4;
	x1 = _bmp180_eeprom.BMP180_AC3 * b6 / (1 << 13);
	x2 = (_bmp180_eeprom.BMP180_B1 * (b6 * b6 / (1 << 12))) / (1 << 16);
	x3 = ((x1 + x2) + 2) / 4;
	uint32_t b4 = _bmp180_eeprom.BMP180_AC4 * (uint32_t) (x3 + 32768) / (1 << 15);
	uint32_t b7 = ((uint32_t) up - b3) * (50000 >> _bmp180_oss);
	int32_t p;
	if (b7 < 0x80000000)
		p = (b7 * 2) / b4;
	else
		p = (b7 / b4) * 2;
	x1 = (p / (1 << 8)) * (p / (1 << 8));
	x1 = (x1 * 3038) / (1 << 16);
	x2 = (-7357 * p) / (1 << 16);
	p = p + (x1 + x2 + 3791) / (1 << 4);
	return p;
}

//温度原始数据
int32_t BMP180_GetUT(void){
	return (BMP180_ReadReg(BMP180_MSB_REG) << 8) | BMP180_ReadReg(BMP180_LSB_REG);
}
//压力原始数据
int32_t BMP180_GetUP(void){
	return ((BMP180_ReadReg(BMP180_MSB_REG) << 16) | (BMP180_ReadReg(BMP180_LSB_REG) << 8) | BMP180_ReadReg(BMP180_XLSB_REG)) >> (8 - _bmp180_oss);
}










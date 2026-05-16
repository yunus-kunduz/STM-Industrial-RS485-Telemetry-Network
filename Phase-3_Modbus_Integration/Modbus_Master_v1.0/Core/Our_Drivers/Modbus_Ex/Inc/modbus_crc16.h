/*
 * modbus_crc16.h
 *
 *  Created on: May 10, 2026
 *      Author: theyu
 */

#ifndef OUR_DRIVERS_MODBUS_EX_INC_MODBUS_CRC16_H_
#define OUR_DRIVERS_MODBUS_EX_INC_MODBUS_CRC16_H_

#include "stdint.h"

uint16_t CRC16_Calculate(uint16_t crcVal, uint8_t data);
uint16_t CRC16_Calculate_With_LookUp_Table(uint8_t *buffer, uint16_t length);

#endif /* OUR_DRIVERS_MODBUS_EX_INC_MODBUS_CRC16_H_ */

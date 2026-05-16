/*
 * modbus_ex.h
 *
 *  Created on: Sep 13, 2023
 *      Author: mfati
 */

#ifndef OUR_DRIVERS_MODBUS_EX_INC_MODBUS_EX_H_
#define OUR_DRIVERS_MODBUS_EX_INC_MODBUS_EX_H_

#include "main.h"
#include "timer_ex.h"
#include "modbus_crc16.h"
#include "ring_buffer.h"
#include "stdbool.h"
#include "string.h"

#define MODBUS_RX_MAX_SIZE		64
#define MODBUS_TX_MAX_SIZE		252

typedef enum
{
	Read_Coil_Status			= 0x01,
	Read_Input_Status			= 0x02,
	Read_Holding_Registers		= 0x03,
	Read_Input_Registers		= 0x04,
	Write_Single_Coil			= 0x05,
	Write_Single_Register		= 0x06,
	Write_Multiple_Coils		= 0x0F,
	Write_Multiple_Registers	= 0x10
}Modbus_Funciton_Code_t;

typedef enum
{
	ASCII,
	RTU
}Modbus_Mode_t;

typedef enum
{
	Coil_Out_Status_Off			= 0x0000,
	Coil_Out_Status_On			= 0xFF00
}Modbus_Coil_Status_t;

typedef struct
{
	uint8_t 				slaveAddress;
	Modbus_Funciton_Code_t	funtionCode;
	uint16_t 				startAddress;
	uint16_t				numOfRegisters;
	uint16_t				writeData[MODBUS_TX_MAX_SIZE];
	uint8_t					messageLength;
	uint8_t					txData[MODBUS_TX_MAX_SIZE];
	uint8_t					txIndex;
	uint16_t				crc16;
	bool					newSendMessageStatus;
}Modbus_Tx_Message_t;

typedef struct
{
	uint8_t 				slaveAddress;
	uint8_t					functionCode;
	uint8_t					rxData[MODBUS_RX_MAX_SIZE];
	uint8_t					rxIndex;
	uint8_t					messageLength;
	Tick_Time_t				rxTime;
	uint16_t				crc16;
	bool					newReceiveMessageStatus;
}Modbus_Rx_Message_t;

typedef enum
{
	Idle					= 0,
	Send_Request			= 1,
	Waiting_For_Reply		= 2,
	Processing_Reply		= 3,
	Processing_Error		= 4
}Modbus_State_t;

typedef enum
{
	No_Error				= 0,
	Invalid_Function_Code	= 1,
	Invalid_Data_Address	= 2,
	Invalid_Data_Value	 	= 3,
	Slave_Device_Error		= 4,
	Info_Message			= 5,
	Invalid_CRC				= 6,
	Timeout					= 7,
}Modbus_Error_Code_t;

typedef struct
{
	Modbus_Tx_Message_t		txMessage;
	Modbus_Rx_Message_t		rxMessage;
	Modbus_Mode_t			mode;
	Modbus_State_t			state;
	Modbus_Error_Code_t		errorCode;
}Modbus_t;

void Modbus_Master_Control(Modbus_t *modbus);

uint8_t Modbus_Master_Write(uint8_t *buffer, uint8_t length);

void Modbus_Read_Coil_Status(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numberOfCoils);
void Modbus_Read_Input_Status(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numberOfInputs);
void Modbus_Read_Holding_Registers(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numberOfRegister);
void Modbus_Read_Input_Registers(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numberOfRegister);

void Modbus_Write_Singel_Coil(Modbus_t *modbus, uint8_t slaveId, uint16_t numOfCoil, Modbus_Coil_Status_t status);
void Modbus_Write_Single_Register(Modbus_t *modbus, uint8_t slaveId, uint16_t numOfRegister, uint16_t data);
void Modbus_Write_Multiple_Coils(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numOfCoils, uint16_t coilStatus);
void Modbus_Write_Multiple_Registers(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numOfRegisters, uint16_t* data);

bool areElementsEqual(uint8_t *txArray, uint8_t *rxArray, uint8_t numberOfElemets);

#endif /* OUR_DRIVERS_MODBUS_EX_INC_MODBUS_EX_H_ */

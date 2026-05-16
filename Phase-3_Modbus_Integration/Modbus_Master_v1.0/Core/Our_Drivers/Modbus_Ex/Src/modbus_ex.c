/*
 * modbus_ex.c
 *
 *  Created on: Sep 13, 2023
 *      Author: mfati
 */

#include "modbus_ex.h"
#include "math.h"

Tick_Time_t idleTime;
Tick_Time_t waitResponseTime;
Tick_Time_t errorTime;

#define LSB(x)				(uint8_t)(x & 0xFF)
#define MSB(x)				(uint8_t)(x >> 8)
#define bitRead(x, y)		(uint8_t)((x & (1 << y)) >> y)

extern Ring_Buffer_t usart2_InRb;
extern UART_HandleTypeDef huart2;

uint8_t cikis10Durum;
uint8_t giris1, giris2, giris3, giris4, giris5;

uint16_t analogValue[10];

uint8_t komutDurum;

/*
 * Modbus Master Kontrol Fonksiyonu
 */
void Modbus_Master_Control(Modbus_t *modbus)
{
	switch(modbus->state)
	{
	case Idle:
		// Boşta durumu : Mesaj göndermeye hazır hale gelecek
		if(modbus->txMessage.newSendMessageStatus && Tick_Timer_Check_Elapsed_Time(&idleTime))
		{
			// Bir mesaj gönderilmeye hazır
			modbus->txMessage.newSendMessageStatus = false;
			modbus->state 				= Send_Request;
			modbus->txMessage.txIndex 	= 0;
			Tick_Timer_Stop_Time(&idleTime);
		}

		break;

	case Send_Request:
		// Sorgu talebi gönder
		// ADU
		modbus->txMessage.txData[modbus->txMessage.txIndex++] = modbus->txMessage.slaveAddress;
		modbus->txMessage.txData[modbus->txMessage.txIndex++] = modbus->txMessage.funtionCode;

		switch(modbus->txMessage.funtionCode)
		{
		case Read_Coil_Status: 	// Data
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.numOfRegisters);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.numOfRegisters);
			break;

		case Read_Input_Status: 	// Data
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.numOfRegisters);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.numOfRegisters);

			break;

		case Read_Holding_Registers:
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.numOfRegisters);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.numOfRegisters);
			break;

		case Read_Input_Registers:
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.numOfRegisters);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.numOfRegisters);
			break;

		case Write_Single_Coil:

			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.writeData[0]);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.writeData[0]);

			break;

		case Write_Single_Register:

			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.writeData[0]);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.writeData[0]);

			break;

		case Write_Multiple_Coils:

			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.numOfRegisters);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.numOfRegisters);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.writeData[0]);	// Bayt Sayısı
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.writeData[1]);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.writeData[1]);

			break;

		case Write_Multiple_Registers:

			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.startAddress);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.numOfRegisters);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.numOfRegisters);
			modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.writeData[0]);	// Bayt Sayısı

			for(int i = 1; i <= modbus->txMessage.numOfRegisters; i++)
			{
				modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.writeData[i]);
				modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.writeData[i]);
			}

			break;
		}

		modbus->txMessage.crc16 = 0xFF;
		modbus->txMessage.crc16 = CRC16_Calculate_With_LookUp_Table(modbus->txMessage.txData, modbus->txMessage.txIndex);
		modbus->txMessage.txData[modbus->txMessage.txIndex++] = LSB(modbus->txMessage.crc16);
		modbus->txMessage.txData[modbus->txMessage.txIndex++] = MSB(modbus->txMessage.crc16);

		Ring_Buffer_Clear(&usart2_InRb);

		Modbus_Master_Write(modbus->txMessage.txData, modbus->txMessage.txIndex);

		Tick_Timer_Set_Time(&waitResponseTime, 1000);

		modbus->state = Waiting_For_Reply;

		break;

	case Waiting_For_Reply:
		// Cevap bekle
		if(usart2_InRb.writePoint > 2)
			modbus->rxMessage.newReceiveMessageStatus = true;

		if(modbus->rxMessage.newReceiveMessageStatus)
		{
			// Zamanı durdur ve Processing Reply
			// Gelen mesajın adres bilgisini al ve kontrol et
			modbus->rxMessage.slaveAddress = usart2_InRb.pBuffer[0];

			if(modbus->rxMessage.slaveAddress == modbus->txMessage.slaveAddress)
			{
				// Yanıt, beklenen bağımlı cihazdan geldi, mesajı işlemeye başla
				modbus->state = Processing_Reply;
				modbus->rxMessage.newReceiveMessageStatus = false;
			}
			else
			{
				// Gelen yanıt farklı bir bağımlı cihazdan geldi, yoksay ve beklemeye devam et
				// Ring Buffer Temizle
				Ring_Buffer_Clear(&usart2_InRb);
			}
		}
		else
		{
			if(Tick_Timer_Check_Elapsed_Time(&waitResponseTime))
			{
				modbus->errorCode = Timeout;
				modbus->state = Processing_Error;
				Tick_Timer_Set_Time(&errorTime, 1000);
			}
		}

		break;

	case Processing_Reply:
		// Beklenilen süre zarfında cevap doğru bir şekilde gelirse, işlenir
		// 1 - Gelen mesajın fonksiyon kodunu al
		modbus->rxMessage.functionCode = usart2_InRb.pBuffer[1];
		// 2 - Gelen fonksiyon kodunu kontrol et
		if((modbus->rxMessage.functionCode & 0x7F) != modbus->txMessage.funtionCode)
		{
			modbus->errorCode = Invalid_Function_Code;
			modbus->state	  = Processing_Error;
			Tick_Timer_Set_Time(&errorTime, 1000);
		}
		// İstisna Mesajı kontrolü yap
		if(bitRead(modbus->rxMessage.functionCode, 7))
		{
			modbus->errorCode 	= usart2_InRb.pBuffer[2];
			modbus->state 		= Processing_Error;
			Tick_Timer_Set_Time(&errorTime, 1000);
		}

		// Gelen data uzunluğunu al
		if(modbus->rxMessage.messageLength == 0 && modbus->rxMessage.functionCode <= Read_Input_Registers)
		{
			modbus->rxMessage.messageLength = usart2_InRb.pBuffer[2] + 5;
		}
		else if(modbus->rxMessage.functionCode >= Write_Single_Coil)
		{
			modbus->rxMessage.messageLength = modbus->txMessage.txIndex;

			if(modbus->rxMessage.functionCode == Write_Multiple_Coils || modbus->rxMessage.functionCode == Write_Multiple_Registers)
				modbus->rxMessage.messageLength = 8;
		}

		// Wait all data
		if((usart2_InRb.writePoint >= modbus->rxMessage.messageLength && modbus->rxMessage.messageLength != 0))
		{
			modbus->rxMessage.messageLength = 0;
			// CRC Kontrolü Yap
			modbus->rxMessage.crc16 = 0xFFFF;
			memcpy(modbus->rxMessage.rxData, usart2_InRb.pBuffer, usart2_InRb.writePoint);
			modbus->rxMessage.rxIndex = usart2_InRb.writePoint;
			modbus->rxMessage.crc16   = CRC16_Calculate_With_LookUp_Table(modbus->rxMessage.rxData, (modbus->rxMessage.rxIndex - 2));

			if(LSB(modbus->rxMessage.crc16) != modbus->rxMessage.rxData[modbus->rxMessage.rxIndex - 2] ||
			  (MSB(modbus->rxMessage.crc16) != modbus->rxMessage.rxData[modbus->rxMessage.rxIndex - 1]))
			{
				// CRC Error
				modbus->errorCode = Invalid_CRC;
				modbus->state 	  = Processing_Error;
			}

			// Tüm verileri doğru bir şekilde geldi ve bilgileri işle
			if(modbus->state != Processing_Error)
			{
				switch(modbus->rxMessage.functionCode)
				{
				case Read_Coil_Status:
					// . . .
					cikis10Durum = bitRead(modbus->rxMessage.rxData[3], 0);
					break;

				case Read_Input_Status:
					giris1 = bitRead(modbus->rxMessage.rxData[3], 0);
					giris2 = bitRead(modbus->rxMessage.rxData[3], 1);
					giris3 = bitRead(modbus->rxMessage.rxData[3], 2);
					giris4 = bitRead(modbus->rxMessage.rxData[3], 3);
					giris5 = bitRead(modbus->rxMessage.rxData[3], 4);
					break;

				case Read_Holding_Registers:

				/*	uint8_t dataLength = modbus->rxMessage.rxData[2] / 2;
					uint8_t index = 3;

					for(int i = 0; i < dataLength; i++)
					{						 	// LSB													// MSB
						analogValue[i] = (modbus->rxMessage.rxData[(index + 1) + i]) | (modbus->rxMessage.rxData[index + i] << 8);
						index += 1;
					}*/

					break;

				case Read_Input_Registers:

					uint8_t dataLength = modbus->rxMessage.rxData[2] / 2;
					uint8_t index = 3;

					for(int i = 0; i < dataLength; i++)
					{						 	// LSB													// MSB
						analogValue[i] = (modbus->rxMessage.rxData[(index + 1) + i]) | (modbus->rxMessage.rxData[index + i] << 8);
						index += 1;
					}

					break;

				case Write_Single_Coil:

					if(areElementsEqual(modbus->txMessage.txData, modbus->rxMessage.rxData, modbus->txMessage.txIndex) == true)
					{
						// Gönderiğimiz komut başarılı sonuçlandı
					}
					else
					{
						// Gönderdiğimiz komut çalışmadı
					}

					break;

				case Write_Single_Register:

					if(areElementsEqual(modbus->txMessage.txData, modbus->rxMessage.rxData, modbus->txMessage.txIndex) == true)
					{
						// Gönderiğimiz komut başarılı sonuçlandı
					}
					else
					{
						// Gönderdiğimiz komut çalışmadı
					}

					break;

				case Write_Multiple_Coils:

					if(modbus->rxMessage.rxData[5] == modbus->txMessage.numOfRegisters)
					{
						// Gönderiğimiz komut başarılı sonuçlandı
					}
					else
					{
						// Gönderdiğimiz komut çalışmadı
					}

					break;

				case Write_Multiple_Registers:

					if(modbus->rxMessage.rxData[5] == modbus->txMessage.numOfRegisters)
					{
						// Gönderiğimiz komut başarılı sonuçlandı
						komutDurum = 1;
					}
					else
					{
						// Gönderdiğimiz komut çalışmadı
						komutDurum = 2;
					}

					break;
				}

				modbus->state = Idle;
			}
		}

		break;

	case Processing_Error:
		// Hata durumu yönetimi
		if(Tick_Timer_Check_Elapsed_Time(&errorTime))
		{
			modbus->state = Idle;
		}

		break;

	default:

		break;
	}
}

uint8_t Modbus_Master_Write(uint8_t *buffer, uint8_t length)
{
	HAL_GPIO_WritePin(TX_RX_EN1_GPIO_Port, TX_RX_EN1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TX_RX_EN2_GPIO_Port, TX_RX_EN2_Pin, GPIO_PIN_SET);

	if(HAL_UART_Transmit(&huart2, buffer, length, HAL_MAX_DELAY) == HAL_OK)
	{
		HAL_GPIO_WritePin(TX_RX_EN1_GPIO_Port, TX_RX_EN1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(TX_RX_EN2_GPIO_Port, TX_RX_EN2_Pin, GPIO_PIN_RESET);
		return HAL_OK;
	}
	else
	{
		HAL_GPIO_WritePin(TX_RX_EN1_GPIO_Port, TX_RX_EN1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(TX_RX_EN2_GPIO_Port, TX_RX_EN2_Pin, GPIO_PIN_RESET);
		return HAL_ERROR;
	}
}

void Modbus_Read_Coil_Status(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numberOfCoils)
{
	modbus->txMessage.funtionCode 			= Read_Coil_Status;
	modbus->txMessage.slaveAddress 			= slaveId;
	modbus->txMessage.startAddress 			= startAddress;
	modbus->txMessage.numOfRegisters 		= numberOfCoils;

	if(modbus->state == Idle && !modbus->txMessage.newSendMessageStatus)
	{
		modbus->txMessage.newSendMessageStatus = true;
		Tick_Timer_Set_Time(&idleTime, 100);
	}
}

void Modbus_Read_Input_Status(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numberOfInputs)
{
	modbus->txMessage.funtionCode			= Read_Input_Status;
	modbus->txMessage.slaveAddress			= slaveId;
	modbus->txMessage.startAddress			= startAddress;
	modbus->txMessage.numOfRegisters		= numberOfInputs;

	if(modbus->state == Idle && !modbus->txMessage.newSendMessageStatus)
	{
		modbus->txMessage.newSendMessageStatus = true;
		Tick_Timer_Set_Time(&idleTime, 100);
	}
}

void Modbus_Read_Holding_Registers(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numberOfRegister)
{
	modbus->txMessage.funtionCode			= Read_Holding_Registers;
	modbus->txMessage.slaveAddress			= slaveId;
	modbus->txMessage.startAddress			= startAddress;
	modbus->txMessage.numOfRegisters		= numberOfRegister;

	if(modbus->state == Idle && !modbus->txMessage.newSendMessageStatus)
	{
		modbus->txMessage.newSendMessageStatus = true;
		Tick_Timer_Set_Time(&idleTime, 100);
	}
}

void Modbus_Read_Input_Registers(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numberOfRegister)
{
	modbus->txMessage.funtionCode			= Read_Input_Registers;
	modbus->txMessage.slaveAddress			= slaveId;
	modbus->txMessage.startAddress			= startAddress;
	modbus->txMessage.numOfRegisters		= numberOfRegister;

	if(modbus->state == Idle && !modbus->txMessage.newSendMessageStatus)
	{
		modbus->txMessage.newSendMessageStatus = true;
		Tick_Timer_Set_Time(&idleTime, 100);
	}
}

void Modbus_Write_Singel_Coil(Modbus_t *modbus, uint8_t slaveId, uint16_t numOfCoil, Modbus_Coil_Status_t status)
{
	modbus->txMessage.funtionCode			= Write_Single_Coil;
	modbus->txMessage.slaveAddress			= slaveId;
	modbus->txMessage.startAddress			= numOfCoil;

	modbus->txMessage.writeData[0] = status;

	if(modbus->state == Idle && !modbus->txMessage.newSendMessageStatus)
	{
		modbus->txMessage.newSendMessageStatus = true;
		Tick_Timer_Set_Time(&idleTime, 100);
	}
}

void Modbus_Write_Single_Register(Modbus_t *modbus, uint8_t slaveId, uint16_t numOfRegister, uint16_t data)
{
	modbus->txMessage.funtionCode			= Write_Single_Register;
	modbus->txMessage.slaveAddress			= slaveId;
	modbus->txMessage.startAddress			= numOfRegister;

	modbus->txMessage.writeData[0] = data;

	if(modbus->state == Idle && !modbus->txMessage.newSendMessageStatus)
	{
		modbus->txMessage.newSendMessageStatus = true;
		Tick_Timer_Set_Time(&idleTime, 100);
	}
}

void Modbus_Write_Multiple_Coils(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numOfCoils, uint16_t coilStatus)
{
	modbus->txMessage.funtionCode			= Write_Multiple_Coils;
	modbus->txMessage.slaveAddress			= slaveId;
	modbus->txMessage.startAddress			= startAddress;
	modbus->txMessage.numOfRegisters		= numOfCoils;

	modbus->txMessage.writeData[0] = (ceil((double)numOfCoils / 8));
	modbus->txMessage.writeData[1] = coilStatus;

	if(modbus->state == Idle && !modbus->txMessage.newSendMessageStatus)
	{
		modbus->txMessage.newSendMessageStatus = true;
		Tick_Timer_Set_Time(&idleTime, 100);
	}
}

void Modbus_Write_Multiple_Registers(Modbus_t *modbus, uint8_t slaveId, uint16_t startAddress, uint16_t numOfRegisters, uint16_t* data)
{
	modbus->txMessage.funtionCode			= Write_Multiple_Registers;
	modbus->txMessage.slaveAddress			= slaveId;
	modbus->txMessage.startAddress			= startAddress;
	modbus->txMessage.numOfRegisters		= numOfRegisters;

	modbus->txMessage.writeData[0] = numOfRegisters * 2;

	for(int i = 0; i < numOfRegisters; i++)
	{
		modbus->txMessage.writeData[i + 1] = data[i];
	}

	if(modbus->state == Idle && !modbus->txMessage.newSendMessageStatus)
	{
		modbus->txMessage.newSendMessageStatus = true;
		Tick_Timer_Set_Time(&idleTime, 100);
	}
}

bool areElementsEqual(uint8_t *txArray, uint8_t *rxArray, uint8_t numberOfElemets)
{
	for(int i = 0; i < numberOfElemets; i++)
	{
		if(txArray[i] != rxArray[i])
			return false;
	}

	return true;
}


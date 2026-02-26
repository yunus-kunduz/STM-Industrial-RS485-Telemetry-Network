/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define BMP180_ADDR (0x77 << 1) // STM32 I2C adresleme formatı (Sola kaydırılmış)

// Kalibrasyon Katsayıları
short AC1, AC2, AC3, B1, B2, MB, MC, MD;
unsigned short AC4, AC5, AC6;

// Sensör Başlatma ve Katsayıları Okuma
void BMP180_Init(void) {
    uint8_t calibData[22];
    HAL_I2C_Mem_Read(&hi2c1, BMP180_ADDR, 0xAA, 1, calibData, 22, 100);

    AC1 = (calibData[0] << 8) | calibData[1];
    AC2 = (calibData[2] << 8) | calibData[3];
    AC3 = (calibData[4] << 8) | calibData[5];
    AC4 = (calibData[6] << 8) | calibData[7];
    AC5 = (calibData[8] << 8) | calibData[9];
    AC6 = (calibData[10] << 8) | calibData[11];
    B1  = (calibData[12] << 8) | calibData[13];
    B2  = (calibData[14] << 8) | calibData[15];
    MB  = (calibData[16] << 8) | calibData[17];
    MC  = (calibData[18] << 8) | calibData[19];
    MD  = (calibData[20] << 8) | calibData[21];
}

// Sıcaklık ve Basınç Okuma Fonksiyonu
void BMP180_Read(float *temp, long *press) {
    uint8_t data[3];
    uint8_t cmd;

    // 1. Sıcaklık Oku
    cmd = 0x2E;
    HAL_I2C_Mem_Write(&hi2c1, BMP180_ADDR, 0xF4, 1, &cmd, 1, 100);
    HAL_Delay(5); // Dönüşüm için bekle
    HAL_I2C_Mem_Read(&hi2c1, BMP180_ADDR, 0xF6, 1, data, 2, 100);
    long UT = (data[0] << 8) | data[1];

    // 2. Basınç Oku (Oversampling = 0)
    cmd = 0x34;
    HAL_I2C_Mem_Write(&hi2c1, BMP180_ADDR, 0xF4, 1, &cmd, 1, 100);
    HAL_Delay(5);
    HAL_I2C_Mem_Read(&hi2c1, BMP180_ADDR, 0xF6, 1, data, 3, 100);
    long UP = (data[0] << 16 | data[1] << 8 | data[2]) >> 8;

    // 3. Matematiksel Dönüşümler (Datasheet Algoritması)
    long X1 = ((UT - (long)AC6) * (long)AC5) >> 15;
    long X2 = ((long)MC << 11) / (X1 + MD);
    long B5 = X1 + X2;
    *temp = ((B5 + 8) >> 4) / 10.0f; // Sıcaklık Hesaplandı

    long B6 = B5 - 4000;
    X1 = (B2 * (B6 * B6 >> 12)) >> 11;
    X2 = (AC2 * B6) >> 11;
    long X3 = X1 + X2;
    long B3 = (((AC1 * 4 + X3) << 0) + 2) >> 2;
    X1 = (AC3 * B6) >> 13;
    X2 = (B1 * (B6 * B6 >> 12)) >> 16;
    X3 = ((X1 + X2) + 2) >> 2;
    unsigned long B4 = (AC4 * (unsigned long)(X3 + 32768)) >> 15;
    unsigned long B7 = ((unsigned long)UP - B3) * 50000;
    long p;
    if (B7 < 0x80000000) p = (B7 * 2) / B4;
    else p = (B7 / B4) * 2;
    X1 = (p >> 8) * (p >> 8);
    X1 = (X1 * 3038) >> 16;
    X2 = (-7357 * p) >> 16;
    *press = p + ((X1 + X2 + 3791) >> 4); // Basınç Hesaplandı
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  // BMP180 Kalibrasyon verilerini çek
    BMP180_Init();

    // RS485'i Dinleme modunda başlat (Hattı kitlememek için)
    HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_RESET);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  float sicaklik;
	        long basinc;

	        // 1. Sensörden gerçek verileri oku
	        BMP180_Read(&sicaklik, &basinc);

	        // Float veriyi yazdırmak için tam sayı ve ondalık kısımlarına ayır
	        int sic_tam = (int)sicaklik;
	        int sic_ondalik = (int)((sicaklik - sic_tam) * 10);
	        if(sic_ondalik < 0) sic_ondalik = -sic_ondalik;

	        // 2. RS485 formatında metin oluştur (Basıncı hPa cinsinden göster)
	        char mesaj[100];
	        sprintf(mesaj, "Sicaklik: %d.%d C, Basinc: %ld hPa\r\n", sic_tam, sic_ondalik, basinc / 100);

	        // 3. Modülü Yazma (TX) moduna al
	        HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_SET);
	        HAL_Delay(1); // Yön oturma gecikmesi

	        // 4. Veriyi gönder
	        HAL_UART_Transmit(&huart1, (uint8_t *)mesaj, strlen(mesaj), 500);
	        HAL_Delay(5); // Altın gecikme (Veri kesilmesin diye)

	        // 5. Modülü Dinleme (RX) moduna geri al
	        HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_RESET);

	        // Saniyede 1 kez ölçüm yap
	        HAL_Delay(1000);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(RS485_DIR_GPIO_Port, RS485_DIR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : RS485_DIR_Pin */
  GPIO_InitStruct.Pin = RS485_DIR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(RS485_DIR_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

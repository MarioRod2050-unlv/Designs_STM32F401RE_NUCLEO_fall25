/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include <stdio.h>
#include <string.h>

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart2;

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);

/* USER CODE BEGIN 0 */

/* Redirect printf() output to UART2 */
int __io_putchar(int ch)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
  return ch;
}

/* IMU I2C Addresses and Registers */
#define LSM6DSV16X_ADDR      (0x6A << 1)
#define LIS2MDL_ADDR         (0x1E << 1)
#define LSM6DSV16X_WHO_AM_I  0x0F
#define LIS2MDL_WHO_AM_I     0x4F
#define LSM6DSV16X_OUTX_L_A  0x28

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  uint8_t who_am_i = 0;
  uint8_t data[6];
  int16_t ax, ay, az;
  int accel_x_mg, accel_y_mg, accel_z_mg;
  HAL_StatusTypeDef ret;
  uint8_t config_data;
  /* USER CODE END 1 */

  /*MCU Configuration--------------------------------------------------------*/

  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  printf("\r\n--- IMU Accelerometer Test ---\r\n");

  /* --- Check LSM6DSV16X ID --- */
  ret = HAL_I2C_Mem_Read(&hi2c1, LSM6DSV16X_ADDR, LSM6DSV16X_WHO_AM_I, 1, &who_am_i, 1, HAL_MAX_DELAY);
  if (ret == HAL_OK)
    printf("LSM6DSV16X WHO_AM_I = 0x%02X\r\n", who_am_i);
  else
    printf("Error reading LSM6DSV16X!\r\n");

  /* --- Initialize LSM6DSV16X Accelerometer --- */
  config_data = 0x38;  // Enable X, Y, Z axes (CTRL9_XL @ 0x18)
  HAL_I2C_Mem_Write(&hi2c1, LSM6DSV16X_ADDR, 0x18, 1, &config_data, 1, HAL_MAX_DELAY);
  config_data = 0x40;  // 104 Hz ODR, ±2g full-scale (CTRL1_XL @ 0x10)
  HAL_I2C_Mem_Write(&hi2c1, LSM6DSV16X_ADDR, 0x10, 1, &config_data, 1, HAL_MAX_DELAY);

  HAL_Delay(100);  // Allow sensor to start up
  printf("Accelerometer initialized.\r\n");
  printf("Streaming accelerometer data (mg)...\r\n");
  /* USER CODE END 2 */

  /*DONT STOP LOOP */
  while (1)
  {
    /*MY CODE BEGIN 3 */
    ret = HAL_I2C_Mem_Read(&hi2c1, LSM6DSV16X_ADDR, LSM6DSV16X_OUTX_L_A, 1, data, 6, HAL_MAX_DELAY);

    if (ret == HAL_OK)
    {
      ax = (int16_t)(data[1] << 8 | data[0]);
      ay = (int16_t)(data[3] << 8 | data[2]);
      az = (int16_t)(data[5] << 8 | data[4]);
      /* Convert to milli-g (mg) using 0.061 mg/LSB sensitivity */
      accel_x_mg = (int)(ax * 61 / 1000);
      accel_y_mg = (int)(ay * 61 / 1000);
      accel_z_mg = (int)(az * 61 / 1000);

      /* Comma-separated output for Python logging */
      printf("%d,%d,%d\r\n", accel_x_mg, accel_y_mg, accel_z_mg);
    }
    else
    {
      printf("I2C read error!\r\n");
    }

    HAL_Delay(100); // 10 samples/sec
    /* USER CODE END 3 */
  }
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
}

/**
  * @brief  Error Handler
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
    //Optional: blink LED here
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* file name num */
}
#endif

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mlx90614.h"
#include "BMP180.h"
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

/* USER CODE BEGIN PV */
uint8_t uart2_rxbuf[1];
uint8_t uart3_rxbuf[1];
uint8_t rxok[1];
uint8_t WheelDrive = 0x04;

float ObjectTemperature;
float AmbientTemperature;
float Emissivity;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */






float airQuality(){
float Value[22];////取22个值做滤波用
	  float AD_Value;
	  HAL_ADCEx_Calibration_Start(&hadc1);
     for(char n=0;n<22;n++)

     {
         HAL_ADC_Start(&hadc1);
         HAL_ADC_PollForConversion(&hadc1, 10);    //等待转换完成，第二个参数表示超时时间，单位ms
         if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
         {
             Value[n]=HAL_ADC_GetValue(&hadc1);
             AD_Value += Value[n];
         }
     }
     HAL_ADC_Stop(&hadc1);

     float max = Value[0];
     float min = Value[0];
     for(char n=0;n<22;n++)//取最大值、最小值
     {
         max = (Value[n]<max)?max:Value[n];
         min = (min<Value[n])?min:Value[n];
     }

     float rkz = (AD_Value - max - min) / 20;


     float result = (float)((AD_Value - max - min) / 20) * (3.311 / 4096);

     return result * 1000;
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
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, uart3_rxbuf, 1);
  BMP180_SetOversampling(BMP180_ULTRA);
  BMP180_UpdateCalibrationData();
  MLX90614_Init(&hi2c1);
  MLX90614_SetEmissivity(0.985);



  float a = 0.2;
int size = sizeof(a);


HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_SET);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


	 // HAL_Delay(1000);
//	  {
	  int32_t  sound = 0;
	  int32_t temperature = BMP180_GetRawTemperature();
	  int32_t pressure = BMP180_GetPressure();
	  MLX90614_ReadAmbientTemperature(&AmbientTemperature);
	  sound = HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12);
	  MLX90614_ReadObjectTemperature(&ObjectTemperature);
	  MLX90614_GetEmissivity(&Emissivity);
	  float AirQuality = airQuality();

	  int32_t upLoad[10];

	  int32_t package1[5] = {0,0,0,0,0};
	  int32_t package2[5] = {0,0,0,0,0};
	  int32_t package3[5] = {0,0,0,0,0};

	  package1[0] = 0;
	  package2[0] = 1;
	  package3[0] = 2;

	  package1[1] = pressure;
	  package1[2] = temperature;
	  package1[3] = (int32_t)(float)(AmbientTemperature * 100);
	  package1[4] = (int32_t)(float)(ObjectTemperature * 100);

	  package2[1] = AirQuality;
	  package2[2] = sound;


//	  upLoad[0] = pressure;
//	  upLoad[1] = temperature;
//	  upLoad[2] = (int32_t)(float)(AmbientTemperature * 100);
//	  upLoad[3] = (int32_t)(float)(ObjectTemperature * 100);
//	  upLoad[4] = AirQuality;
//	  upLoad[5] = sound;
	 // (int32_t)(AmbientTemperature*100);
	 // HAL_UART_Transmit(&huart3,upLoad,24,0xFFFF); 旧包



	  HAL_UART_Transmit(&huart3,package1,20,0xFFFF);
	  HAL_UART_Transmit(&huart3,package2,20,0xFFFF);
//	  HAL_UART_Transmit(&huart3,package3,24,0xFFFF);


//
//
//	  }
//
//
//正对调节 右边延时，顺时延长
//
//	  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1) {
//		  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_RESET);
//	  } else {
//		  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13, GPIO_PIN_SET);
//	  }
//











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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
//串口3连接了蓝牙
	 if(huart->Instance == USART2)
	      {

			    // 重新注册一次，要不然下次收不到了（上个函数内部已经注册）
	      }
	 if(huart->Instance == USART3)
	      {
			//收到前驱信息
					if (uart3_rxbuf[0] == 0x03) {
						WheelDrive = 0x03;
					}
			//收到四驱信息
					if (uart3_rxbuf[0] == 0x04) {
						WheelDrive = 0x04;
					}
			//收到后驱信息
					if (uart3_rxbuf[0] == 0x05) {
						WheelDrive = 0x05;
					}
















			//收到前进信息
			if (uart3_rxbuf[0] == 0xAA) {

				if(WheelDrive == 0x03)
				{
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9, GPIO_PIN_SET);
				}
				else if (WheelDrive == 0x05)
				{
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11, GPIO_PIN_RESET);
				}
				else {
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11, GPIO_PIN_RESET);
				}
			}
			//收到后退信息
			if (uart3_rxbuf[0] == 0xBB) {

				if(WheelDrive == 0x03)
				{
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9, GPIO_PIN_RESET);
				}
				else if (WheelDrive == 0x05)
				{
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11, GPIO_PIN_SET);
				}
				else {
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_SET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11, GPIO_PIN_SET);
				}

			}

			//收到左转信息
					if (uart3_rxbuf[0] == 0xCC) {

						  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0, GPIO_PIN_RESET);
						  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1, GPIO_PIN_SET);

					}



			//收到右转信息
					if (uart3_rxbuf[0] == 0xDD) {

						  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0, GPIO_PIN_SET);
						  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1, GPIO_PIN_RESET);

					}
			//收到右转信息
			if (uart3_rxbuf[0] == 0x02) {

				  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1, GPIO_PIN_RESET);

			}







			//停止信息（待机 00 ）
			if (uart3_rxbuf[0] == 0x01) {

				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8, GPIO_PIN_RESET);
				  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9, GPIO_PIN_RESET);
				  	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_10, GPIO_PIN_RESET);
				  	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11, GPIO_PIN_RESET);

			}

			HAL_UART_Receive_IT(&huart3, uart3_rxbuf, 1);       // 重新注册一次，要不然下次收不到了
	      }



}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

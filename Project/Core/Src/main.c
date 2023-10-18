/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
//#include "Protocol.h"
#include "stdbool.h"
#include "HDLC_Protocol.h"
#include "ModBusSlave.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUF_LEN           100u
#define TIME_SLEAP        500u    //ms
#define TIMEOUT            10u    //ms
//#define READY_TO_TRANSMIT 1
typedef enum
{
  NONE,
  READY_TO_TRANSMIT,
  WAIT_TO
} t_status;

typedef union
{
  float fl[10];
  uint32_t u32[10];
  uint16_t u16[20];
}t_typeflConvert;

typedef union
{
  int64_t i64[3];
  uint16_t u16[12];
}t_typeI64Convert;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart2_tx;
DMA_HandleTypeDef hdma_usart3_tx;

/* USER CODE BEGIN PV */
// TODO глоюальные переменные пишем с большой буквы
uint8_t Rx_buf_HDLC[BUF_LEN] = {0};
uint8_t Tx_buf_HDLC[BUF_LEN] = {0};
uint8_t Rxel_HDLC = 0;
uint8_t Txel_HDLC = 0;

uint8_t Rx_buf_MB[BUF_LEN] = {0};
uint8_t Tx_buf_MB[BUF_LEN] = {0};
uint8_t Rxel_MB = 0;
uint8_t Txel_MB = 0;
uint16_t SendRegMB [100] = {0};
int64_t TES = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// ----------------------------------------------------------------------------
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if(huart == &huart3)
  {

    HDLC_ProtocolDataReceive(&Rx_buf_HDLC[Rxel_HDLC], 1);

    if (Rxel_HDLC == (BUF_LEN-1) )
      Rxel_HDLC = 0;
    else
      Rxel_HDLC++;
    HAL_UART_Receive_IT(&huart3, &Rx_buf_HDLC[Rxel_HDLC], 1);

//    if (++Rxel_HDLC >= BUF_LEN)
//      Rxel_HDLC = 0;

  }
  if(huart == &huart2)
  {

    ModBusSetData(&Rx_buf_MB[Rxel_MB], 1);
    
    if (Rxel_MB == (BUF_LEN-1) )
      Rxel_MB = 0;
    else
      Rxel_MB++;
    HAL_UART_Receive_IT(&huart2, &Rx_buf_MB[Rxel_MB], 1);

//    if (++Rxel_MB >= BUF_LEN)
//      Rxel_MB = 0;

  }
}
// ----------------------------------------------------------------------------
void UartSendDataHDLC(uint8_t *data, uint16_t len)
{
  // сделать проверку длины данных и размера буфера
  if(len > BUF_LEN)
  {
    return;
  }
  memcpy(Tx_buf_HDLC, data, len);
  HAL_UART_Transmit_DMA(&huart3, Tx_buf_HDLC, len);
  return;
}

void UartSendDataMB(uint8_t *data, uint16_t len)
{
  // сделать проверку длины данных и размера буфера
  if(len > BUF_LEN)
  {
    return;
  }
  memcpy(Tx_buf_MB, data, len);
  HAL_UART_Transmit_DMA(&huart2, Tx_buf_MB, len);
  return;
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  t_typeflConvert HDLC_to_MB_fl;
  t_typeI64Convert HDLC_to_MB_i64;

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
  MX_DMA_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  
  HAL_UART_Receive_IT(&huart3, Rx_buf_HDLC, 1);
  HAL_UART_Receive_IT(&huart2, Rx_buf_MB, 1);

  t_InitParams initHDLC;
  t_modbus_init_struct initMB;

  HDLC_ProtocolInitParamsStructureReset(&initHDLC);

  initHDLC.server_address = DEFAULT_SERVER_ADDRESS;
  initHDLC.client_address = DEFAULT_CLIENT_ADDRESS;
  memcpy(initHDLC.pasword, DEFAULT_PASWORD, 8);

  initHDLC.uartSendDataCB = UartSendDataHDLC;
  initHDLC.getTicksCB = HAL_GetTick;

  HDLC_ProtocolInit(&initHDLC);
//--------------------------------------MB----------------------------------------
  initMB.stationId = 1;
  initMB._GetTicks100Mks = HAL_GetTick;
  initMB._sendByte = UartSendDataMB;
  initMB.HoldingRegistersPtr = SendRegMB;
  initMB.HoldingRegistersSize = 36;
  
  ModBusInit(&initMB);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HDLC_ProtocolMain();
    ModBusRun();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    HDLC_to_MB_fl.fl[0]=GetCurrentA();
    HDLC_to_MB_fl.fl[1]=GetCurrentB();
    HDLC_to_MB_fl.fl[2]=GetCurrentC();
    HDLC_to_MB_fl.fl[3]=GetVoltageA();
    HDLC_to_MB_fl.fl[4]=GetVoltageB();
    HDLC_to_MB_fl.fl[5]=GetVoltageC();
    HDLC_to_MB_fl.fl[6]=GetSumPowerReactive();
    HDLC_to_MB_fl.fl[7]=GetSumPowerActive();
    HDLC_to_MB_fl.fl[8]=GetSumPowerApparent();
    HDLC_to_MB_fl.fl[9]=GetCosFi();
    
    HDLC_to_MB_i64.i64[0] = GetEnergyActiveImport();
    HDLC_to_MB_i64.i64[1] = GetEnergyReactiveImport();
    HDLC_to_MB_i64.i64[2] = GetEnergyApparentImport();
    
    for(int i=0; i+1<20; i+=2)
    {
      SendRegMB[ i ]=HDLC_to_MB_fl.u16[i+1];
      SendRegMB[i+1]=HDLC_to_MB_fl.u16[ i ];
    }
    TES = GetEnergyActiveImport();
    for(int i=0; i+3<12; i+=4)
    {
      SendRegMB[ (i+20) ]=HDLC_to_MB_i64.u16[i+3];
      SendRegMB[(i+20)+1]=HDLC_to_MB_i64.u16[i+2];
      SendRegMB[(i+20)+2]=HDLC_to_MB_i64.u16[i+1];
      SendRegMB[(i+20)+3]=HDLC_to_MB_i64.u16[ i ];
    }
    
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
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7200-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 100-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
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
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_PIN_Pin */
  GPIO_InitStruct.Pin = LED_PIN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_PIN_GPIO_Port, &GPIO_InitStruct);

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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

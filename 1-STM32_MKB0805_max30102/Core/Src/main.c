#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "delay.h"
#include "max30102.h"
#include "led.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "oled.h"
#include "debug_printf.h"

uint8_t uart1_tx_buf[6];

typedef struct
{
  uint8_t Systolic;  //收缩压
  uint8_t Diastolic; //舒张压
  uint8_t HeartRate; //心率
} BloodPressure_t;

BloodPressure_t BloodPressure;

const int bufflen = 6; //数据长度
uint8_t TXBuff[bufflen] = {0xFd, 0x00, 0x00, 0x00, 0x00, 0x00};//发送的数据
uint8_t RXBuff[bufflen];//接收的数据
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

//函数声明
void getData(void);
void display_data(uint8_t n_sp02);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//获取数据
  if (RXBuff[0] == 0xFD)
  {
    BloodPressure.Systolic = RXBuff[1];
    BloodPressure.Diastolic = RXBuff[2];
    BloodPressure.HeartRate = RXBuff[3];
  }
	//重新打开串口中断
  HAL_UART_Receive_IT(&huart2, (uint8_t *)RXBuff, bufflen);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  int32_t n_heart_rate = 0;  //获取心率值
  int32_t n_sp02 = 0;        //获取血氧值
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
	delay_init(72);
  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
	led_init();
	OLED_Init();
	OLED_ShowString(0,0,"HR:",OLED_8X16);
	OLED_ShowString(8*6,0,"/sp02:",OLED_8X16);
	OLED_ShowChinese(0,16,"收缩压");
	OLED_ShowChinese(0,16*2,"舒张压");
	OLED_Update();
	//打开串口中断
  HAL_UART_Receive_IT(&huart2, RXBuff, bufflen);
	max30102_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 
  while (1)
  {
		max30102_Read_Data(&n_heart_rate, &n_sp02);   

    getData();

		display_data((uint8_t)n_sp02);
		HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);
		
		uart1_tx_buf[0] = 0xfd;
		uart1_tx_buf[1] = BloodPressure.HeartRate;
		uart1_tx_buf[2] = BloodPressure.Systolic; 
    uart1_tx_buf[3] = BloodPressure.Diastolic;
		uart1_tx_buf[4] = (uint8_t)n_sp02;
		uart1_tx_buf[5] = 0xfe;
		HAL_UART_Transmit(&huart1, (uint8_t *)uart1_tx_buf, sizeof(uart1_tx_buf), 0xffff);
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

/* USER CODE BEGIN 4 */

// 获取数据
void getData(void)
{
  HAL_UART_Transmit(&huart2, (uint8_t *)TXBuff, sizeof(TXBuff), 0xffff);
}

void display_data(uint8_t n_sp02)
{
	OLED_ShowNum(8*3,0,BloodPressure.HeartRate,3,OLED_8X16);
	OLED_ShowNum(8*12,0,n_sp02,2,OLED_8X16);
	OLED_ShowNum(16*4,16,BloodPressure.Systolic,3,OLED_8X16);
	OLED_ShowNum(16*4,16*2,BloodPressure.Diastolic,3,OLED_8X16);
	OLED_Update();
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


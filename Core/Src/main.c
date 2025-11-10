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
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_RTT.h"
#include "app_terminal.h"

// #include "usbd_cdc_acm.h"
// #include "usb_log.h"
// #include "cdc_acm_ringbuffer.h"
// #include "lcd_logic.h"
// #include "gt9147_logic.h"

#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// 定义UI布局
#define UI_AREA_HEIGHT    60  // 顶部UI条的高度 (像素)
#define BUTTON_WIDTH      100 // 每个按钮的宽度
#define BUTTON_HEIGHT     50  // 每个按钮的高度
#define BUTTON_MARGIN     10  // 按钮间距

// 定义按钮的X坐标
#define BUTTON_X_RED      (BUTTON_MARGIN)                                    // 10
#define BUTTON_X_GREEN    (BUTTON_X_RED + BUTTON_WIDTH + BUTTON_MARGIN)      // 120
#define BUTTON_X_BLUE     (BUTTON_X_GREEN + BUTTON_WIDTH + BUTTON_MARGIN)    // 230
#define BUTTON_X_ERASER   (BUTTON_X_BLUE + BUTTON_WIDTH + BUTTON_MARGIN)     // 340
#define BUTTON_X_CLEAR    (lcddev.width - BUTTON_WIDTH - BUTTON_MARGIN)      // 假设800宽, 为 690

// 定义画笔/橡皮擦大小 (奇数, 方便计算中心点)
#define BRUSH_SIZE        5   // 5x5 像素

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// // 全局画笔颜色 (默认为黑色)
// static uint32_t current_color = BLACK;
// // 画笔/橡皮擦半径
// static int32_t brush_radius = BRUSH_SIZE / 2;

// /* USER CODE END PV */

// /* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
// /* USER CODE BEGIN PFP */
// static void draw_ui_palette(void); // 绘制UI界面
// static void handle_touch(uint16_t x, uint16_t y); // 处理触摸事件

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief USB Log (Retargets the C library printf function to the USART)
  */
#if defined(__CC_ARM)      // Keil
int fputc(int ch,FILE *f)
{
  SEGGER_RTT_Write(0, ptr, len);
  return len;
}
#elif defined(__GNUC__)   // GCC
int _write(int fd, char *ptr, int len)  
{  
  SEGGER_RTT_Write(0, ptr, len);
  return len;
  // HAL_Delay();
}
#endif

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
  SEGGER_RTT_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  // // FSMC LCD Init
  // LCD_Init();
  // // Set LCD Display Direction
  // LCD_Display_Dir(1); 

  // // USB Device CDC Init
  // uint8_t busid = 0;
  // cdc_acm_init(busid, USB_OTG_FS_PERIPH_BASE);

  // // Only for Test
  // // HAL_Delay(100);
  // // LCD_Fill(0, 0, 100, 100, RED); 
  // // LCD_ShowString(120, 10, 200, 16, 16, "Hello");
  // // LCD_ShowString(120, 40, 200, 16, 16, (uint8_t*)"Hello STM32!");
  // // LCD_DrawCircle(120, 160, 50);

  // /* 4. 触摸驱动初始化 */
  // // 我们将 LCD 的参数传递给触摸驱动，以便它正确映射坐标
  // if (GT9147_Init(lcddev.width, lcddev.height, lcddev.dir) != 0)
  // {
  //     // GT9147 初始化失败 (未检测到)
  //     // 在屏幕上显示错误信息
  //     LCD_ShowString(10, 10, 200, 20, 16, (uint8_t*)"Touch Init FAILED!");
  //     while(1);
  // }

  // /* 5. 准备绘画板 */
  // current_color = BLACK; // 默认画笔为黑色
  // LCD_Clear(WHITE);      // 将背景设为白色 "画纸"
  // draw_ui_palette();     // 绘制顶部的工具栏
  
  // /* 6. 主循环 */
  // GT_TouchPoint_t touch_points[GT_MAX_POINTS]; // 存储触摸点的数组

  if (App_Terminal_Init(0) != 0) // 传入 busid 0
  {
      // 初始化失败 (已在 app_terminal.c 中显示错误)
      Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // if (!cdc_acm_is_rx_empty())
    // {
    //   uint8_t rx_buf[128];
    //   int len = cdc_acm_read_data(rx_buf, sizeof(rx_buf));

    //   if (len > 0)
    //   {
    //     // 回显接收到的数据
    //     cdc_acm_send_data(busid, rx_buf, len);
    //     LCD_ShowString(5, 120, 400, 16, 32, rx_buf);
    //   }
    // }

    // // 定期发送心跳
    // static uint32_t tick = 0;
    // if (++tick % 10000 == 0)
    // {
    //     const char *msg = "Heartbeat\r\n";
    //     cdc_acm_send_data(busid, (uint8_t *)msg, strlen(msg));
    //     USB_LOG_INFO("Heartbeat");
    // }

    // // 重要：触发发送
    // cdc_acm_try_send(busid);

    // // 检测触摸事件
    // uint8_t touch_count = GT9147_Scan(touch_points, GT_MAX_POINTS);
      
    //   if (touch_count > 0)
    //   {
    //       // 遍历所有检测到的触摸点 (支持多点触控)
    //       for(uint8_t i = 0; i < touch_count; i++)
    //       {
    //           // 处理这个点的触摸事件
    //           handle_touch(touch_points[i].x, touch_points[i].y);
    //       }
    //   }

    // HAL_Delay(5);

    App_Terminal_Tasks();
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// /**
//   * @brief  处理单点触摸事件
//   * @param  x: 触摸点 X 坐标
//   * @param  y: 触摸点 Y 坐标
//   * @retval 无
//   */
// static void handle_touch(uint16_t x, uint16_t y)
// {
//     // 检查触摸点是否在UI工具栏区域
//     if (y < UI_AREA_HEIGHT)
//     {
//         // 1. 点击了 "红色" 按钮
//         if (x > BUTTON_X_RED && x < (BUTTON_X_RED + BUTTON_WIDTH))
//         {
//             current_color = RED;
//         }
//         // 2. 点击了 "绿色" 按钮
//         else if (x > BUTTON_X_GREEN && x < (BUTTON_X_GREEN + BUTTON_WIDTH))
//         {
//             current_color = GREEN;
//         }
//         // 3. 点击了 "蓝色" 按钮
//         else if (x > BUTTON_X_BLUE && x < (BUTTON_X_BLUE + BUTTON_WIDTH))
//         {
//             current_color = BLUE;
//         }
//         // 4. 点击了 "橡皮擦" 按钮
//         else if (x > BUTTON_X_ERASER && x < (BUTTON_X_ERASER + BUTTON_WIDTH))
//         {
//             current_color = WHITE; // 橡皮擦就是白色的画笔
//         }
//         // 5. 点击了 "清除" 按钮
//         else if (x > BUTTON_X_CLEAR && x < (BUTTON_X_CLEAR + BUTTON_WIDTH))
//         {
//             // 清除绘画区域 (不清除UI)
//             LCD_Fill(0, UI_AREA_HEIGHT, lcddev.width - 1, lcddev.height - 1, WHITE);
//         }
//     }
//     // 触摸点在绘画区域
//     else
//     {
//         // 计算画刷的绘制范围
//         uint16_t sx = (x <= brush_radius) ? 0 : (x - brush_radius);
//         uint16_t sy = (y <= brush_radius) ? 0 : (y - brush_radius);
//         uint16_t ex = (x >= lcddev.width - 1 - brush_radius) ? (lcddev.width - 1) : (x + brush_radius);
//         uint16_t ey = (y >= lcddev.height - 1 - brush_radius) ? (lcddev.height - 1) : (y + brush_radius);
        
//         // 确保绘制不会覆盖UI
//         if(sy < UI_AREA_HEIGHT)
//         {
//             sy = UI_AREA_HEIGHT;
//         }

//         // 用当前颜色填充画刷区域
//         LCD_Fill(sx, sy, ex, ey, current_color);
//     }
// }


// /**
//   * @brief  绘制UI工具栏
//   * @retval 无
//   */
// static void draw_ui_palette(void)
// {
//     // 1. 绘制灰色背景条
//     LCD_Fill(0, 0, lcddev.width - 1, UI_AREA_HEIGHT - 1, LGRAY); // 浅灰色

//     // 2. 绘制按钮
//     uint16_t button_y_start = (UI_AREA_HEIGHT - BUTTON_HEIGHT) / 2; // Y坐标居中
    
//     // 红色按钮
//     LCD_Fill(BUTTON_X_RED, button_y_start, BUTTON_X_RED + BUTTON_WIDTH - 1, button_y_start + BUTTON_HEIGHT - 1, RED);
    
//     // 绿色按钮
//     LCD_Fill(BUTTON_X_GREEN, button_y_start, BUTTON_X_GREEN + BUTTON_WIDTH - 1, button_y_start + BUTTON_HEIGHT - 1, GREEN);
    
//     // 蓝色按钮
//     LCD_Fill(BUTTON_X_BLUE, button_y_start, BUTTON_X_BLUE + BUTTON_WIDTH - 1, button_y_start + BUTTON_HEIGHT - 1, BLUE);
    
//     // 橡皮擦按钮 (白色)
//     LCD_Fill(BUTTON_X_ERASER, button_y_start, BUTTON_X_ERASER + BUTTON_WIDTH - 1, button_y_start + BUTTON_HEIGHT - 1, WHITE);
//     LCD_DrawRectangle(BUTTON_X_ERASER, button_y_start, BUTTON_X_ERASER + BUTTON_WIDTH - 1, button_y_start + BUTTON_HEIGHT - 1); // 加个黑框
    
//     // 清除按钮 (深蓝色)
//     LCD_Fill(BUTTON_X_CLEAR, button_y_start, BUTTON_X_CLEAR + BUTTON_WIDTH - 1, button_y_start + BUTTON_HEIGHT - 1, DARKBLUE);
//     LCD_ShowString(BUTTON_X_CLEAR + 10, button_y_start + 17, BUTTON_WIDTH-20, BUTTON_HEIGHT-20, 16, (uint8_t*)"CLEAR ALL");
// }

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
  // printf("[ERR/SYS] Error_Handler\r\n");
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

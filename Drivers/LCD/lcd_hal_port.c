#include "lcd_hal_port.h"

/* * 包含您项目的主 HAL 头文件。
 * 它可能是 "stm32f1xx_hal.h", "stm32f4xx_hal.h", "stm32f7xx_hal.h" 等，
 * 或者通常是 "main.h"（如果它在CubeMX中生成）。
 */
#include "main.h" // <<< 确保此头文件包含了正确的 HAL 库

/*
================================================================================
  1. FSMC 内存映射 (根据您原始的 lcd.h)
================================================================================
*/

// 基于您原始的 lcd.h 定义:
// #define LCD_BASE        ((u32)(0x6C000000 | 0x0000007E))
// typedef struct
// {
// 	vu16 LCD_REG; // 位于 0x6C00007E
// 	vu16 LCD_RAM; // 位于 0x6C000080 (偏移 +2 字节)
// } LCD_TypeDef;

#define LCD_REG_ADDR    ((uint32_t)0x6C00007E)
#define LCD_RAM_ADDR    ((uint32_t)0x6C000080)

// 定义用于访问的宏
#define LCD_REG         (*(volatile uint16_t *)LCD_REG_ADDR)
#define LCD_RAM         (*(volatile uint16_t *)LCD_RAM_ADDR)


/*
================================================================================
  2. 背光 (BLK) 引脚定义 (根据您原始的 lcd.h)
================================================================================
*/

// #define	LCD_LED PBout(15) // PB15
#define LCD_BLK_PORT    GPIOB
#define LCD_BLK_PIN     GPIO_PIN_15


/*
================================================================================
  3. 硬件抽象函数实现
================================================================================
*/

/**
 * @brief 硬件：写入一个16位寄存器地址/命令
 * @param reg: 16位寄存器地址
 */
void LCD_HW_WriteReg(volatile uint16_t reg)
{
    // 写入寄存器地址 (RS=0)
    LCD_REG = reg;
}

/**
 * @brief 硬件：写入一个16位数据
 * @param data: 16位数据
 */
void LCD_HW_WriteData(volatile uint16_t data)
{
    // 写入数据 (RS=1)
    LCD_RAM = data;
}

/**
 * @brief 硬件：读取一个16位数据
 * @return 16位数据
 */
uint16_t LCD_HW_ReadData(void)
{
    // 读取数据 (RS=1)
    return LCD_RAM;
}

/**
 * @brief 硬件：控制背光
 * @param on: 1 为开启, 0 为关闭
 */
void LCD_HW_SetBacklight(uint8_t on)
{
    if (on)
    {
        // 根据原始 lcd.c (LCD_LED=0)，置0为点亮
        HAL_GPIO_WritePin(LCD_BLK_PORT, LCD_BLK_PIN, GPIO_PIN_RESET); 
    }
    else
    {
        HAL_GPIO_WritePin(LCD_BLK_PORT, LCD_BLK_PIN, GPIO_PIN_SET);
    }
}

/**
 * @brief 硬件：毫秒延时
 * @param ms: 延时毫秒数
 */
void LCD_HW_Delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

/**
 * @brief 硬件：微秒延时 (需要校准)
 * @param us: 延时微秒数
 */
void LCD_HW_Delay_us(uint32_t us)
{
    /*
     * !! 注意：这是一个粗略的、未校准的延时循环。
     * 您必须根据您的 HCLK 频率和编译器优化级别 (-O0, -O2...) 对其进行校准。
     *
     * 获取精确 us 延时的一种更可靠的方法是使用 DWT 周期计数器或一个专用的 TIM 定时器。
     *
     * --- 校准方法 ---
     * 1. 设置一个 GPIO 引脚。
     * 2. 调用 LCD_HW_Delay_us(1000); // 延时 1ms
     * 3. 清除该 GPIO 引脚。
     * 4. 使用示波器测量脉冲宽度，调整下面的 "ticks" 乘数，直到脉宽接近 1000us (1ms)。
     *
     * (以下值是基于 ~168MHz HCLK 和 -O2 优化的一个粗略估计)
     */
    
    // 估算值: (us * (HCLK_MHz / 5)) -> 假设 1 循环 ~ 5 个时钟周期
    // volatile uint32_t ticks = us * (SystemCoreClock / 1000000 / 5);
    
    // 使用一个固定的估算值，例如 30 
    volatile uint32_t ticks = us * 30; // 假设 30 个循环 ~ 1us
    
    if (ticks == 0) ticks = 1; // 至少执行一次
    
    while(ticks-- > 0);
}
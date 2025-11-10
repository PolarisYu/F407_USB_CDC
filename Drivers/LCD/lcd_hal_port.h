#ifndef __LCD_HAL_PORT_H
#define __LCD_HAL_PORT_H

#include <stdint.h> // 使用标准整数类型

/*
================================================================================
  LCD 硬件抽象层 (HAL) 端口
  
  您必须为您的特定平台实现此文件中的所有 "LCD_HW_..." 函数。
  例如，如果您使用FSMC，您将在这里实现FSMC的读/写。
  如果您使用SPI，您将在这里实现SPI的读/写。
================================================================================
*/


/**
 * @brief 硬件：写入一个16位寄存器地址
 * @param reg: 16位寄存器地址
 */
void LCD_HW_WriteReg(volatile uint16_t reg);

/**
 * @brief 硬件：写入一个16位数据
 * @param data: 16位数据
 */
void LCD_HW_WriteData(volatile uint16_t data);

/**
 * @brief 硬件：读取一个16位数据
 * @return 16位数据
 */
uint16_t LCD_HW_ReadData(void);

/**
 * @brief 硬件：控制背光
 * @param on: 1 为开启, 0 为关闭
 */
void LCD_HW_SetBacklight(uint8_t on);

/**
 * @brief 硬件：毫秒延时
 * @param ms: 延时毫秒数
 */
void LCD_HW_Delay_ms(uint32_t ms);

/**
 * @brief 硬件：微秒延时
 * @param us: 延时微秒数
 */
void LCD_HW_Delay_us(uint32_t us);

#endif
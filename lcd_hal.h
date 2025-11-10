#ifndef __LCD_HAL_H
#define __LCD_HAL_H

#include "main.h"
#include "stdlib.h"

// LCD重要参数集
typedef struct  
{										    
	uint16_t width;			// LCD宽度
	uint16_t height;		// LCD高度
	uint16_t id;			// LCD ID
	uint8_t  dir;			// 横屏或竖屏控制：0-竖屏；1-横屏
	uint16_t wramcmd;		// 开始写gram指令
	uint16_t setxcmd;		// 设置x坐标指令
	uint16_t setycmd;		// 设置y坐标指令
}_lcd_dev; 	

// LCD参数
extern _lcd_dev lcddev;	// 管理LCD重要参数

// LCD画笔颜色和背景色	   
extern uint16_t POINT_COLOR; // 默认红色    
extern uint16_t BACK_COLOR;  // 背景颜色，默认为白色

// FSMC LCD寄存器地址定义
// 根据你的硬件连接修改，这里假设使用FSMC Bank1 区域4，A6作为地址线
#define LCD_BASE        ((uint32_t)(0x60000000 | 0x0000007E))
#define LCD_REG         *(__IO uint16_t *)(LCD_BASE)
#define LCD_RAM         *(__IO uint16_t *)(LCD_BASE + 2)

// 背光控制引脚
#define LCD_BL_GPIO_Port GPIOB
#define LCD_BL_Pin GPIO_PIN_15
#define LCD_LED(x)  HAL_GPIO_WritePin(LCD_BL_GPIO_Port, LCD_BL_Pin, x ? GPIO_PIN_SET : GPIO_PIN_RESET)

// 扫描方向定义
#define L2R_U2D  0 // 从左到右,从上到下
#define L2R_D2U  1 // 从左到右,从下到上
#define R2L_U2D  2 // 从右到左,从上到下
#define R2L_D2U  3 // 从右到左,从下到上

#define U2D_L2R  4 // 从上到下,从左到右
#define U2D_R2L  5 // 从上到到,从右到左
#define D2U_L2R  6 // 从下到上,从左到右
#define D2U_R2L  7 // 从下到上,从右到左	 

#define DFT_SCAN_DIR  L2R_U2D  // 默认的扫描方向

// 画笔颜色
#define WHITE       0xFFFF
#define BLACK       0x0000	  
#define BLUE        0x001F  
#define BRED        0XF81F
#define GRED        0XFFE0
#define GBLUE       0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN       0XBC40
#define BRRED       0XFC07
#define GRAY        0X8430
#define DARKBLUE    0X01CF
#define LIGHTBLUE   0X7D7C
#define GRAYBLUE    0X5458
#define LIGHTGREEN  0X841F
#define LGRAY       0XC618
#define LGRAYBLUE   0XA651
#define LBBLUE      0X2B12

// SSD1963相关定义
#define SSD_HOR_RESOLUTION		800		// LCD水平分辨率
#define SSD_VER_RESOLUTION		480		// LCD垂直分辨率
#define SSD_HOR_PULSE_WIDTH		1		// 水平脉宽
#define SSD_HOR_BACK_PORCH		46		// 水平前廊
#define SSD_HOR_FRONT_PORCH		210		// 水平后廊
#define SSD_VER_PULSE_WIDTH		1		// 垂直脉宽
#define SSD_VER_BACK_PORCH		23		// 垂直前廊
#define SSD_VER_FRONT_PORCH		22		// 垂直前廊
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)

// ========== LCD底层驱动函数 ==========
void LCD_Init(void);                                          // LCD初始化
void LCD_DisplayOn(void);                                     // 开启显示
void LCD_DisplayOff(void);                                    // 关闭显示
void LCD_WriteReg(uint16_t LCD_Reg, uint16_t LCD_RegValue);  // 写寄存器
uint16_t LCD_ReadReg(uint16_t LCD_Reg);                      // 读寄存器
void LCD_WriteRAM_Prepare(void);                             // 准备写GRAM
void LCD_WriteRAM(uint16_t RGB_Code);                        // 写GRAM
uint16_t LCD_BGR2RGB(uint16_t c);                            // 颜色格式转换
void LCD_Scan_Dir(uint8_t dir);                              // 设置扫描方向
void LCD_Display_Dir(uint8_t dir);                           // 设置显示方向(横屏/竖屏)
void LCD_Set_Window(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height); // 设置窗口
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);           // 设置光标位置

// ========== 绘图函数 ==========
void LCD_Clear(uint16_t color);                              // 清屏
void LCD_DrawPoint(uint16_t x, uint16_t y);                  // 画点
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color); // 快速画点
uint16_t LCD_ReadPoint(uint16_t x, uint16_t y);             // 读点
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2); // 画线
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2); // 画矩形
void LCD_Draw_Circle(uint16_t x0, uint16_t y0, uint8_t r);  // 画圆

// ========== 填充函数 ==========
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color); // 单色填充
void LCD_Color_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color); // 彩色填充

// ========== 文字显示函数 ==========
void LCD_ShowChar(uint16_t x, uint16_t y, uint8_t num, uint8_t size, uint8_t mode); // 显示字符
void LCD_ShowNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size);  // 显示数字
void LCD_ShowxNum(uint16_t x, uint16_t y, uint32_t num, uint8_t len, uint8_t size, uint8_t mode); // 显示数字(扩展)
void LCD_ShowString(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t size, uint8_t *p); // 显示字符串

// ========== 辅助函数 ==========
uint32_t LCD_Pow(uint8_t m, uint8_t n);                     // m的n次方
void LCD_SSD_BackLightSet(uint8_t pwm);                     // SSD1963背光控制(0-100)

#endif
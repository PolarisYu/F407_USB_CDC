#ifndef __APP_TERMINAL_H
#define __APP_TERMINAL_H

#include <stdint.h>
#include <stdbool.h>

/*
 * 包含所有此应用依赖的底层驱动
 */
#include "lcd_logic.h"
#include "gt9147_logic.h"
#include "cdc_acm_ringbuffer.h"

/*
================================================================================
  公共 API 函数
================================================================================
*/

/**
 * @brief 初始化终端应用
 * @details 此函数将初始化 LCD、Touch 和 USB CDC，并绘制 UI 界面。
 * @param busid 要使用的 USB busid (通常为 0)
 * @return 0: 成功, -1: 硬件初始化失败
 */
int App_Terminal_Init(uint8_t busid);

/**
 * @brief 终端应用的任务轮询
 * @details 此函数应在 main.c 的 while(1) 循环中被调用。
 * 它负责处理所有 USB 接收和触摸事件。
 */
void App_Terminal_Tasks(void);


#endif /* __APP_TERMINAL_H */
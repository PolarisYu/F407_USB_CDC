#include "gt9147_logic.h"
#include "touch_hal_port.h" // 仅包含硬件抽象层
#include <string.h>         // 用于 strcmp

// 存储屏幕方向参数
static uint16_t s_lcd_width = 0;
static uint16_t s_lcd_height = 0;
static uint8_t s_lcd_orientation = 0; // 0=Portrait, 1=Landscape

// 触摸点寄存器表
const uint16_t GT9147_TPX_TBL[GT_MAX_POINTS] = {GT_TP1_REG, GT_TP2_REG, GT_TP3_REG, GT_TP4_REG, GT_TP5_REG};

// 原始配置表
const uint8_t GT9147_CFG_TBL[]=
{ 
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,
};  

/*
================================================================================
  底层I2C封装 (依赖 HAL Port)
================================================================================
*/

/**
 * @brief 写入寄存器
 */
uint8_t GT9147_WR_Reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i;
	uint8_t ret = 0;
	TOUCH_HW_IIC_Start();	
 	TOUCH_HW_IIC_Send_Byte(GT_CMD_WR);   	// 发送写命令 	 
	if(TOUCH_HW_IIC_Wait_Ack()) { ret=1; goto stop; }
	TOUCH_HW_IIC_Send_Byte(reg >> 8);   	// 发送高8位地址
	if(TOUCH_HW_IIC_Wait_Ack()) { ret=1; goto stop; } 	 										  		   
	TOUCH_HW_IIC_Send_Byte(reg & 0XFF);   	// 发送低8位地址
	if(TOUCH_HW_IIC_Wait_Ack()) { ret=1; goto stop; } 
	for(i = 0; i < len; i++)
	{	   
    	TOUCH_HW_IIC_Send_Byte(buf[i]);  	// 发数据
		if(TOUCH_HW_IIC_Wait_Ack()) { ret=1; break; }
	}
stop:
    TOUCH_HW_IIC_Stop();					// 产生一个停止条件	    
	return ret; 
}

/**
 * @brief 读取寄存器
 */
void GT9147_RD_Reg(uint16_t reg, uint8_t *buf, uint8_t len)
{
	uint8_t i; 
 	TOUCH_HW_IIC_Start();	
 	TOUCH_HW_IIC_Send_Byte(GT_CMD_WR);   // 发送写命令 	 
	if(TOUCH_HW_IIC_Wait_Ack()) { goto stop; }
 	TOUCH_HW_IIC_Send_Byte(reg >> 8);   	// 发送高8位地址
	if(TOUCH_HW_IIC_Wait_Ack()) { goto stop; } 	 										  		   
 	TOUCH_HW_IIC_Send_Byte(reg & 0XFF);   	// 发送低8位地址
	if(TOUCH_HW_IIC_Wait_Ack()) { goto stop; } 
	
 	TOUCH_HW_IIC_Start();  	 	   
	TOUCH_HW_IIC_Send_Byte(GT_CMD_RD);   // 发送读命令		   
	if(TOUCH_HW_IIC_Wait_Ack()) { goto stop; } 	   
	for(i = 0; i < len; i++)
	{	   
    	buf[i] = TOUCH_HW_IIC_Read_Byte(i == (len - 1) ? 0 : 1); // 读数据, 最后一个字节NAck
	} 
stop:
    TOUCH_HW_IIC_Stop();// 产生一个停止条件    
} 

/**
 * @brief 发送GT9147配置参数
 * @param mode 0,参数不保存到flash; 1,参数保存到flash
 */
uint8_t GT9147_Send_Cfg(uint8_t mode)
{
	uint8_t buf[2];
	uint8_t i = 0;
	buf[0] = 0;
	buf[1] = mode;	// 是否写入到GT9147 FLASH?  即是否掉电保存
	for(i = 0; i < sizeof(GT9147_CFG_TBL); i++) buf[0] += GT9147_CFG_TBL[i];//计算校验和
    buf[0] = (~buf[0]) + 1;
	GT9147_WR_Reg(GT_CFGS_REG, (uint8_t*)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL));//发送寄存器配置
	GT9147_WR_Reg(GT_CHECK_REG, buf, 2);//写入校验和,和配置更新标记
	return 0;
} 

/*
================================================================================
  公开 API 函数实现
================================================================================
*/

/**
 * @brief 初始化GT9147触摸屏
 */
uint8_t GT9147_Init(uint16_t width, uint16_t height, uint8_t orientation)
{
	uint8_t temp[5]; 
    
    // 保存屏幕参数
    s_lcd_width = width;
    s_lcd_height = height;
    s_lcd_orientation = orientation;

    // 初始化硬件IO (RST, INT)
	TOUCH_HW_GPIO_Init();
    // 初始化 I2C
	TOUCH_HW_IIC_Init();      	
    
    // 硬件复位
	TOUCH_HW_Set_RST(0);	// 复位
	TOUCH_HW_Delay_ms(10);
 	TOUCH_HW_Set_RST(1);	// 释放复位		    
	TOUCH_HW_Delay_ms(10); 
		
	TOUCH_HW_Delay_ms(100);  
	
	GT9147_RD_Reg(GT_PID_REG, temp, 4); // 读取产品ID
	temp[4] = 0;
	
	if(strcmp((char*)temp, "9147") == 0) // 确认ID是9147
	{
		temp[0] = 0X02;			
		GT9147_WR_Reg(GT_CTRL_REG, temp, 1); // 软复位GT9147
		
		GT9147_RD_Reg(GT_CFGS_REG, temp, 1); // 读取配置版本
		if(temp[0] < 0X60) // 默认版本比较低,需要更新flash配置
		{
			GT9147_Send_Cfg(1); // 更新并保存配置
		}
		TOUCH_HW_Delay_ms(10);
		temp[0] = 0X00;	 
		GT9147_WR_Reg(GT_CTRL_REG, temp, 1); // 结束复位
		return 0; // 初始化成功
	} 
	return 1; // 初始化失败
}


/**
 * @brief 扫描触摸点
 */
uint8_t GT9147_Scan(GT_TouchPoint_t* points, uint8_t max_points)
{
	uint8_t buf[4];
	uint8_t i = 0;
	uint8_t point_status = 0;
    uint8_t point_count = 0;
    uint8_t clear_status = 0;
    
    // 1. 读取状态寄存器
	GT9147_RD_Reg(GT_GSTID_REG, &point_status, 1);	
		
    point_count = point_status & 0x0F;

    // 检查是否有数据且触摸点数量有效
	if(point_count > 0 && point_count <= max_points)
	{
        // 2. 循环读取每个点的数据
		for(i = 0; i < point_count; i++)
		{
			GT9147_RD_Reg(GT9147_TPX_TBL[i], buf, 4);	// 读取XY坐标值
            
            uint16_t raw_x = ((uint16_t)buf[1] << 8) + buf[0];
            uint16_t raw_y = ((uint16_t)buf[3] << 8) + buf[2];
            
            // 3. 根据屏幕方向转换坐标
            // 原始逻辑假定: 
            // raw_x (buf[1]/[0]) 对应 竖屏 X (0-480)
            // raw_y (buf[3]/[2]) 对应 竖屏 Y (0-800)
            if(s_lcd_orientation == 1) // 1 = 横屏 (Landscape)
            {
                points[i].x = s_lcd_width - raw_y; // (e.g., 800 - raw_y)
                points[i].y = raw_x;
            }
            else // 0 = 竖屏 (Portrait)
            {
                points[i].x = raw_x;
                points[i].y = raw_y;
            }
            points[i].id = i; // (buf[0] 在GT9147中是Track ID, 但我们用i)
		} 
	}
    
    // 4. 清除状态寄存器，允许下一次读
	if(point_status & 0x80) // 必须检查 Buffer Status
	{
		GT9147_WR_Reg(GT_GSTID_REG, &clear_status, 1); 
	}
    
	return point_count; // 返回找到的点数
}
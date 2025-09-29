#ifndef __GRAYSCALE_SENSOR_H__
#define __GRAYSCALE_SENSOR_H__

#include "zf_common_headfile.h"

// ============================ 宏定义区 ============================
/** 灰度传感器引脚定义 */
#define GRAY_SENSOR_PIN            C6              // 灰度传感器信号引脚

// ============================ 全局变量声明区 ============================
/** 灰度传感器数据 */
extern uint8 Gray_level;                           // 当前灰度等级值

// ============================ 函数声明区 ============================
/**
 * 灰度传感器相关函数
 */
void Gray_Init(void);
uint8 Gray_Get(void);

#endif
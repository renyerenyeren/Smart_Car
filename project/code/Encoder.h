#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h> 
#include "zf_driver_encoder.h"

// ============================ 编码器硬件配置区 ============================
/**
 * 编码器编号枚举
 * 用于统一标识3个编码器，替代原1/2/3数字标识
 */
typedef enum {
    ENCODER_NUM_1,  // 编码器1
    ENCODER_NUM_2,  // 编码器2
    ENCODER_NUM_3,  // 编码器3
	ENCODER_COUNT   // 编码器总数（自动计算，用于参数校验）
} EncoderNum;

/**
 * 编码器硬件映射表
 * 包含编码器对应的定时器通道和引脚定义，集中管理硬件配置
 */
#define ENCODER_1_TIMER       QTIMER1_ENCODER1  // 编码器1使用的定时器
#define ENCODER_1_CH1_PIN     QTIMER1_ENCODER1_CH1_C0  // 编码器1通道1引脚
#define ENCODER_1_CH2_PIN     QTIMER1_ENCODER1_CH2_C1  // 编码器1通道2引脚

#define ENCODER_2_TIMER       QTIMER2_ENCODER1  // 编码器2使用的定时器
#define ENCODER_2_CH1_PIN     QTIMER2_ENCODER1_CH1_C3   // 编码器2通道1引脚
#define ENCODER_2_CH2_PIN     QTIMER2_ENCODER1_CH2_C25  // 编码器2通道2引脚

#define ENCODER_3_TIMER       QTIMER1_ENCODER2  // 编码器3使用的定时器
#define ENCODER_3_CH1_PIN     QTIMER1_ENCODER2_CH1_C2   // 编码器3通道1引脚
#define ENCODER_3_CH2_PIN     QTIMER1_ENCODER2_CH2_C24  // 编码器3通道2引脚

// ============================ 函数声明区 ============================
/**
 * 编码器初始化相关函数
 */
void Encoder_Init(void);                  // 初始化所有编码器

/**
 * 编码器数据读取相关函数
 */
int32_t Encoder_ReadSpeed(EncoderNum num);  // 读取指定编码器的速度值

#endif

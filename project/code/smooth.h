#ifndef _SMOOTH_H_
#define _SMOOTH_H_

#include "zf_common_headfile.h" 

// ============================ 枚举类型定义区 ============================
/**
 * @brief 速度平滑模式枚举
 * 区分不同运动方向的速度阈值与输出策略
 */
typedef enum {
    SPEED_MODE_BACK = 0,  // 后退模式
    SPEED_MODE_PUSH       // 推送模式
} SpeedDir;

// ============================ 宏定义区 ============================
// 后退模式 - 编码器阈值（按从小到大排序，适配二分查找）
#define BACK_ENC_THRESHOLD_1    (-16)  // 最低阈值
#define BACK_ENC_THRESHOLD_2    (-8)   // 第二阈值
#define BACK_ENC_THRESHOLD_3    (0)    // 第三阈值
#define BACK_ENC_THRESHOLD_4    (8)    // 最高阈值

// 后退模式 - 阶段速度（与阈值一一对应）
#define BACK_STAGE_SPEED_1      (50)   // 对应最低阈值（最高速度）
#define BACK_STAGE_SPEED_2      (35)   // 对应第二阈值
#define BACK_STAGE_SPEED_3      (20)   // 对应第三阈值
#define BACK_STAGE_SPEED_4      (15)   // 对应最高阈值（最低速度）

// 推送模式 - 编码器阈值（按从小到大排序，适配二分查找）
#define PUSH_ENC_THRESHOLD_1    (10)   // 最低阈值
#define PUSH_ENC_THRESHOLD_2    (20)   // 第二阈值
#define PUSH_ENC_THRESHOLD_3    (30)   // 第三阈值
#define PUSH_ENC_THRESHOLD_4    (40)   // 最高阈值

// 推送模式 - 阶段速度（与阈值一一对应）
#define PUSH_STAGE_SPEED_1      (40)   // 对应最低阈值
#define PUSH_STAGE_SPEED_2      (60)   // 对应第二阈值
#define PUSH_STAGE_SPEED_3      (50)   // 对应第三阈值
#define PUSH_STAGE_SPEED_4      (50)   // 对应最高阈值

// ============================ 函数声明区 ============================
/**
 * 速度平滑相关函数
 */
int16_t speed_smooth(uint8_t encoder_flag, int16_t encoder_val, SpeedDir mode);

#endif
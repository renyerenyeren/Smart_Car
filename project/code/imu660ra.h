#ifndef _IMU660RA_H_
#define _IMU660RA_H_

#include "zf_common_headfile.h" 

// ============================ 宏定义区 ============================
/** 陀螺仪零漂校准使能开关 */
#define IS_ZERO_BIAS                    1           // 0-关闭零漂校准；1-开启零漂校准

/** 互补滤波系数（平衡陀螺仪动态响应与加速度计静态稳定性） */
#define COMPLEMENTARY_FILTER_K          0.1f

/** 采样参数 */
#define ZERO_BIAS_SAMPLE_COUNT          500         // 零漂校准采样次数
#define SAMPLE_INTERVAL_MS              5           // 采样间隔（单位：ms）

// ============================ 结构体定义区 ============================
/**
  * @brief 陀螺仪/加速度计参数结构体
  * @details 存储各轴（X/Y/Z）的原始数据、积分角度或融合结果，用途依变量名区分
  */
typedef struct 
{
    float Xdata;  // X轴数据（角速度/角度，需结合变量语义判断）
    float Ydata;  // Y轴数据（角速度/角度，需结合变量语义判断）
    float Zdata;  // Z轴数据（角速度/角度，需结合变量语义判断）
} gyro_param_t;

// ============================ 全局变量声明区 ============================
/** 传感器核心参数 */
extern gyro_param_t Gyro_Angle;         // 陀螺仪积分计算的实时角度

/** 转向控制专用角度参数 */
extern float angle_before_turn;         // 转向启动前的初始角度（单位：度）
extern float angle_turn;                // 期望转向的目标角度（单位：度）
extern float Angle_now;                 // 当前实时角度（单位：度，用于转向闭环控制）

// ============================ 函数声明区 ============================
/**
 * 传感器初始化相关函数
 */
void imu660ra_zeroBias(void);
void my_imu660ra_init(void);

/**
 * 中断处理函数
 */
void pit_handler_1(void);

#endif
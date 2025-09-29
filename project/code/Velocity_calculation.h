#ifndef _VELOCITY_CALCULATION_H_  
#define _VELOCITY_CALCULATION_H_

#include "zf_common_headfile.h" 

// ============================ 枚举类型定义区 ============================
/**
 * @brief 运动模式枚举
 * 对应不同的速度解算逻辑，适配小车不同运动场景
 */
typedef enum {
    NORMAL,      // 正常模式：全向运动，三轮均含角速度补偿
    TURN,        // 转向模式：差速转向，参数加权调整转向灵敏度
    CORRECT      // 校正模式：姿态矫正，微调参数实现对齐
} MotionMode;

// ============================ 宏定义区 ============================
// 机械参数
#define L 1.0f                  // 小车轮距/臂长参数（需按实际机械结构调整）
// 时间与采样参数
#define IT_MS 3.0f              // 传感器采样周期（单位：ms）
// 校正系数
#define CORRECTION 0.028f       // 平动位移积分校正系数（补偿齿轮传动误差）
#define CACULATE_REVISE 1.0f    // 总位移最终修正系数
// 角度常量
#define cos60 0.5f              // 60°余弦值（固定常量）
#define sin60 0.8660254f        // 60°正弦值（固定常量）
// 工具宏
#define RADtoDEG(rad) (rad * 180.0f / PI)  // 弧度转角度（依赖zf_common_headfile.h中的PI）

// ============================ 全局变量声明区 ============================
/** 速度控制相关参数（供运动控制与解算函数共用） */
extern float V;                // 期望合速度大小
extern float theta;            // 速度方向夹角（度）
extern float w;                // 车身旋转角速度（顺时针为正）
extern float Vx;               // X轴方向分速度
extern float Vy;               // Y轴方向分速度

/** 位移解算相关参数（解算结果存储与使能控制） */
extern bool calculate_flag;    // 位移解算使能标志（true：计算；false：重置）
extern float shift_yaw;        // 累计偏航角度（度）
extern float shift_distance;   // 累计总位移（无单位）
extern float x_distance;       // X轴累计位移（无单位）
extern float y_distance;       // Y轴累计位移（无单位）

// ============================ 函数声明区 ============================
/**
 * 速度解算相关函数
 */
void calculateSpeeds(MotionMode mode, float V, float theta, float w, int* V1, int* V2, int* V3);
void calculate_angle_with_y_axis(float x, float y, float *V, float *theta);

/**
 * 运动控制相关函数
 */
void turn_around(float R, int8_t direction, float want_Vx, float *Vx, float *w);
void move(int16_t angle, int8_t speed);

/**
 * 位移解算相关函数
 */
void translate_shift(int motor_1_speed, int motor_2_speed, int motor_3_speed);

#endif
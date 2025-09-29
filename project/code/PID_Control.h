#ifndef _PID_CONTROL_H_
#define _PID_CONTROL_H_

#include "zf_common_headfile.h" 

// ============================ 宏定义区 ============================
/** 控制参数宏定义 */
#define TARGET_ANGLE_X                 0           // 目标X轴角度
#define TARGET_POSITION_Y              135         // 目标Y轴位置
#define TARGET_SLOPE                   0           // 目标斜率


/** 运动参数宏定义 */
#define DEFAULT_SPEED                  8000
#define SPEED_1M6                      9000
#define SPEED_1M7                      10000
#define SPEED_FASTER                   12000
#define DEFAULT_TURNING_RADIUS         4.2f        // 默认转弯半径

// ============================ 全局变量声明区 ============================
/** 控制模式与状态标志 */
extern bool SPEED_ENABLE;                          // 速度开关标志位
extern uint8_t Motor_Control_Mode;                 // 控制模式变量

/** 运动控制参数 */
extern int8 rotation_direction;                    // 车身旋转方向（-1：逆时针，1：顺时针）
extern int8 tangential_linear_speed;               // 旋转时的切向线速度

/** 速度相关参数 */
extern int16 tracking_speed;                       // 循迹速度，y方向
extern int16 last_speed;                           // 循迹模式下的速度一阶滤波
extern int speed_compensation_offset;              // 速度补偿偏移量

/** 电机控制参数 */
extern int Read_wheelspeed[3];                     // 读取的三轮速度
extern int wheel[3];                               // 解算的三个轮子相应的目标速度

// ============================ 函数声明区 ============================
/**
 * 猛冲
 */
void rushrushrush(void);

/**
 * 电机控制相关函数
 */
void Motor_Control(void);
void pit_handler_0(void);

#endif


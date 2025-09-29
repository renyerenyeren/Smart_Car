#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "zf_common_headfile.h" 

// ============================ 枚举类型定义区 ============================
/**
 * @brief 电机编号枚举
 */
typedef enum {
    MOTOR_NUM_1 = 0,  // 电机1
    MOTOR_NUM_2,      // 电机2
    MOTOR_NUM_3,      // 电机3
    MOTOR_COUNT       // 电机总数（自动计算，用于参数校验）
} MotorNum;

// ============================ 宏定义区 ============================
/**
 * 电机硬件配置（按电机编号集中管理，修改时仅需调整此处）
 */
// 电机1：PWM引脚 + 方向引脚
#define MOTOR1_PWM_PIN    PWM4_MODULE3_CHA_C31
#define MOTOR1_DIR_PIN    C30
// 电机2：PWM引脚 + 方向引脚
#define MOTOR2_PWM_PIN    PWM1_MODULE3_CHB_D1
#define MOTOR2_DIR_PIN    D3
// 电机3：PWM引脚 + 方向引脚
#define MOTOR3_PWM_PIN    PWM1_MODULE3_CHA_D0
#define MOTOR3_DIR_PIN    D2

// 电机通用参数
#define MOTOR_PWM_FREQ    17000UL  // PWM频率（17kHz）
#define MOTOR_INIT_DUTY   0UL      // 初始占空比（0）
#define MOTOR_MAX_DUTY    7500     // 最大占空比限制（防止过流）

// ============================ 函数声明区 ============================
/**
 * 电机初始化相关函数
 */
void motor_Init(void);  // 初始化所有电机的PWM和方向引脚

/**
 * 电机控制相关函数
 */
void motor_set_duty(MotorNum motor_num, int duty);  // 设置指定电机的占空比与方向

#endif
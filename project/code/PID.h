#ifndef __PID_H_
#define __PID_H_

#include "zf_common_headfile.h" 

// ============================ 枚举类型定义区 ============================
/**
  * @brief 速度环状态枚举
  *        对应不同的PID参数配置，适应不同运动场景需求
  */
typedef enum 
{
    Soft,      // 软性参数：响应平缓，适用于平稳运动
    Hard,      // 硬性参数：响应迅速，适用于快速响应
    Withy,     // 柔性参数：适中响应，适用于一般场景
    Strong     // 强性参数：强力响应，适用于高负载场景
} VelocityState;

// ============================ 结构体定义区 ============================
/**
  * @brief 标准PID结构体
  * @details 含P/I/D三项，支持积分限幅与输出限幅
  *          适用场景：PositionX_Second_Correct、PositionY_Second_Correct等标准PID控制
  */
typedef struct 
{
    /* 控制参数 */
    float Kp;                   // 比例系数
    float Ki;                   // 积分系数
    float Kd;                   // 微分系数
    /* 运行状态 */
    float last_err;             // 上一次误差
    float integral;             // 积分项累加值
    /* 配置参数 */
    float integral_limit;       // 积分限幅
    float output_limit;         // 输出限幅
} StdPID_HandleTypeDef;

/**
  * @brief 自适应参数PID结构体
  * @details 根据误差大小切换Kp/Kd参数，支持PD控制
  *          适用场景：AdaptiveRobust_PositionY_Correct等自适应参数场景
  */
typedef struct 
{
    /* 自适应参数组 */
    float Kp_large;             // 大误差区域比例系数
    float Kd_large;             // 大误差区域微分系数
    float Kp_small;             // 小误差区域比例系数
    float Kd_small;             // 小误差区域微分系数
    /* 运行状态 */
    float last_err;             // 上一次误差
    /* 配置参数 */
    float err_threshold;        // 误差阈值（区分大小误差区域）
    float output_limit;         // 输出限幅
} AdaptivePID_HandleTypeDef;

/**
  * @brief 转向专用PID结构体
  * @details 支持动态P项，含Kp动态调整
  *          适用场景：Turn函数等转向控制场景
  */
typedef struct 
{
    /* 基础控制参数 */
    float base_Kp;              // 基础比例系数
    float Kd;                   // 微分系数
    /* 动态P项配置 */
    float k_value;              // 动态项比例系数（与速度配置关联）
    float dynamic_term_limit;   // 动态项限幅
    /* 运行状态 */
    float last_err;             // 上一次误差
    /* 配置参数 */
    float output_limit;         // 输出限幅
} turnPID_HandleTypeDef;

/**
  * @brief 速度环专用PID结构体
  * @details 增量式PID + 误差低通滤波
  *          适用场景：Velocity函数等速度控制场景
  */
typedef struct 
{
    /* 控制参数 */
    float Kp;                   // 比例系数（原Velocity_Kp）
    float Ki;                   // 积分系数（原Velocity_Ki）
    /* 运行状态 */
    float error_last;           // 上一次滤波后误差
    float error_lowout;         // 当前滤波后误差
    float output;               // 累加后的输出值
    /* 配置参数 */
    float filter_alpha;         // 低通滤波系数（a越大，滤波越强）
    float output_limit;         // 输出限幅
} VelocityPID_HandleTypeDef;

/**
  * @brief 速度配置结构体
  * @details 供转向PID与业务代码关联，需根据实际业务场景调整成员变量
  */
typedef struct 
{
    float turn_kp;              // 转向PID基础Kp
    float turn_kd;              // 转向PID Kd
    float k_value;              // 转向动态项比例系数
    float dynamic_term_limit;   // 转向动态项限幅
    float output_limit;         // 转向输出限幅
} SpeedConfigTypeDef;

// ============================ 全局变量声明区 ============================
/** 速度环PID句柄（3个电机，对应原controllers[0~2]） */
extern VelocityPID_HandleTypeDef hvel_pid[3];

/** 转向专用PID句柄 */
extern turnPID_HandleTypeDef hturn_pid;

/** 自适应X轴角度控制PID句柄 */
extern AdaptivePID_HandleTypeDef hadaptive_anglex_pid;

/** 自适应Y轴位置校正PID句柄 */
extern AdaptivePID_HandleTypeDef hadaptive_posy_pid;

/** 角度PID句柄 */
extern StdPID_HandleTypeDef hangle_pid;

/** Y轴保持PID句柄 */
extern StdPID_HandleTypeDef hposy_keep_pid;

/** X轴保持PID句柄 */
extern StdPID_HandleTypeDef hposx_keep_pid;

// ============================ 函数声明区 ============================
/**
 * PID计算相关函数
 */
float StdPID_Calculate(StdPID_HandleTypeDef *hpid, float actual, float target);
float AdaptivePID_Calculate(AdaptivePID_HandleTypeDef *hpid, float actual, float target);
float turnPID_Calculate(turnPID_HandleTypeDef *hpid, float actual, float target);
int VelocityPID_Calculate(VelocityPID_HandleTypeDef *hpid, int actual_speed, int target_speed);

/**
 * 初始化与配置函数
 */
void PID_Init(void);
void change_Velocity(VelocityState x);

#endif /* __PID_H */
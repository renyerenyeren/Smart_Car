#include "PID.h"

// ============================ 全局变量定义区 ============================
/** 全局速度环PID句柄（3个电机） */
VelocityPID_HandleTypeDef hvel_pid[3];

/** 转向专用PID句柄 */
turnPID_HandleTypeDef hturn_pid;

/** 自适应X轴角度控制PID句柄 */
AdaptivePID_HandleTypeDef hadaptive_anglex_pid;

/** 自适应Y轴位置校正PID句柄 */
AdaptivePID_HandleTypeDef hadaptive_posy_pid;

/** 角度保持PID句柄 */
StdPID_HandleTypeDef hangle_pid;

/** Y轴保持PID句柄 */
StdPID_HandleTypeDef hposy_keep_pid;

/** X轴保持PID句柄 */
StdPID_HandleTypeDef hposx_keep_pid;

// ============================ 函数定义区 ============================
/**
  * 函    数：StdPID_Calculate
  * 功    能：标准PID计算实现
  * 参    数：hpid   - 标准PID结构体指针
  * 参    数：actual - 实际测量值
  * 参    数：target - 目标值
  * 返 回 值：PID输出值（已限幅）
  * 备    注：含P/I/D三项，支持积分限幅与输出限幅
  */
float StdPID_Calculate(StdPID_HandleTypeDef *hpid, float actual, float target) 
{
    if (hpid == NULL) return 0.0f;

    // 1. 计算当前误差（实际值 - 目标值，与原代码逻辑一致）
    float err = actual - target;

    // 2. 积分项计算（含限幅）
    hpid->integral += err;
    hpid->integral = func_limit(hpid->integral, hpid->integral_limit);

    // 3. 微分项计算（误差变化量）
    float derivative = err - hpid->last_err;

    // 4. PID输出计算
    float output = hpid->Kp * err + 
                   hpid->Ki * hpid->integral + 
                   hpid->Kd * derivative;

    // 5. 输出限幅
    output = func_limit(output, hpid->output_limit);

    // 6. 更新上一次误差
    hpid->last_err = err;

    return output;
}

/**
  * 函    数：AdaptivePID_Calculate
  * 功    能：自适应参数PID计算实现
  * 参    数：hpid   - 自适应PID结构体指针
  * 参    数：actual - 实际测量值
  * 参    数：target - 目标值
  * 返 回 值：PID输出值（已限幅）
  * 备    注：根据误差大小选择参数组，PD控制
  */
float AdaptivePID_Calculate(AdaptivePID_HandleTypeDef *hpid, float actual, float target) 
{
    if (hpid == NULL) return 0.0f;

    // 1. 计算当前误差（实际值 - 目标值，与原代码逻辑一致）
    float err = actual - target;

    // 2. 根据误差大小选择参数组
    float current_Kp, current_Kd;
    if (fabs(err) > hpid->err_threshold) 
    {
        // 大误差区域：强响应参数
        current_Kp = hpid->Kp_large;
        current_Kd = hpid->Kd_large;
    } 
    else 
    {
        // 小误差区域：弱响应参数（减少超调）
        current_Kp = hpid->Kp_small;
        current_Kd = hpid->Kd_small;
    }

    // 3. 微分项计算（误差变化量）
    float derivative = err - hpid->last_err;

    // 4. PD输出计算
    float output = current_Kp * err + current_Kd * derivative;

    // 5. 输出限幅
    output = func_limit(output, hpid->output_limit);

    // 6. 更新上一次误差
    hpid->last_err = err;

    return output;
}

/**
  * 函    数：turnPID_Calculate
  * 功    能：转向专用PID计算实现
  * 参    数：hpid   - 转向PID结构体指针
  * 参    数：actual - 实际测量值
  * 参    数：target - 目标值
  * 返 回 值：PID输出值（已限幅）
  * 备    注：含动态P项，PD控制
  */
float turnPID_Calculate(turnPID_HandleTypeDef *hpid, float actual, float target) 
{
    if (hpid == NULL) return 0.0f;

    float err = actual - target;
    float dynamic_term = err * err * hpid->k_value;
    dynamic_term = func_limit(dynamic_term, hpid->dynamic_term_limit);
    
    float current_Kp = hpid->base_Kp + dynamic_term;
    float derivative = err - hpid->last_err;
    float output = current_Kp * err + hpid->Kd * derivative;
    output = func_limit(output, hpid->output_limit);
    hpid->last_err = err;

    return output;
}

/**
  * 函    数：VelocityPID_Calculate
  * 功    能：速度环专用PID计算实现
  * 参    数：hpid         - 速度环PID结构体指针
  * 参    数：actual_speed - 实际速度
  * 参    数：target_speed - 目标速度
  * 返 回 值：PID输出值（已限幅，int类型兼容原接口）
  * 备    注：增量式PID + 误差低通滤波
  */
int VelocityPID_Calculate(VelocityPID_HandleTypeDef *hpid, int actual_speed, int target_speed)	
{
    if (hpid == NULL) return 0;

    int raw_err = actual_speed - target_speed;
    hpid->error_lowout = hpid->error_last * hpid->filter_alpha + raw_err * (1 - hpid->filter_alpha);
    int delta_err = hpid->error_lowout - hpid->error_last;
    int delta_out = (int)(hpid->Kp * delta_err + hpid->Ki * hpid->error_lowout);

    hpid->output += delta_out;
    hpid->output = func_limit(hpid->output, hpid->output_limit);
    hpid->error_last = hpid->error_lowout;

    return (int)hpid->output;
}

/**
  * 函    数：PID_Init
  * 功    能：PID模块全局初始化
  * 参    数：无
  * 返 回 值：无
  * 备    注：初始化所有PID句柄参数
  */
void PID_Init(void) 
{
    // 速度环PID初始化（3个电机，默认Soft状态参数）
    for (uint8_t i = 0; i < 3; i++) 
    {
        hvel_pid[i].Kp = -45.0f;
        hvel_pid[i].Ki = -2.95f;
        hvel_pid[i].filter_alpha = 0.3f;   // 原代码滤波系数
        hvel_pid[i].output_limit = 7500.0f;
        hvel_pid[i].error_last = 0.0f;
        hvel_pid[i].error_lowout = 0.0f;
        hvel_pid[i].output = 0.0f;
    }

    // 转向专用PID初始化
    hturn_pid.base_Kp = 0.285f;
    hturn_pid.Kd = 7.0f;
    hturn_pid.k_value = 0.0000225f;
    hturn_pid.dynamic_term_limit = 0.045f;
    hturn_pid.output_limit = 70.0f;
    hturn_pid.last_err = 0.0f;

    // 自适应X轴角度控制PID初始化
    hadaptive_anglex_pid.Kp_large = 0.195f;  // Kp_min + (Kp_max - Kp_min) * 0.3f
    hadaptive_anglex_pid.Kd_large = 3.5f;    // Kd_max
    hadaptive_anglex_pid.Kp_small = 0.1f;    // Kp_min
    hadaptive_anglex_pid.Kd_small = 1.5f;    // Kd_min
    hadaptive_anglex_pid.err_threshold = 15.0f;
    hadaptive_anglex_pid.output_limit = 60.0f;
    hadaptive_anglex_pid.last_err = 0.0f;

    // 自适应Y轴位置校正PID初始化
    hadaptive_posy_pid.Kp_large = -0.645f;
    hadaptive_posy_pid.Kd_large = -4.95f;
    hadaptive_posy_pid.Kp_small = -0.4f;
    hadaptive_posy_pid.Kd_small = -2.5f;
    hadaptive_posy_pid.err_threshold = 15.0f;
    hadaptive_posy_pid.output_limit = 30.0f;
    hadaptive_posy_pid.last_err = 0.0f;

	 // 角度保持PID初始化
    hangle_pid.Kp = -0.55f;
    hangle_pid.Ki = 0.0f;
    hangle_pid.Kd = -0.25f;
    hangle_pid.integral_limit = 0.0f;
    hangle_pid.output_limit = 70.0f;
    hangle_pid.last_err = 0.0f;
    hangle_pid.integral = 0.0f;

    // Y轴保持PID初始化
    hposy_keep_pid.Kp = -0.75f;
    hposy_keep_pid.Ki = 0.0f;
    hposy_keep_pid.Kd = -0.5f;
    hposy_keep_pid.integral_limit = 0.0f;
    hposy_keep_pid.output_limit = 10.0f;
    hposy_keep_pid.last_err = 0.0f;
    hposy_keep_pid.integral = 0.0f;

    // X轴保持PID初始化
    hposx_keep_pid.Kp = 1.785f;
    hposx_keep_pid.Ki = 0.0f;
    hposx_keep_pid.Kd = 1.85f;
    hposx_keep_pid.integral_limit = 0.0f;
    hposx_keep_pid.output_limit = 5.0f;
    hposx_keep_pid.last_err = 0.0f;
    hposx_keep_pid.integral = 0.0f;
}

/**
  * 函    数：change_Velocity
  * 功    能：改变速度环PID参数
  * 参    数：x - 速度环状态（Soft/Hard/Withy/Strong）
  * 返 回 值：无
  * 备    注：根据状态切换不同的PID参数配置
  */
void change_Velocity(VelocityState x) 
{
    float kp_val, ki_val;

    // 根据状态选择参数
    switch (x) 
    {
        case Soft:
            kp_val = -45.0f;
            ki_val = -2.95f;
            break;
        case Hard:
            kp_val = -60.0f;
            ki_val = -15.0f;
            break;
        case Withy:
            kp_val = -50.0f;
            ki_val = -5.95f;
            break;
        case Strong:
            kp_val = -65.0f;
            ki_val = -20.0f;
            break;
        default:
            kp_val = -45.0f;
            ki_val = -2.95f;
            break;
    }

    // 更新3个电机的速度环参数
    for (uint8_t i = 0; i < 3; i++) 
    {
        hvel_pid[i].Kp = kp_val;
        hvel_pid[i].Ki = ki_val;
    }
}
#include "imu660ra.h"

// ============================ 全局变量定义区 ============================
/** 传感器核心参数初始化（显式置0，避免编译器随机值） */
gyro_param_t Gyro_Bias = {0.0f, 0.0f, 0.0f};    // 零漂校准值初始化为0
gyro_param_t Gyro_Angle = {0.0f, 0.0f, 0.0f};   // 陀螺仪积分角度初始化为0
gyro_param_t Acc_Angle = {0.0f, 0.0f, 0.0f};    // 加速度计角度初始化为0
gyro_param_t Fusion_Angle = {0.0f, 0.0f, 0.0f}; // 融合角度初始化为0

/** 传感器转换后数据初始化 */
float tra_acc_x = 0.0f;
float tra_acc_y = 0.0f;
float tra_acc_z = 0.0f;
float tra_gyro_x = 0.0f;
float tra_gyro_y = 0.0f;
float tra_gyro_z = 0.0f;

/** 转向控制角度初始化 */
float angle_before_turn = 0.0f;  // 初始角度默认0度
float angle_turn = 0.0f;         // 目标转向角度默认0度
float Angle_now = 0.0f;          // 当前角度默认0度

// ============================ 函数定义区 ============================
/**
  * 函    数：imu660ra_zeroBias
  * 功    能：陀螺仪零漂校准，计算Z轴零漂补偿值
  * 参    数：无
  * 返 回 值：无
  * 备    注：1. 仅在IS_ZERO_BIAS宏定义为真时执行校准；
  *           2. 通过500次采样取平均降低噪声影响；
  *           3. 采样间隔5ms，总校准时间约2.5秒
  */
void imu660ra_zeroBias(void)
{
#if IS_ZERO_BIAS  // 简化宏判断
    Gyro_Bias.Zdata = 0.0f;  // 初始化Z轴零漂累加器
    // 多次采集降低随机噪声，取平均作为零漂补偿值
    for (uint16_t i = 0; i < 500; i++)
    {
        imu660ra_get_gyro();  // 读取陀螺仪原始寄存器数据
        // 累加转换后的Z轴角速度（×0.005需结合硬件文档确认含义，疑似临时系数）
        Gyro_Bias.Zdata += (imu660ra_gyro_transition(imu660ra_gyro_z) * 0.005f);
        system_delay_ms(5);   // 采样间隔5ms，与中断周期匹配
    }
    Gyro_Bias.Zdata /= 500.0f;  // 求平均值，得到最终零漂校准值
#endif
}

/**
  * 函    数：my_imu660ra_init
  * 功    能：初始化IMU660RA传感器及相关参数
  * 参    数：无
  * 返 回 值：无
  * 备    注：1. 先调用官方初始化函数，再重置角度相关参数；
  *           2. 显式将所有角度参数置0，确保初始状态一致；
  *           3. 需在系统启动时调用一次
  */
void my_imu660ra_init(void)
{
    imu660ra_init();
    
    Gyro_Angle.Zdata = 0.0f;
    Gyro_Angle.Ydata = 0.0f;
    Gyro_Bias.Zdata = 0.0f;
    Acc_Angle.Xdata = 0.0f;
    Acc_Angle.Ydata = 0.0f;
    Acc_Angle.Zdata = 0.0f;
    Fusion_Angle.Ydata = 0.0f;
}

/**
  * 函    数：pit_handler_1
  * 功    能：定时器中断处理函数，实时更新陀螺仪角度
  * 参    数：无
  * 返 回 值：无
  * 备    注：1. 建议配置为5ms周期，与采样间隔保持一致；
  *           2. 角度计算公式：角度 = 角速度×时间（时间单位：秒）；
  *           3. 0.005f对应5ms中断周期，需与硬件定时器配置匹配；
  *           4. 包含零漂补偿，提高角度计算精度
  */
void pit_handler_1(void)
{
    // 读取并转换陀螺仪数据（原始值→度/秒）
    imu660ra_get_gyro();
    tra_gyro_z = imu660ra_gyro_transition(imu660ra_gyro_z);
    
    // 陀螺仪角度积分（角度 = 角速度×时间，时间单位：秒）
    // 注：0.005f对应5ms中断周期，需与硬件定时器配置保持一致
    Gyro_Angle.Zdata += (tra_gyro_z*0.005-Gyro_Bias.Zdata);
    
    // 更新当前角度（优先使用融合角度，此处保留原逻辑）
    Angle_now = Gyro_Angle.Zdata;
}

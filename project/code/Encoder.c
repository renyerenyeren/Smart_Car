#include "Encoder.h"

// ============================ 静态变量定义区 ============================
// 编码器定时器句柄数组，与EncoderNum枚举对应，简化索引访问
static const encoder_index_enum g_encoderTimers[] = {
    ENCODER_1_TIMER,
    ENCODER_2_TIMER,
    ENCODER_3_TIMER
};

// ============================ 编码器初始化函数 ============================
/**
  * 函    数：Encoder_Init
  * 功    能：初始化所有编码器硬件（配置定时器和引脚）
  * 参    数：无
  * 返 回 值：无
  */
void Encoder_Init(void)
{
    // 初始化编码器
    encoder_quad_init(ENCODER_1_TIMER, ENCODER_1_CH1_PIN, ENCODER_1_CH2_PIN);
    encoder_quad_init(ENCODER_2_TIMER, ENCODER_2_CH1_PIN, ENCODER_2_CH2_PIN);
    encoder_quad_init(ENCODER_3_TIMER, ENCODER_3_CH1_PIN, ENCODER_3_CH2_PIN);
}

// ============================ 编码器速度读取函数 ============================
/**
  * 函    数：Encoder_ReadSpeed
  * 功    能：读取指定编码器的速度值（带符号，反映方向）
  * 参    数：num - 编码器编号（ENCODER_NUM_1 ~ ENCODER_NUM_3）
  * 返 回 值：int32_t - 速度值（经过2分频处理，负号表示方向反转）
  * 备    注：1. 读取后会清除当前计数值，为下次采样做准备；
  *           2. 非法编号返回0，增强容错性
  */
int32_t Encoder_ReadSpeed(EncoderNum num)
{
    // 检查编号合法性
    if (num >= ENCODER_COUNT) 
	{
        return 0;
    }
    // 读取计数并转换为速度值
    int32_t speed = -encoder_get_count(g_encoderTimers[num]);
    // 清除计数
    encoder_clear_count(g_encoderTimers[num]);
    
    return speed;
}

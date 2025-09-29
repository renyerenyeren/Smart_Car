#include "smooth.h"

// ============================ 核心功能函数区 ============================
/**
  * 函    数：speed_smooth
  * 功    能：根据编码器值与运动模式计算平滑速度
  * 参    数：encoder_flag - 编码器有效标志（0：无效；1：有效）
  * 参    数：encoder_val  - 编码器Y轴值（用于匹配速度阈值）
  * 参    数：mode         - 运动模式（SPEED_MODE_BACK/SPPED_MODE_PUSH）
  * 返 回 值：int16_t - 平滑后的输出速度值（无效时返回0）
  * 备    注：1. 采用二分查找匹配阈值，提升查询效率；
  *           2. 阈值需按从小到大排序，否则二分查找逻辑失效；
  *           3. 模式参数非法或编码器无效时，返回0作为安全值
  */
int16_t speed_smooth(uint8_t encoder_flag, int16_t encoder_val, SpeedDir mode)
{
    // 快速退出：编码器无效直接返回0
    if (!encoder_flag) 
    {
        return 0;
    }

    // 模式合法性校验：非法模式返回0
    if (mode > SPEED_MODE_PUSH) 
    {
        return 0;
    }

    // 阈值与速度参数表（与头文件宏定义一一对应，按模式索引）
    static const int16_t thresholds[2][4] = {
        {BACK_ENC_THRESHOLD_1, BACK_ENC_THRESHOLD_2, BACK_ENC_THRESHOLD_3, BACK_ENC_THRESHOLD_4},
        {PUSH_ENC_THRESHOLD_1, PUSH_ENC_THRESHOLD_2, PUSH_ENC_THRESHOLD_3, PUSH_ENC_THRESHOLD_4}
    };
    static const int16_t speeds[2][4] = {
        {BACK_STAGE_SPEED_1, BACK_STAGE_SPEED_2, BACK_STAGE_SPEED_3, BACK_STAGE_SPEED_4},
        {PUSH_STAGE_SPEED_1, PUSH_STAGE_SPEED_2, PUSH_STAGE_SPEED_3, PUSH_STAGE_SPEED_4}
    };
    static const uint8_t THRESHOLD_CNT = 4;  // 每个模式的阈值数量（固定为4级）

    // 二分查找：匹配编码器值对应的阈值索引
    uint8_t low = 0, high = THRESHOLD_CNT - 1, mid;
    while (low <= high) 
    {
        mid = (low + high) / 2;
        if (encoder_val > thresholds[mode][mid]) 
        {
            // 编码器值大于当前阈值，查找更高阈值区间
            high = mid - 1;
        } 
        else 
        {
            // 编码器值小于等于当前阈值，查找更低阈值区间
            low = mid + 1;
        }
    }

    // 返回匹配的速度值（low为匹配索引，超出范围时返回最后一级速度）
    return (low < THRESHOLD_CNT) ? speeds[mode][low] : speeds[mode][THRESHOLD_CNT - 1];
}
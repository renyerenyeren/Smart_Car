#include "IMAGE_Control.h"

// ============================ 全局变量定义 ============================
int16 Slope = 0;                            // 图像斜率，用于路径方向判断
uint8 Image_Mode = 0;                       // 图像处理模式选择
int Threshold;                              // 图像二值化阈值
float Symmetry_Ratio = 0.0f;                // 图像左右对称率，用于推箱子等场景

// ============================ 函数实现 ============================
/**
 * 函    数：Image_Control
 * 功    能：根据当前图像模式执行相应的图像处理逻辑
 * 参    数：无
 * 返 回 值：无
 * 备    注：支持多种图像处理模式，根据Image_Mode切换功能
 */
void Image_Control(void)
{
    switch (Image_Mode)
    {
        case 0:  // 标准循迹模式（包含十字和环岛检测）
            find_middle();         // 计算路径中线
            Island_Detect();       // 检测环岛并处理
            Cross_Detect();        // 检测十字并处理
            Slope = slope(Slope_Mode);  // 计算路径斜率
            break;
            
        case 3:  // 简化循迹模式（仅循迹，不检测十字和环岛）
            find_middle();         // 计算路径中线
            Slope = slope(Slope_Mode);  // 计算路径斜率
            break;
            
        case 4:  // 等待模式（不执行任何图像处理）
            break;
            
        case 7:  // 对称率计算模式（用于推箱子等需要对称操作的场景）
            // 自适应计算二值化阈值
            Threshold = my_adapt_threshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
            // 执行图像二值化处理
            Image_Binarization(Threshold);
            // 计算指定区域的加权对称率
            Symmetry_Ratio = Get_Weighted_Symmetry(0, 45, MT9V03X_W, 75);
            break;
    }
}

/**
 * 函    数：pit_handler_2
 * 功    能：周期性中断处理函数，触发图像处理
 * 参    数：无
 * 返 回 值：无
 * 备    注：当图像采集完成后，调用图像处理函数
 */
void pit_handler_2()
{
    // 检查图像采集完成标志
    if (mt9v03x_finish_flag)
    {
        mt9v03x_finish_flag = 0;  // 清除标志位
        Image_Control();          // 执行图像处理
    }
}

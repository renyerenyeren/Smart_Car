#ifndef _IMAGE_CONTROL_H_
#define _IMAGE_CONTROL_H_

#include "zf_common_headfile.h"

// ============================ 外部变量声明 ============================
extern uint8 Image_Mode;                     // 图像处理模式
extern int16 Slope;                         // 图像斜率
extern int Threshold;                        // 图像二值化阈值
extern float Symmetry_Ratio;                 // 图像左右对称率

// ============================ 函数声明 ============================
void Image_Control(void);                    // 图像处理控制函数
void pit_handler_2(void);                    // 定时器中断处理函数

#endif

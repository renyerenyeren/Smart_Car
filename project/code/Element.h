#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include "zf_common_headfile.h" 

// ============================ 外部变量声明区 ============================
/** 环岛状态相关变量 */
extern int Island_State;                     // 环岛状态标志
extern int Left_Island_Flag;                 // 左环岛标志
extern int Right_Island_Flag;                // 右环岛标志
extern int continuity_change_right_flag;      // 右边界连续性标志（连续为0）
extern int continuity_change_left_flag;   // 左边界连续性标志（连续为0）
extern int monotonicity_change_left_flag;     // 左边界单调性变化标志（不变化为0）
extern int monotonicity_change_right_flag;    // 右边界单调性变化标志（不变化为0）
extern int monotonicity_change_column[2];     // 单调性变化点坐标（[0]行, [1]列）
extern int monotonicity_change_line[2];       // 单调性变化点坐标（[0]行, [1]列）
extern int right_down_guai[2];                // 右下拐点坐标（[0]行, [1]列）

// ============================ 函数声明区 ============================
/**
 * 始终线检测相关函数
 */
uint8 find_start_finish_line();

/**
 * 十字检测相关函数
 */
void Cross_Detect();

/**
 * 环岛检测相关函数
 */
void Island_Detect();

#endif
    
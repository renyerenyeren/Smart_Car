#ifndef _ELEM_PROC_H_
#define _ELEM_PROC_H_

#include "zf_common_headfile.h"

// ============================ 外部变量声明区 ============================
/** 二值化图像数组 */
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W];

// ============================ 函数声明区 ============================
/**
 * 画线与补线相关函数
 */
void Draw_Line(int startX, int startY, int endX, int endY);
void K_Draw_Line(float k, int startX, int startY, int endY);

/**
 * 赛道连续性检测函数
 */
int Continuity_Change_Left(int start, int end);
int Continuity_Change_Right(int start, int end);

/**
 * 角点检测相关函数
 */
int Find_Left_Down_Point(int start, int end);
int Find_Left_Up_Point(int start, int end);
int Find_Right_Down_Point(int start, int end);
int Find_Right_Up_Point(int start, int end);

/**
 * 单调性检测相关函数
 */
int Monotonicity_Change_Right_Land(int start, int end);
int Monotonicity_Change_Left(int start, int end);
int Monotonicity_Change_Right(int start, int end);

/**
 * 边界补线与延长函数
 */
void K_Add_Boundry_Left(float k, int startX, int startY, int endY);
void K_Add_Boundry_Right(float k, int startX, int startY, int endY);
float Get_Left_K(int start_line, int end_line);
float Get_Right_K(int start_line, int end_line);
void Lengthen_Left_Boundry(int start, int end);
void Lengthen_Right_Boundry(int start, int end);
void Left_Add_Line(int x1, int y1, int x2, int y2);
void Right_Add_Line(int x1, int y1, int x2, int y2);

/**
 * 图像二值化相关函数
 */
int my_adapt_threshold(uint8 *image, uint16 col, uint16 row);
int My_Adapt_Threshold(uint8* image, uint16 width, uint16 height);
void Image_Binarization(int threshold);

/**
 * 图像对称性分析函数
 */
float Get_Weighted_Symmetry(int x, int y, int w, int h);

#endif
    
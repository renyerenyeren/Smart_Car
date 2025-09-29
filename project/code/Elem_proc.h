#ifndef _ELEM_PROC_H_
#define _ELEM_PROC_H_

#include "zf_common_headfile.h"

// ============================ �ⲿ���������� ============================
/** ��ֵ��ͼ������ */
extern uint8 image_two_value[MT9V03X_H][MT9V03X_W];

// ============================ ���������� ============================
/**
 * �����벹����غ���
 */
void Draw_Line(int startX, int startY, int endX, int endY);
void K_Draw_Line(float k, int startX, int startY, int endY);

/**
 * ���������Լ�⺯��
 */
int Continuity_Change_Left(int start, int end);
int Continuity_Change_Right(int start, int end);

/**
 * �ǵ�����غ���
 */
int Find_Left_Down_Point(int start, int end);
int Find_Left_Up_Point(int start, int end);
int Find_Right_Down_Point(int start, int end);
int Find_Right_Up_Point(int start, int end);

/**
 * �����Լ����غ���
 */
int Monotonicity_Change_Right_Land(int start, int end);
int Monotonicity_Change_Left(int start, int end);
int Monotonicity_Change_Right(int start, int end);

/**
 * �߽粹�����ӳ�����
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
 * ͼ���ֵ����غ���
 */
int my_adapt_threshold(uint8 *image, uint16 col, uint16 row);
int My_Adapt_Threshold(uint8* image, uint16 width, uint16 height);
void Image_Binarization(int threshold);

/**
 * ͼ��Գ��Է�������
 */
float Get_Weighted_Symmetry(int x, int y, int w, int h);

#endif
    
#ifndef _IMAGE_CONTROL_H_
#define _IMAGE_CONTROL_H_

#include "zf_common_headfile.h"

// ============================ �ⲿ�������� ============================
extern uint8 Image_Mode;                     // ͼ����ģʽ
extern int16 Slope;                         // ͼ��б��
extern int Threshold;                        // ͼ���ֵ����ֵ
extern float Symmetry_Ratio;                 // ͼ�����ҶԳ���

// ============================ �������� ============================
void Image_Control(void);                    // ͼ������ƺ���
void pit_handler_2(void);                    // ��ʱ���жϴ�����

#endif

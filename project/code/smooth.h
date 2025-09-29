#ifndef _SMOOTH_H_
#define _SMOOTH_H_

#include "zf_common_headfile.h" 

// ============================ ö�����Ͷ����� ============================
/**
 * @brief �ٶ�ƽ��ģʽö��
 * ���ֲ�ͬ�˶�������ٶ���ֵ���������
 */
typedef enum {
    SPEED_MODE_BACK = 0,  // ����ģʽ
    SPEED_MODE_PUSH       // ����ģʽ
} SpeedDir;

// ============================ �궨���� ============================
// ����ģʽ - ��������ֵ������С��������������ֲ��ң�
#define BACK_ENC_THRESHOLD_1    (-16)  // �����ֵ
#define BACK_ENC_THRESHOLD_2    (-8)   // �ڶ���ֵ
#define BACK_ENC_THRESHOLD_3    (0)    // ������ֵ
#define BACK_ENC_THRESHOLD_4    (8)    // �����ֵ

// ����ģʽ - �׶��ٶȣ�����ֵһһ��Ӧ��
#define BACK_STAGE_SPEED_1      (50)   // ��Ӧ�����ֵ������ٶȣ�
#define BACK_STAGE_SPEED_2      (35)   // ��Ӧ�ڶ���ֵ
#define BACK_STAGE_SPEED_3      (20)   // ��Ӧ������ֵ
#define BACK_STAGE_SPEED_4      (15)   // ��Ӧ�����ֵ������ٶȣ�

// ����ģʽ - ��������ֵ������С��������������ֲ��ң�
#define PUSH_ENC_THRESHOLD_1    (10)   // �����ֵ
#define PUSH_ENC_THRESHOLD_2    (20)   // �ڶ���ֵ
#define PUSH_ENC_THRESHOLD_3    (30)   // ������ֵ
#define PUSH_ENC_THRESHOLD_4    (40)   // �����ֵ

// ����ģʽ - �׶��ٶȣ�����ֵһһ��Ӧ��
#define PUSH_STAGE_SPEED_1      (40)   // ��Ӧ�����ֵ
#define PUSH_STAGE_SPEED_2      (60)   // ��Ӧ�ڶ���ֵ
#define PUSH_STAGE_SPEED_3      (50)   // ��Ӧ������ֵ
#define PUSH_STAGE_SPEED_4      (50)   // ��Ӧ�����ֵ

// ============================ ���������� ============================
/**
 * �ٶ�ƽ����غ���
 */
int16_t speed_smooth(uint8_t encoder_flag, int16_t encoder_val, SpeedDir mode);

#endif
#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h> 
#include "zf_driver_encoder.h"

// ============================ ������Ӳ�������� ============================
/**
 * ���������ö��
 * ����ͳһ��ʶ3�������������ԭ1/2/3���ֱ�ʶ
 */
typedef enum {
    ENCODER_NUM_1,  // ������1
    ENCODER_NUM_2,  // ������2
    ENCODER_NUM_3,  // ������3
	ENCODER_COUNT   // �������������Զ����㣬���ڲ���У�飩
} EncoderNum;

/**
 * ������Ӳ��ӳ���
 * ������������Ӧ�Ķ�ʱ��ͨ�������Ŷ��壬���й���Ӳ������
 */
#define ENCODER_1_TIMER       QTIMER1_ENCODER1  // ������1ʹ�õĶ�ʱ��
#define ENCODER_1_CH1_PIN     QTIMER1_ENCODER1_CH1_C0  // ������1ͨ��1����
#define ENCODER_1_CH2_PIN     QTIMER1_ENCODER1_CH2_C1  // ������1ͨ��2����

#define ENCODER_2_TIMER       QTIMER2_ENCODER1  // ������2ʹ�õĶ�ʱ��
#define ENCODER_2_CH1_PIN     QTIMER2_ENCODER1_CH1_C3   // ������2ͨ��1����
#define ENCODER_2_CH2_PIN     QTIMER2_ENCODER1_CH2_C25  // ������2ͨ��2����

#define ENCODER_3_TIMER       QTIMER1_ENCODER2  // ������3ʹ�õĶ�ʱ��
#define ENCODER_3_CH1_PIN     QTIMER1_ENCODER2_CH1_C2   // ������3ͨ��1����
#define ENCODER_3_CH2_PIN     QTIMER1_ENCODER2_CH2_C24  // ������3ͨ��2����

// ============================ ���������� ============================
/**
 * ��������ʼ����غ���
 */
void Encoder_Init(void);                  // ��ʼ�����б�����

/**
 * ���������ݶ�ȡ��غ���
 */
int32_t Encoder_ReadSpeed(EncoderNum num);  // ��ȡָ�����������ٶ�ֵ

#endif

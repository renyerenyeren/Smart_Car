#ifndef __GRAYSCALE_SENSOR_H__
#define __GRAYSCALE_SENSOR_H__

#include "zf_common_headfile.h"

// ============================ �궨���� ============================
/** �Ҷȴ��������Ŷ��� */
#define GRAY_SENSOR_PIN            C6              // �Ҷȴ������ź�����

// ============================ ȫ�ֱ��������� ============================
/** �Ҷȴ��������� */
extern uint8 Gray_level;                           // ��ǰ�Ҷȵȼ�ֵ

// ============================ ���������� ============================
/**
 * �Ҷȴ�������غ���
 */
void Gray_Init(void);
uint8 Gray_Get(void);

#endif
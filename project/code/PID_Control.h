#ifndef _PID_CONTROL_H_
#define _PID_CONTROL_H_

#include "zf_common_headfile.h" 

// ============================ �궨���� ============================
/** ���Ʋ����궨�� */
#define TARGET_ANGLE_X                 0           // Ŀ��X��Ƕ�
#define TARGET_POSITION_Y              135         // Ŀ��Y��λ��
#define TARGET_SLOPE                   0           // Ŀ��б��


/** �˶������궨�� */
#define DEFAULT_SPEED                  8000
#define SPEED_1M6                      9000
#define SPEED_1M7                      10000
#define SPEED_FASTER                   12000
#define DEFAULT_TURNING_RADIUS         4.2f        // Ĭ��ת��뾶

// ============================ ȫ�ֱ��������� ============================
/** ����ģʽ��״̬��־ */
extern bool SPEED_ENABLE;                          // �ٶȿ��ر�־λ
extern uint8_t Motor_Control_Mode;                 // ����ģʽ����

/** �˶����Ʋ��� */
extern int8 rotation_direction;                    // ������ת����-1����ʱ�룬1��˳ʱ�룩
extern int8 tangential_linear_speed;               // ��תʱ���������ٶ�

/** �ٶ���ز��� */
extern int16 tracking_speed;                       // ѭ���ٶȣ�y����
extern int16 last_speed;                           // ѭ��ģʽ�µ��ٶ�һ���˲�
extern int speed_compensation_offset;              // �ٶȲ���ƫ����

/** ������Ʋ��� */
extern int Read_wheelspeed[3];                     // ��ȡ�������ٶ�
extern int wheel[3];                               // ���������������Ӧ��Ŀ���ٶ�

// ============================ ���������� ============================
/**
 * �ͳ�
 */
void rushrushrush(void);

/**
 * ���������غ���
 */
void Motor_Control(void);
void pit_handler_0(void);

#endif


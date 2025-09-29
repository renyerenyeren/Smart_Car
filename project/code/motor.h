#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "zf_common_headfile.h" 

// ============================ ö�����Ͷ����� ============================
/**
 * @brief ������ö��
 */
typedef enum {
    MOTOR_NUM_1 = 0,  // ���1
    MOTOR_NUM_2,      // ���2
    MOTOR_NUM_3,      // ���3
    MOTOR_COUNT       // ����������Զ����㣬���ڲ���У�飩
} MotorNum;

// ============================ �궨���� ============================
/**
 * ���Ӳ�����ã��������ż��й����޸�ʱ��������˴���
 */
// ���1��PWM���� + ��������
#define MOTOR1_PWM_PIN    PWM4_MODULE3_CHA_C31
#define MOTOR1_DIR_PIN    C30
// ���2��PWM���� + ��������
#define MOTOR2_PWM_PIN    PWM1_MODULE3_CHB_D1
#define MOTOR2_DIR_PIN    D3
// ���3��PWM���� + ��������
#define MOTOR3_PWM_PIN    PWM1_MODULE3_CHA_D0
#define MOTOR3_DIR_PIN    D2

// ���ͨ�ò���
#define MOTOR_PWM_FREQ    17000UL  // PWMƵ�ʣ�17kHz��
#define MOTOR_INIT_DUTY   0UL      // ��ʼռ�ձȣ�0��
#define MOTOR_MAX_DUTY    7500     // ���ռ�ձ����ƣ���ֹ������

// ============================ ���������� ============================
/**
 * �����ʼ����غ���
 */
void motor_Init(void);  // ��ʼ�����е����PWM�ͷ�������

/**
 * ���������غ���
 */
void motor_set_duty(MotorNum motor_num, int duty);  // ����ָ�������ռ�ձ��뷽��

#endif
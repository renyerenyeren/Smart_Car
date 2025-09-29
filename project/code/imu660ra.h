#ifndef _IMU660RA_H_
#define _IMU660RA_H_

#include "zf_common_headfile.h" 

// ============================ �궨���� ============================
/** ��������ƯУ׼ʹ�ܿ��� */
#define IS_ZERO_BIAS                    1           // 0-�ر���ƯУ׼��1-������ƯУ׼

/** �����˲�ϵ����ƽ�������Ƕ�̬��Ӧ����ٶȼƾ�̬�ȶ��ԣ� */
#define COMPLEMENTARY_FILTER_K          0.1f

/** �������� */
#define ZERO_BIAS_SAMPLE_COUNT          500         // ��ƯУ׼��������
#define SAMPLE_INTERVAL_MS              5           // �����������λ��ms��

// ============================ �ṹ�嶨���� ============================
/**
  * @brief ������/���ٶȼƲ����ṹ��
  * @details �洢���ᣨX/Y/Z����ԭʼ���ݡ����ֽǶȻ��ںϽ������;������������
  */
typedef struct 
{
    float Xdata;  // X�����ݣ����ٶ�/�Ƕȣ����ϱ��������жϣ�
    float Ydata;  // Y�����ݣ����ٶ�/�Ƕȣ����ϱ��������жϣ�
    float Zdata;  // Z�����ݣ����ٶ�/�Ƕȣ����ϱ��������жϣ�
} gyro_param_t;

// ============================ ȫ�ֱ��������� ============================
/** ���������Ĳ��� */
extern gyro_param_t Gyro_Angle;         // �����ǻ��ּ����ʵʱ�Ƕ�

/** ת�����ר�ýǶȲ��� */
extern float angle_before_turn;         // ת������ǰ�ĳ�ʼ�Ƕȣ���λ���ȣ�
extern float angle_turn;                // ����ת���Ŀ��Ƕȣ���λ���ȣ�
extern float Angle_now;                 // ��ǰʵʱ�Ƕȣ���λ���ȣ�����ת��ջ����ƣ�

// ============================ ���������� ============================
/**
 * ��������ʼ����غ���
 */
void imu660ra_zeroBias(void);
void my_imu660ra_init(void);

/**
 * �жϴ�����
 */
void pit_handler_1(void);

#endif
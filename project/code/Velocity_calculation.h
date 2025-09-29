#ifndef _VELOCITY_CALCULATION_H_  
#define _VELOCITY_CALCULATION_H_

#include "zf_common_headfile.h" 

// ============================ ö�����Ͷ����� ============================
/**
 * @brief �˶�ģʽö��
 * ��Ӧ��ͬ���ٶȽ����߼�������С����ͬ�˶�����
 */
typedef enum {
    NORMAL,      // ����ģʽ��ȫ���˶������־������ٶȲ���
    TURN,        // ת��ģʽ������ת�򣬲�����Ȩ����ת��������
    CORRECT      // У��ģʽ����̬������΢������ʵ�ֶ���
} MotionMode;

// ============================ �궨���� ============================
// ��е����
#define L 1.0f                  // С���־�/�۳��������谴ʵ�ʻ�е�ṹ������
// ʱ�����������
#define IT_MS 3.0f              // �������������ڣ���λ��ms��
// У��ϵ��
#define CORRECTION 0.028f       // ƽ��λ�ƻ���У��ϵ�����������ִ�����
#define CACULATE_REVISE 1.0f    // ��λ����������ϵ��
// �Ƕȳ���
#define cos60 0.5f              // 60������ֵ���̶�������
#define sin60 0.8660254f        // 60������ֵ���̶�������
// ���ߺ�
#define RADtoDEG(rad) (rad * 180.0f / PI)  // ����ת�Ƕȣ�����zf_common_headfile.h�е�PI��

// ============================ ȫ�ֱ��������� ============================
/** �ٶȿ�����ز��������˶���������㺯�����ã� */
extern float V;                // �������ٶȴ�С
extern float theta;            // �ٶȷ���нǣ��ȣ�
extern float w;                // ������ת���ٶȣ�˳ʱ��Ϊ����
extern float Vx;               // X�᷽����ٶ�
extern float Vy;               // Y�᷽����ٶ�

/** λ�ƽ�����ز������������洢��ʹ�ܿ��ƣ� */
extern bool calculate_flag;    // λ�ƽ���ʹ�ܱ�־��true�����㣻false�����ã�
extern float shift_yaw;        // �ۼ�ƫ���Ƕȣ��ȣ�
extern float shift_distance;   // �ۼ���λ�ƣ��޵�λ��
extern float x_distance;       // X���ۼ�λ�ƣ��޵�λ��
extern float y_distance;       // Y���ۼ�λ�ƣ��޵�λ��

// ============================ ���������� ============================
/**
 * �ٶȽ�����غ���
 */
void calculateSpeeds(MotionMode mode, float V, float theta, float w, int* V1, int* V2, int* V3);
void calculate_angle_with_y_axis(float x, float y, float *V, float *theta);

/**
 * �˶�������غ���
 */
void turn_around(float R, int8_t direction, float want_Vx, float *Vx, float *w);
void move(int16_t angle, int8_t speed);

/**
 * λ�ƽ�����غ���
 */
void translate_shift(int motor_1_speed, int motor_2_speed, int motor_3_speed);

#endif
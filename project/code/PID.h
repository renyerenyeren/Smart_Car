#ifndef __PID_H_
#define __PID_H_

#include "zf_common_headfile.h" 

// ============================ ö�����Ͷ����� ============================
/**
  * @brief �ٶȻ�״̬ö��
  *        ��Ӧ��ͬ��PID�������ã���Ӧ��ͬ�˶���������
  */
typedef enum 
{
    Soft,      // ���Բ�������Ӧƽ����������ƽ���˶�
    Hard,      // Ӳ�Բ�������ӦѸ�٣������ڿ�����Ӧ
    Withy,     // ���Բ�����������Ӧ��������һ�㳡��
    Strong     // ǿ�Բ�����ǿ����Ӧ�������ڸ߸��س���
} VelocityState;

// ============================ �ṹ�嶨���� ============================
/**
  * @brief ��׼PID�ṹ��
  * @details ��P/I/D���֧�ֻ����޷�������޷�
  *          ���ó�����PositionX_Second_Correct��PositionY_Second_Correct�ȱ�׼PID����
  */
typedef struct 
{
    /* ���Ʋ��� */
    float Kp;                   // ����ϵ��
    float Ki;                   // ����ϵ��
    float Kd;                   // ΢��ϵ��
    /* ����״̬ */
    float last_err;             // ��һ�����
    float integral;             // �������ۼ�ֵ
    /* ���ò��� */
    float integral_limit;       // �����޷�
    float output_limit;         // ����޷�
} StdPID_HandleTypeDef;

/**
  * @brief ����Ӧ����PID�ṹ��
  * @details ��������С�л�Kp/Kd������֧��PD����
  *          ���ó�����AdaptiveRobust_PositionY_Correct������Ӧ��������
  */
typedef struct 
{
    /* ����Ӧ������ */
    float Kp_large;             // ������������ϵ��
    float Kd_large;             // ���������΢��ϵ��
    float Kp_small;             // С����������ϵ��
    float Kd_small;             // С�������΢��ϵ��
    /* ����״̬ */
    float last_err;             // ��һ�����
    /* ���ò��� */
    float err_threshold;        // �����ֵ�����ִ�С�������
    float output_limit;         // ����޷�
} AdaptivePID_HandleTypeDef;

/**
  * @brief ת��ר��PID�ṹ��
  * @details ֧�ֶ�̬P���Kp��̬����
  *          ���ó�����Turn������ת����Ƴ���
  */
typedef struct 
{
    /* �������Ʋ��� */
    float base_Kp;              // ��������ϵ��
    float Kd;                   // ΢��ϵ��
    /* ��̬P������ */
    float k_value;              // ��̬�����ϵ�������ٶ����ù�����
    float dynamic_term_limit;   // ��̬���޷�
    /* ����״̬ */
    float last_err;             // ��һ�����
    /* ���ò��� */
    float output_limit;         // ����޷�
} turnPID_HandleTypeDef;

/**
  * @brief �ٶȻ�ר��PID�ṹ��
  * @details ����ʽPID + ����ͨ�˲�
  *          ���ó�����Velocity�������ٶȿ��Ƴ���
  */
typedef struct 
{
    /* ���Ʋ��� */
    float Kp;                   // ����ϵ����ԭVelocity_Kp��
    float Ki;                   // ����ϵ����ԭVelocity_Ki��
    /* ����״̬ */
    float error_last;           // ��һ���˲������
    float error_lowout;         // ��ǰ�˲������
    float output;               // �ۼӺ�����ֵ
    /* ���ò��� */
    float filter_alpha;         // ��ͨ�˲�ϵ����aԽ���˲�Խǿ��
    float output_limit;         // ����޷�
} VelocityPID_HandleTypeDef;

/**
  * @brief �ٶ����ýṹ��
  * @details ��ת��PID��ҵ���������������ʵ��ҵ�񳡾�������Ա����
  */
typedef struct 
{
    float turn_kp;              // ת��PID����Kp
    float turn_kd;              // ת��PID Kd
    float k_value;              // ת��̬�����ϵ��
    float dynamic_term_limit;   // ת��̬���޷�
    float output_limit;         // ת������޷�
} SpeedConfigTypeDef;

// ============================ ȫ�ֱ��������� ============================
/** �ٶȻ�PID�����3���������Ӧԭcontrollers[0~2]�� */
extern VelocityPID_HandleTypeDef hvel_pid[3];

/** ת��ר��PID��� */
extern turnPID_HandleTypeDef hturn_pid;

/** ����ӦX��Ƕȿ���PID��� */
extern AdaptivePID_HandleTypeDef hadaptive_anglex_pid;

/** ����ӦY��λ��У��PID��� */
extern AdaptivePID_HandleTypeDef hadaptive_posy_pid;

/** �Ƕ�PID��� */
extern StdPID_HandleTypeDef hangle_pid;

/** Y�ᱣ��PID��� */
extern StdPID_HandleTypeDef hposy_keep_pid;

/** X�ᱣ��PID��� */
extern StdPID_HandleTypeDef hposx_keep_pid;

// ============================ ���������� ============================
/**
 * PID������غ���
 */
float StdPID_Calculate(StdPID_HandleTypeDef *hpid, float actual, float target);
float AdaptivePID_Calculate(AdaptivePID_HandleTypeDef *hpid, float actual, float target);
float turnPID_Calculate(turnPID_HandleTypeDef *hpid, float actual, float target);
int VelocityPID_Calculate(VelocityPID_HandleTypeDef *hpid, int actual_speed, int target_speed);

/**
 * ��ʼ�������ú���
 */
void PID_Init(void);
void change_Velocity(VelocityState x);

#endif /* __PID_H */
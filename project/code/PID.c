#include "PID.h"

// ============================ ȫ�ֱ��������� ============================
/** ȫ���ٶȻ�PID�����3������� */
VelocityPID_HandleTypeDef hvel_pid[3];

/** ת��ר��PID��� */
turnPID_HandleTypeDef hturn_pid;

/** ����ӦX��Ƕȿ���PID��� */
AdaptivePID_HandleTypeDef hadaptive_anglex_pid;

/** ����ӦY��λ��У��PID��� */
AdaptivePID_HandleTypeDef hadaptive_posy_pid;

/** �Ƕȱ���PID��� */
StdPID_HandleTypeDef hangle_pid;

/** Y�ᱣ��PID��� */
StdPID_HandleTypeDef hposy_keep_pid;

/** X�ᱣ��PID��� */
StdPID_HandleTypeDef hposx_keep_pid;

// ============================ ���������� ============================
/**
  * ��    ����StdPID_Calculate
  * ��    �ܣ���׼PID����ʵ��
  * ��    ����hpid   - ��׼PID�ṹ��ָ��
  * ��    ����actual - ʵ�ʲ���ֵ
  * ��    ����target - Ŀ��ֵ
  * �� �� ֵ��PID���ֵ�����޷���
  * ��    ע����P/I/D���֧�ֻ����޷�������޷�
  */
float StdPID_Calculate(StdPID_HandleTypeDef *hpid, float actual, float target) 
{
    if (hpid == NULL) return 0.0f;

    // 1. ���㵱ǰ��ʵ��ֵ - Ŀ��ֵ����ԭ�����߼�һ�£�
    float err = actual - target;

    // 2. ��������㣨���޷���
    hpid->integral += err;
    hpid->integral = func_limit(hpid->integral, hpid->integral_limit);

    // 3. ΢������㣨���仯����
    float derivative = err - hpid->last_err;

    // 4. PID�������
    float output = hpid->Kp * err + 
                   hpid->Ki * hpid->integral + 
                   hpid->Kd * derivative;

    // 5. ����޷�
    output = func_limit(output, hpid->output_limit);

    // 6. ������һ�����
    hpid->last_err = err;

    return output;
}

/**
  * ��    ����AdaptivePID_Calculate
  * ��    �ܣ�����Ӧ����PID����ʵ��
  * ��    ����hpid   - ����ӦPID�ṹ��ָ��
  * ��    ����actual - ʵ�ʲ���ֵ
  * ��    ����target - Ŀ��ֵ
  * �� �� ֵ��PID���ֵ�����޷���
  * ��    ע����������Сѡ������飬PD����
  */
float AdaptivePID_Calculate(AdaptivePID_HandleTypeDef *hpid, float actual, float target) 
{
    if (hpid == NULL) return 0.0f;

    // 1. ���㵱ǰ��ʵ��ֵ - Ŀ��ֵ����ԭ�����߼�һ�£�
    float err = actual - target;

    // 2. ��������Сѡ�������
    float current_Kp, current_Kd;
    if (fabs(err) > hpid->err_threshold) 
    {
        // ���������ǿ��Ӧ����
        current_Kp = hpid->Kp_large;
        current_Kd = hpid->Kd_large;
    } 
    else 
    {
        // С�����������Ӧ���������ٳ�����
        current_Kp = hpid->Kp_small;
        current_Kd = hpid->Kd_small;
    }

    // 3. ΢������㣨���仯����
    float derivative = err - hpid->last_err;

    // 4. PD�������
    float output = current_Kp * err + current_Kd * derivative;

    // 5. ����޷�
    output = func_limit(output, hpid->output_limit);

    // 6. ������һ�����
    hpid->last_err = err;

    return output;
}

/**
  * ��    ����turnPID_Calculate
  * ��    �ܣ�ת��ר��PID����ʵ��
  * ��    ����hpid   - ת��PID�ṹ��ָ��
  * ��    ����actual - ʵ�ʲ���ֵ
  * ��    ����target - Ŀ��ֵ
  * �� �� ֵ��PID���ֵ�����޷���
  * ��    ע������̬P�PD����
  */
float turnPID_Calculate(turnPID_HandleTypeDef *hpid, float actual, float target) 
{
    if (hpid == NULL) return 0.0f;

    float err = actual - target;
    float dynamic_term = err * err * hpid->k_value;
    dynamic_term = func_limit(dynamic_term, hpid->dynamic_term_limit);
    
    float current_Kp = hpid->base_Kp + dynamic_term;
    float derivative = err - hpid->last_err;
    float output = current_Kp * err + hpid->Kd * derivative;
    output = func_limit(output, hpid->output_limit);
    hpid->last_err = err;

    return output;
}

/**
  * ��    ����VelocityPID_Calculate
  * ��    �ܣ��ٶȻ�ר��PID����ʵ��
  * ��    ����hpid         - �ٶȻ�PID�ṹ��ָ��
  * ��    ����actual_speed - ʵ���ٶ�
  * ��    ����target_speed - Ŀ���ٶ�
  * �� �� ֵ��PID���ֵ�����޷���int���ͼ���ԭ�ӿڣ�
  * ��    ע������ʽPID + ����ͨ�˲�
  */
int VelocityPID_Calculate(VelocityPID_HandleTypeDef *hpid, int actual_speed, int target_speed)	
{
    if (hpid == NULL) return 0;

    int raw_err = actual_speed - target_speed;
    hpid->error_lowout = hpid->error_last * hpid->filter_alpha + raw_err * (1 - hpid->filter_alpha);
    int delta_err = hpid->error_lowout - hpid->error_last;
    int delta_out = (int)(hpid->Kp * delta_err + hpid->Ki * hpid->error_lowout);

    hpid->output += delta_out;
    hpid->output = func_limit(hpid->output, hpid->output_limit);
    hpid->error_last = hpid->error_lowout;

    return (int)hpid->output;
}

/**
  * ��    ����PID_Init
  * ��    �ܣ�PIDģ��ȫ�ֳ�ʼ��
  * ��    ������
  * �� �� ֵ����
  * ��    ע����ʼ������PID�������
  */
void PID_Init(void) 
{
    // �ٶȻ�PID��ʼ����3�������Ĭ��Soft״̬������
    for (uint8_t i = 0; i < 3; i++) 
    {
        hvel_pid[i].Kp = -45.0f;
        hvel_pid[i].Ki = -2.95f;
        hvel_pid[i].filter_alpha = 0.3f;   // ԭ�����˲�ϵ��
        hvel_pid[i].output_limit = 7500.0f;
        hvel_pid[i].error_last = 0.0f;
        hvel_pid[i].error_lowout = 0.0f;
        hvel_pid[i].output = 0.0f;
    }

    // ת��ר��PID��ʼ��
    hturn_pid.base_Kp = 0.285f;
    hturn_pid.Kd = 7.0f;
    hturn_pid.k_value = 0.0000225f;
    hturn_pid.dynamic_term_limit = 0.045f;
    hturn_pid.output_limit = 70.0f;
    hturn_pid.last_err = 0.0f;

    // ����ӦX��Ƕȿ���PID��ʼ��
    hadaptive_anglex_pid.Kp_large = 0.195f;  // Kp_min + (Kp_max - Kp_min) * 0.3f
    hadaptive_anglex_pid.Kd_large = 3.5f;    // Kd_max
    hadaptive_anglex_pid.Kp_small = 0.1f;    // Kp_min
    hadaptive_anglex_pid.Kd_small = 1.5f;    // Kd_min
    hadaptive_anglex_pid.err_threshold = 15.0f;
    hadaptive_anglex_pid.output_limit = 60.0f;
    hadaptive_anglex_pid.last_err = 0.0f;

    // ����ӦY��λ��У��PID��ʼ��
    hadaptive_posy_pid.Kp_large = -0.645f;
    hadaptive_posy_pid.Kd_large = -4.95f;
    hadaptive_posy_pid.Kp_small = -0.4f;
    hadaptive_posy_pid.Kd_small = -2.5f;
    hadaptive_posy_pid.err_threshold = 15.0f;
    hadaptive_posy_pid.output_limit = 30.0f;
    hadaptive_posy_pid.last_err = 0.0f;

	 // �Ƕȱ���PID��ʼ��
    hangle_pid.Kp = -0.55f;
    hangle_pid.Ki = 0.0f;
    hangle_pid.Kd = -0.25f;
    hangle_pid.integral_limit = 0.0f;
    hangle_pid.output_limit = 70.0f;
    hangle_pid.last_err = 0.0f;
    hangle_pid.integral = 0.0f;

    // Y�ᱣ��PID��ʼ��
    hposy_keep_pid.Kp = -0.75f;
    hposy_keep_pid.Ki = 0.0f;
    hposy_keep_pid.Kd = -0.5f;
    hposy_keep_pid.integral_limit = 0.0f;
    hposy_keep_pid.output_limit = 10.0f;
    hposy_keep_pid.last_err = 0.0f;
    hposy_keep_pid.integral = 0.0f;

    // X�ᱣ��PID��ʼ��
    hposx_keep_pid.Kp = 1.785f;
    hposx_keep_pid.Ki = 0.0f;
    hposx_keep_pid.Kd = 1.85f;
    hposx_keep_pid.integral_limit = 0.0f;
    hposx_keep_pid.output_limit = 5.0f;
    hposx_keep_pid.last_err = 0.0f;
    hposx_keep_pid.integral = 0.0f;
}

/**
  * ��    ����change_Velocity
  * ��    �ܣ��ı��ٶȻ�PID����
  * ��    ����x - �ٶȻ�״̬��Soft/Hard/Withy/Strong��
  * �� �� ֵ����
  * ��    ע������״̬�л���ͬ��PID��������
  */
void change_Velocity(VelocityState x) 
{
    float kp_val, ki_val;

    // ����״̬ѡ�����
    switch (x) 
    {
        case Soft:
            kp_val = -45.0f;
            ki_val = -2.95f;
            break;
        case Hard:
            kp_val = -60.0f;
            ki_val = -15.0f;
            break;
        case Withy:
            kp_val = -50.0f;
            ki_val = -5.95f;
            break;
        case Strong:
            kp_val = -65.0f;
            ki_val = -20.0f;
            break;
        default:
            kp_val = -45.0f;
            ki_val = -2.95f;
            break;
    }

    // ����3��������ٶȻ�����
    for (uint8_t i = 0; i < 3; i++) 
    {
        hvel_pid[i].Kp = kp_val;
        hvel_pid[i].Ki = ki_val;
    }
}
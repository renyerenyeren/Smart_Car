#include "motor.h"

// ============================ ��̬���ݶ����� ============================
/**
 * ���Ӳ�������ṹ������
 * ��ÿ�������PWM���š������������Ű󶨣�ͨ��ö������ֱ�ӷ���
 */
typedef struct {
    pwm_channel_enum pwm_pin;   // PWM�������
    gpio_pin_enum    dir_pin;   // �����������
} MotorHardware;

static const MotorHardware g_motor_hw[MOTOR_COUNT] = {
    {MOTOR1_PWM_PIN, MOTOR1_DIR_PIN},  // ���1����PWM�ͷ�������
    {MOTOR2_PWM_PIN, MOTOR2_DIR_PIN},  // ���2����PWM�ͷ�������
    {MOTOR3_PWM_PIN, MOTOR3_DIR_PIN}   // ���3����PWM�ͷ�������
};

// ============================ �����ʼ������ ============================
/**
  * ��    ����motor_Init
  * ��    �ܣ���ʼ�����е����PWMͨ���ͷ����������
  * ��    ������
  * �� �� ֵ����
  * ��    ע��1. ��������˳���ʼ����ͳһʹ��MOTOR_PWM_FREQƵ�ʣ�
  *           2. ��������Ĭ�ϳ�ʼΪ�ߵ�ƽ������Ӳ��ת��ƥ�䣩
  */
void motor_Init(void)
{
    // ��ʼ�����1��PWM + ��������
    pwm_init(g_motor_hw[MOTOR_NUM_1].pwm_pin, MOTOR_PWM_FREQ, MOTOR_INIT_DUTY);
    gpio_init(g_motor_hw[MOTOR_NUM_1].dir_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    
    // ��ʼ�����2��PWM + ��������
    pwm_init(g_motor_hw[MOTOR_NUM_2].pwm_pin, MOTOR_PWM_FREQ, MOTOR_INIT_DUTY);
    gpio_init(g_motor_hw[MOTOR_NUM_2].dir_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    
    // ��ʼ�����3��PWM + ��������
    pwm_init(g_motor_hw[MOTOR_NUM_3].pwm_pin, MOTOR_PWM_FREQ, MOTOR_INIT_DUTY);
    gpio_init(g_motor_hw[MOTOR_NUM_3].dir_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

// ============================ ���ռ�ձȿ��ƺ��� ============================
/**
  * ��    ����motor_set_duty
  * ��    �ܣ�����ָ�������ռ�ձȣ���������ƣ�
  * ��    ����motor_num - �����ţ�MOTOR_NUM_1~MOTOR_NUM_3��
  * ��    ����duty      - ռ�ձȣ�������ת��������ת����Χ[-MOTOR_MAX_DUTY, MOTOR_MAX_DUTY]��
  * �� �� ֵ����
  * ��    ע��1. �Զ�����ռ�ձ������Χ�ڣ���ֹ������أ�
  *           2. �Ƿ�������ֱ���˳�����ǿ�ݴ���
  */
void motor_set_duty(MotorNum motor_num, int duty)
{
    // �����Ϸ���У�飺�����ų�����Χ���˳�
    if (motor_num >= MOTOR_COUNT) 
	{
        return;
    }

    // ռ�ձ��޷�����ֹ�������������ֵ
    duty = func_limit(duty, MOTOR_MAX_DUTY);

    // ������ƣ�����ռ�ձ��������÷������ŵ�ƽ
    if (duty > 0)	{gpio_set_level(g_motor_hw[motor_num].dir_pin, GPIO_LOW);}
    else 			{gpio_set_level(g_motor_hw[motor_num].dir_pin, GPIO_HIGH); duty = -duty;}
    // ���PWMռ�ձ�
    pwm_set_duty(g_motor_hw[motor_num].pwm_pin, (uint32_t)duty);
}
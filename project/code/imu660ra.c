#include "imu660ra.h"

// ============================ ȫ�ֱ��������� ============================
/** ���������Ĳ�����ʼ������ʽ��0��������������ֵ�� */
gyro_param_t Gyro_Bias = {0.0f, 0.0f, 0.0f};    // ��ƯУ׼ֵ��ʼ��Ϊ0
gyro_param_t Gyro_Angle = {0.0f, 0.0f, 0.0f};   // �����ǻ��ֽǶȳ�ʼ��Ϊ0
gyro_param_t Acc_Angle = {0.0f, 0.0f, 0.0f};    // ���ٶȼƽǶȳ�ʼ��Ϊ0
gyro_param_t Fusion_Angle = {0.0f, 0.0f, 0.0f}; // �ںϽǶȳ�ʼ��Ϊ0

/** ������ת�������ݳ�ʼ�� */
float tra_acc_x = 0.0f;
float tra_acc_y = 0.0f;
float tra_acc_z = 0.0f;
float tra_gyro_x = 0.0f;
float tra_gyro_y = 0.0f;
float tra_gyro_z = 0.0f;

/** ת����ƽǶȳ�ʼ�� */
float angle_before_turn = 0.0f;  // ��ʼ�Ƕ�Ĭ��0��
float angle_turn = 0.0f;         // Ŀ��ת��Ƕ�Ĭ��0��
float Angle_now = 0.0f;          // ��ǰ�Ƕ�Ĭ��0��

// ============================ ���������� ============================
/**
  * ��    ����imu660ra_zeroBias
  * ��    �ܣ���������ƯУ׼������Z����Ư����ֵ
  * ��    ������
  * �� �� ֵ����
  * ��    ע��1. ����IS_ZERO_BIAS�궨��Ϊ��ʱִ��У׼��
  *           2. ͨ��500�β���ȡƽ����������Ӱ�죻
  *           3. �������5ms����У׼ʱ��Լ2.5��
  */
void imu660ra_zeroBias(void)
{
#if IS_ZERO_BIAS  // �򻯺��ж�
    Gyro_Bias.Zdata = 0.0f;  // ��ʼ��Z����Ư�ۼ���
    // ��βɼ��������������ȡƽ����Ϊ��Ư����ֵ
    for (uint16_t i = 0; i < 500; i++)
    {
        imu660ra_get_gyro();  // ��ȡ������ԭʼ�Ĵ�������
        // �ۼ�ת�����Z����ٶȣ���0.005����Ӳ���ĵ�ȷ�Ϻ��壬������ʱϵ����
        Gyro_Bias.Zdata += (imu660ra_gyro_transition(imu660ra_gyro_z) * 0.005f);
        system_delay_ms(5);   // �������5ms�����ж�����ƥ��
    }
    Gyro_Bias.Zdata /= 500.0f;  // ��ƽ��ֵ���õ�������ƯУ׼ֵ
#endif
}

/**
  * ��    ����my_imu660ra_init
  * ��    �ܣ���ʼ��IMU660RA����������ز���
  * ��    ������
  * �� �� ֵ����
  * ��    ע��1. �ȵ��ùٷ���ʼ�������������ýǶ���ز�����
  *           2. ��ʽ�����нǶȲ�����0��ȷ����ʼ״̬һ�£�
  *           3. ����ϵͳ����ʱ����һ��
  */
void my_imu660ra_init(void)
{
    imu660ra_init();
    
    Gyro_Angle.Zdata = 0.0f;
    Gyro_Angle.Ydata = 0.0f;
    Gyro_Bias.Zdata = 0.0f;
    Acc_Angle.Xdata = 0.0f;
    Acc_Angle.Ydata = 0.0f;
    Acc_Angle.Zdata = 0.0f;
    Fusion_Angle.Ydata = 0.0f;
}

/**
  * ��    ����pit_handler_1
  * ��    �ܣ���ʱ���жϴ�������ʵʱ���������ǽǶ�
  * ��    ������
  * �� �� ֵ����
  * ��    ע��1. ��������Ϊ5ms���ڣ�������������һ�£�
  *           2. �Ƕȼ��㹫ʽ���Ƕ� = ���ٶȡ�ʱ�䣨ʱ�䵥λ���룩��
  *           3. 0.005f��Ӧ5ms�ж����ڣ�����Ӳ����ʱ������ƥ�䣻
  *           4. ������Ư��������߽Ƕȼ��㾫��
  */
void pit_handler_1(void)
{
    // ��ȡ��ת�����������ݣ�ԭʼֵ����/�룩
    imu660ra_get_gyro();
    tra_gyro_z = imu660ra_gyro_transition(imu660ra_gyro_z);
    
    // �����ǽǶȻ��֣��Ƕ� = ���ٶȡ�ʱ�䣬ʱ�䵥λ���룩
    // ע��0.005f��Ӧ5ms�ж����ڣ�����Ӳ����ʱ�����ñ���һ��
    Gyro_Angle.Zdata += (tra_gyro_z*0.005-Gyro_Bias.Zdata);
    
    // ���µ�ǰ�Ƕȣ�����ʹ���ںϽǶȣ��˴�����ԭ�߼���
    Angle_now = Gyro_Angle.Zdata;
}

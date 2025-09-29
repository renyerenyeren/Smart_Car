#include "PID_Control.h" 

// ============================ ȫ�ֱ��������� ============================
/** ����ģʽ��״̬��־��ʼ�� */
bool SPEED_ENABLE = false;                         // �ٶȿ��ر�־λ
float Target_slope = 0;                            // Ŀ��б��

/** �ٶ���ز�����ʼ�� */
int16 Speed_factory = DEFAULT_SPEED;
int16 tracking_speed;                              // ѭ���ٶȣ�y����
int16 last_speed;                                  // ѭ��ģʽ�µ��ٶ�һ���˲�
int speed_compensation_offset = 10;                // �ٶȲ���ƫ���������ڲ�����еĦ���ȣ�

/** �˶����Ʋ�����ʼ�� */
int8 rotation_direction;                           // ������ת����-1����ʱ�룬1��˳ʱ�룩
int8 tangential_linear_speed;                      // ��תʱ���������ٶ�

/** ������Ʋ�����ʼ�� */
int Read_wheelspeed[3] = {0, 0, 0};                // ��ȡ�������ٶ�
int wheel[3] = {0};                                // ���������������Ӧ��Ŀ���ٶ�
uint8_t Motor_Control_Mode;                        // ����ģʽ����

// ============================ ���������� ============================
// ��̳壡������������
void rushrushrush(void)
{
	Speed_factory = SPEED_FASTER;
}	



/**
  * ��    ����Motor_Control
  * ��    �ܣ�������������������ݲ�ͬ����ģʽ���������
  * ��    ������
  * �� �� ֵ����
  * ��    ע��1. ֧�ֶ��ֿ���ģʽ��ѭ����ת��λ��У���ȣ�
  *           2. ����ͨ���ٶȻ�PID���Ƶ�������
  *           3. ����SPEED_ENABLE��־λ�����Ƿ�ʵ�����
  */
void Motor_Control(void)
{
    static int Last_Now_Angle_X;
    
    // ���ݿ���ģʽ���������ٶȺͽ��ٶ�
    switch (Motor_Control_Mode)
    {
        case 0:  // ����ѭ��ģʽ
            w = turnPID_Calculate(&hturn_pid, Slope, Target_slope);
            break;
            
        case 1:  // �ȴ�ģʽ
            w = 0;
            move(0, 0);
            break;
            
        case 2:  // ���Ƴ���Ŀ��Ƕ�
            w = StdPID_Calculate(&hangle_pid, Angle_now, angle_before_turn + angle_turn);
            break;
            
        case 9:  // ��ת�˶�ģʽ
            Vy = 0;
            turn_around(DEFAULT_TURNING_RADIUS, rotation_direction, tangential_linear_speed, &Vx, &w);
            calculate_angle_with_y_axis(Vx, Vy, &V, &theta);  // ����ת��
            break;
            
        case 16: // λ�ñ���ģʽ��������Լ����
            if (Now_Position_Y)
            {
                Vy = StdPID_Calculate(&hposy_keep_pid, Now_Position_Y, TARGET_POSITION_Y);
                turn_around(DEFAULT_TURNING_RADIUS, rotation_direction, tangential_linear_speed, &Vx, &w);
                Vx += StdPID_Calculate(&hposx_keep_pid, Now_Angle_X, TARGET_ANGLE_X);  // ����Լ��
            }
            else
            {
                Vy = 0;
                turn_around(DEFAULT_TURNING_RADIUS, rotation_direction, tangential_linear_speed, &Vx, &w);
            }
            calculate_angle_with_y_axis(Vx, Vy, &V, &theta);  // ����ת��
            break;
            
        case 12: // ����λ�ý���ģʽ��������룩
            Vx = StdPID_Calculate(&hposx_keep_pid, Now_Angle_X, TARGET_ANGLE_X);  // ����Լ��
            Vy = 0;
            calculate_angle_with_y_axis(Vx, Vy, &V, &theta);  // ����ת��
            w = StdPID_Calculate(&hangle_pid, Angle_now, angle_before_turn + angle_turn);
            break;
            
        case 14: // ����Ӧλ��У��ģʽ
            if (Now_Position_Y)
            {
                V = AdaptivePID_Calculate(&hadaptive_posy_pid, Now_Position_Y, TARGET_POSITION_Y);
                w = AdaptivePID_Calculate(&hadaptive_anglex_pid, Now_Angle_X, TARGET_ANGLE_X);  // ���������
                Last_Now_Angle_X = Now_Angle_X;
            }
            else
            {
                V = last_speed * 0.5f;
                w = AdaptivePID_Calculate(&hadaptive_anglex_pid, Now_Angle_X, TARGET_ANGLE_X) * 1.5;
            }
            theta = 0;
            break;
            
        case 15: // ����ѭ��ģʽ
            w = turnPID_Calculate(&hturn_pid, Slope, Target_slope);
            break;
    }
    
    // ���ݿ���ģʽ�������Ŀ���ٶ�
    switch (Motor_Control_Mode)
    {
        case 0:  // ��ͨѭ��ģʽ
        {
            float tracking_speed = Speed_factory / (100.0f + fabsf(Slope));
            w *= tracking_speed / 32.5f;
            
            V = 0.7 * tracking_speed + 0.3 * last_speed + speed_compensation_offset;
            last_speed = V;
            theta = 0;
            calculateSpeeds(TURN, V, theta, w, &wheel[0], &wheel[1], &wheel[2]);
            break;
        }
        
        case 14: // ����ӦУ��ģʽ
        {
            calculateSpeeds(CORRECT, V, theta, w, &wheel[0], &wheel[1], &wheel[2]);
            break;
        }
        
        case 15: // ����ѭ��ģʽ
        {
            tracking_speed = 4500 / (100 + abs(Slope));
            w *= (float)tracking_speed / 25;
            
            V = 0.7 * tracking_speed + 0.3 * last_speed;
            last_speed = V;
            theta = 0;
            calculateSpeeds(TURN, V, theta, w, &wheel[0], &wheel[1], &wheel[2]);
            break;
        }
        
        default: // ����ȫ���˶�ģʽ
        {
            calculateSpeeds(NORMAL, V, theta, w, &wheel[0], &wheel[1], &wheel[2]);
        }
    }
    
    // �����ٶ�ʹ�ܱ�־���Ƶ��ʵ�����
    if (SPEED_ENABLE)
    {
        motor_set_duty(MOTOR_NUM_1, VelocityPID_Calculate(&hvel_pid[0], Read_wheelspeed[0], wheel[0]));
        motor_set_duty(MOTOR_NUM_2, VelocityPID_Calculate(&hvel_pid[1], Read_wheelspeed[1], wheel[1]));
        motor_set_duty(MOTOR_NUM_3, VelocityPID_Calculate(&hvel_pid[2], Read_wheelspeed[2], wheel[2]));
    }
}

/**
  * ��    ����pit_handler_0
  * ��    �ܣ�PIT��ʱ���жϴ�����
  * ��    ������
  * �� �� ֵ����
  * ��    ע��1. ��ȡ�������ٶȣ�
  *           2. ����λ�ƽ��㣻
  *           3. ��ȡ�Ҷȴ��������ݣ�
  *           4. ִ�е������
  */
void pit_handler_0(void)
{
    // ��ȡ���ֱ������ٶ�
    Read_wheelspeed[0] = Encoder_ReadSpeed(ENCODER_NUM_1);
    Read_wheelspeed[1] = Encoder_ReadSpeed(ENCODER_NUM_2);
    Read_wheelspeed[2] = Encoder_ReadSpeed(ENCODER_NUM_3);
    
    // λ�ƽ���
    translate_shift(Read_wheelspeed[0], Read_wheelspeed[1], Read_wheelspeed[2]);
    
    // ��ȡ�Ҷȴ���������
    Gray_level = Gray_Get();
    
    // ִ�е������
    Motor_Control();
}
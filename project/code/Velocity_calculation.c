#include "Velocity_calculation.h"

// ============================ ȫ�ֱ��������� ============================
/** �ٶȿ�����ز�����ʼ�� */
float V = 0.0f;                // �������ٶȴ�С��ʼ��Ϊ0
float theta = 0.0f;            // �ٶȷ���нǳ�ʼ��Ϊ0��
float w = 0.0f;                // ������ת���ٶȳ�ʼ��Ϊ0
float Vx = 0.0f;               // X����ٶȳ�ʼ��Ϊ0     //###ʹ�ú�ǵ�ת��ΪV��theta###
float Vy = 0.0f;               // Y����ٶȳ�ʼ��Ϊ0

/** λ�ƽ�����ز�����ʼ�� */
bool calculate_flag = false;   // Ĭ�Ϲر�λ�ƽ��㣨����״̬��
float shift_yaw = 0.0f;        // �ۼ�ƫ���Ƕȳ�ʼ��Ϊ0��
float shift_distance = 0.0f;   // �ۼ���λ�Ƴ�ʼ��Ϊ0
float x_distance = 0.0f;       // X���ۼ�λ�Ƴ�ʼ��Ϊ0
float y_distance = 0.0f;       // Y���ۼ�λ�Ƴ�ʼ��Ϊ0

// ============================ �ٶȽ�����غ��� ============================
/**
  * ��    ����calculateSpeeds
  * ��    �ܣ������˶�ģʽ���������ٶ�
  * ��    ����mode    - �˶�ģʽ��NORMAL/TURN/CORRECT��
  * ��    ����V       - �������ٶȴ�С���޵�λ������������ƥ�䣩
  * ��    ����theta   - �ٶȷ���нǣ���λ���ȣ����׼����ļнǣ�
  * ��    ����w       - �������ٶȣ��޵�λ��ת��ģʽ����Ч��
  * ��    ����V1      - ��1�ٶȣ���������������͵������ֵ��
  * ��    ����V2      - ��2�ٶȣ���������������͵������ֵ��
  * ��    ����V3      - ��3�ٶȣ���������������͵������ֵ��
  * �� �� ֵ����
  * ��    ע��1. ת��/У��ģʽ��V3�������ٶȲ���������ģʽ��������
  *           2. �����ٶ�ֵ����������ǿ��ת��Ϊ����
  */
void calculateSpeeds(MotionMode mode, float V, float theta, float w, int* V1, int* V2, int* V3) 
{
    float L_parameter = 1.0f;  // ��1���ٶȼ�Ȩ������Ĭ��1.0���޼�Ȩ��
    float R_parameter = 1.0f;  // ��2���ٶȼ�Ȩ������Ĭ��1.0���޼�Ȩ��
    
    // �����˶�ģʽ���ü�Ȩ����
    switch (mode) 
    {
        case TURN:
            if (w > 0)  { L_parameter = 0.6f; R_parameter = 1.4f; } 
            else        { R_parameter = 0.6f; L_parameter = 1.4f; }
            break;
            
        case CORRECT:
            if (w > 0)  { L_parameter = 0.85f; R_parameter = 1.15f; } 
            else        { R_parameter = 0.85f; L_parameter = 1.15f; }
            break;
            
        case NORMAL:
        default:
            L_parameter = 1.0f;
            R_parameter = 1.0f;
            break;
    }
    
    // ��������ٶȣ����������+���ٶȲ�����
    *V1 = (int)((2.0f / 3.0f) * cos((30.0f - theta) * PI / 180.0f) * V + L_parameter * L * w);
    *V2 = (int)((2.0f / 3.0f) * cos((150.0f - theta) * PI / 180.0f) * V + R_parameter * L * w);
    // V3������ģʽ�����ٶȲ���
    *V3 = (int)((2.0f / 3.0f) * cos((270.0f - theta) * PI / 180.0f) * V + (mode == NORMAL ? L * w : 0.0f));
}

/**
  * ��    ����calculate_angle_with_y_axis
  * ��    �ܣ���x/y�ٶȷ���ת��Ϊ���ٶȼ���y��н�
  * ��    ����x      - x�����ٶȷ������޵�λ���������ٶ�ƥ�䣩
  * ��    ����y      - y�����ٶȷ������޵�λ���������ٶ�ƥ�䣩
  * ��    ����V      - ���ٶȴ�С������������޵�λ��
  * ��    ����theta  - ��y������нǣ������������λ���ȣ���Χ[-180, 180]��
  * �� �� ֵ����
  * ��    ע��1. �Ƕȶ��壺˳ʱ��Ϊ����y������Ϊ0�ȣ�
  *           2. ��x��y��Ϊ0ʱ��thetaĬ�Ϸ���0��ԭ������Ч����
  */
void calculate_angle_with_y_axis(float x, float y, float *V, float *theta) 
{
    *V = (float)sqrt(x*x + y*y);
    
    if (x == 0 && y == 0) 
    {
        *theta = 0.0f;  // ԭ�����⴦������Ĭ�ϽǶ�0
        return;
    }

    // 1. ����x������Ϊ��׼�Ļ��Ƚ�
    double angle_rad = atan2(y, x);
    // 2. ת��Ϊy������Ϊ��׼��˳ʱ��Ϊ��
    double angle_rad_adjusted = - (angle_rad - PI / 2.0);
    // 3. ����ת�ǶȲ���һ����[-180, 180]
    double angle_deg = angle_rad_adjusted * (180.0 / PI);
    angle_deg = (angle_deg > 180.0f) ? (angle_deg - 360.0f) : (angle_deg < -180.0f ? angle_deg + 360.0f : angle_deg);

    *theta = (float)angle_deg;
}

// ============================ �˶�������غ��� ============================
/**
  * ��    ����turn_around
  * ��    �ܣ�����С����ת�˶�����
  * ��    ����R        - ��ת�뾶���޵�λ�������е�ṹƥ�䣩
  * ��    ����direction - ��ת����-1����ʱ�룻1��˳ʱ�룩
  * ��    ����want_Vx   - �������ٶȣ��޵�λ����תʱ�������ٶȣ�
  * �� �� ֵ����
  * ��    ע��1. �������洢��ȫ�ֱ���Vx�����ٶȣ���w�����ٶȣ���
  *           2. ����ӳ�䣺�����ơ���ʱ�루-1���������ơ�˳ʱ�루1��
  */
void turn_around(float R, int8_t direction, float want_Vx, float *Vx, float *w)
{     
    *Vx = want_Vx * direction * (-1.0f);
    // ������ٶȣ�v = ��R �� �� = v/R��
    *w = (want_Vx / R) * direction;
}

/**
  * ��    ����move
  * ��    �ܣ�����С��ֱ���ƶ�����
  * ��    ����angle  - �ƶ�����нǣ���λ���ȣ����׼����ļнǣ�
  * ��    ����speed  - �ƶ��ٶȴ�С���޵�λ������������ƥ�䣩
  * �� �� ֵ����
  * ��    ע�������洢��ȫ�ֱ���theta���Ƕȣ���V���ٶȣ������ٶȽ��㺯������
  */
void move(int16_t angle, int8_t speed)
{
    theta = (float)angle;
    V = (float)speed;
}

// ============================ λ�ƽ�����غ��� ============================
/**
  * ��    ����translate_shift
  * ��    �ܣ�����ƽ��λ�ơ�ƫ���Ƕȼ���λ��
  * ��    ����motor_1_speed - ���1�ٶȣ��������������޵�λ��
  * ��    ����motor_2_speed - ���2�ٶȣ��������������޵�λ��
  * ��    ����motor_3_speed - ���3�ٶȣ��������������޵�λ��
  * �� �� ֵ����
  * ��    ע��1. ����ȫ�ֱ�־λcalculate_flag��true-���ּ��㣬false-���ã�
  *           2. ���ֹ�ʽ��λ�� = �ٶ� �� ʱ�� �� У��ϵ��
  */
void translate_shift(int motor_1_speed, int motor_2_speed, int motor_3_speed)
{
    if (calculate_flag)
    {
        // 1. ����x��y�����ٶȷ��������ڵ���ٶȺϳɣ�
        float x_speed = (motor_1_speed * cos60 + motor_2_speed * cos60 - motor_3_speed);
        float y_speed = (motor_1_speed * sin60 - motor_2_speed * sin60);
        
        // 2. λ�ƻ��֣�����ʱ���У��ϵ����������������ת��Ϊλ�ƣ�
        x_distance += x_speed * IT_MS * CORRECTION;
        y_distance += y_speed * IT_MS * CORRECTION;
        
        // 3. ����ƫ���Ƕȣ�����x/yλ�Ƶķ����У�
        shift_yaw = RADtoDEG(atan(x_distance / y_distance));
        // 4. ����������atanĬ�Ϸ�Χ[-90,90]����չ��ȫ���ޣ�
        if (x_distance > 0 && y_distance < 0)
        {
            shift_yaw += 180.0f;
        }
        else if (x_distance < 0 && y_distance < 0)
        {
            shift_yaw -= 180.0f;
        }
        
        // 5. ������λ�ƣ�ŷ�Ͼ���+����������
        shift_distance = sqrt(x_distance * x_distance + y_distance * y_distance) * CACULATE_REVISE;
    }
    else
    {
        // ��������λ������
        x_distance = 0.0f;
        y_distance = 0.0f;
        shift_yaw = 0.0f;
        shift_distance = 0.0f;
    }
}
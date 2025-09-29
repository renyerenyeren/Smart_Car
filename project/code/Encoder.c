#include "Encoder.h"

// ============================ ��̬���������� ============================
// ��������ʱ��������飬��EncoderNumö�ٶ�Ӧ������������
static const encoder_index_enum g_encoderTimers[] = {
    ENCODER_1_TIMER,
    ENCODER_2_TIMER,
    ENCODER_3_TIMER
};

// ============================ ��������ʼ������ ============================
/**
  * ��    ����Encoder_Init
  * ��    �ܣ���ʼ�����б�����Ӳ�������ö�ʱ�������ţ�
  * ��    ������
  * �� �� ֵ����
  */
void Encoder_Init(void)
{
    // ��ʼ��������
    encoder_quad_init(ENCODER_1_TIMER, ENCODER_1_CH1_PIN, ENCODER_1_CH2_PIN);
    encoder_quad_init(ENCODER_2_TIMER, ENCODER_2_CH1_PIN, ENCODER_2_CH2_PIN);
    encoder_quad_init(ENCODER_3_TIMER, ENCODER_3_CH1_PIN, ENCODER_3_CH2_PIN);
}

// ============================ �������ٶȶ�ȡ���� ============================
/**
  * ��    ����Encoder_ReadSpeed
  * ��    �ܣ���ȡָ�����������ٶ�ֵ�������ţ���ӳ����
  * ��    ����num - ��������ţ�ENCODER_NUM_1 ~ ENCODER_NUM_3��
  * �� �� ֵ��int32_t - �ٶ�ֵ������2��Ƶ�������ű�ʾ����ת��
  * ��    ע��1. ��ȡ��������ǰ����ֵ��Ϊ�´β�����׼����
  *           2. �Ƿ���ŷ���0����ǿ�ݴ���
  */
int32_t Encoder_ReadSpeed(EncoderNum num)
{
    // ����źϷ���
    if (num >= ENCODER_COUNT) 
	{
        return 0;
    }
    // ��ȡ������ת��Ϊ�ٶ�ֵ
    int32_t speed = -encoder_get_count(g_encoderTimers[num]);
    // �������
    encoder_clear_count(g_encoderTimers[num]);
    
    return speed;
}

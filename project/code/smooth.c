#include "smooth.h"

// ============================ ���Ĺ��ܺ����� ============================
/**
  * ��    ����speed_smooth
  * ��    �ܣ����ݱ�����ֵ���˶�ģʽ����ƽ���ٶ�
  * ��    ����encoder_flag - ��������Ч��־��0����Ч��1����Ч��
  * ��    ����encoder_val  - ������Y��ֵ������ƥ���ٶ���ֵ��
  * ��    ����mode         - �˶�ģʽ��SPEED_MODE_BACK/SPPED_MODE_PUSH��
  * �� �� ֵ��int16_t - ƽ���������ٶ�ֵ����Чʱ����0��
  * ��    ע��1. ���ö��ֲ���ƥ����ֵ��������ѯЧ�ʣ�
  *           2. ��ֵ�谴��С�������򣬷�����ֲ����߼�ʧЧ��
  *           3. ģʽ�����Ƿ����������Чʱ������0��Ϊ��ȫֵ
  */
int16_t speed_smooth(uint8_t encoder_flag, int16_t encoder_val, SpeedDir mode)
{
    // �����˳�����������Чֱ�ӷ���0
    if (!encoder_flag) 
    {
        return 0;
    }

    // ģʽ�Ϸ���У�飺�Ƿ�ģʽ����0
    if (mode > SPEED_MODE_PUSH) 
    {
        return 0;
    }

    // ��ֵ���ٶȲ�������ͷ�ļ��궨��һһ��Ӧ����ģʽ������
    static const int16_t thresholds[2][4] = {
        {BACK_ENC_THRESHOLD_1, BACK_ENC_THRESHOLD_2, BACK_ENC_THRESHOLD_3, BACK_ENC_THRESHOLD_4},
        {PUSH_ENC_THRESHOLD_1, PUSH_ENC_THRESHOLD_2, PUSH_ENC_THRESHOLD_3, PUSH_ENC_THRESHOLD_4}
    };
    static const int16_t speeds[2][4] = {
        {BACK_STAGE_SPEED_1, BACK_STAGE_SPEED_2, BACK_STAGE_SPEED_3, BACK_STAGE_SPEED_4},
        {PUSH_STAGE_SPEED_1, PUSH_STAGE_SPEED_2, PUSH_STAGE_SPEED_3, PUSH_STAGE_SPEED_4}
    };
    static const uint8_t THRESHOLD_CNT = 4;  // ÿ��ģʽ����ֵ�������̶�Ϊ4����

    // ���ֲ��ң�ƥ�������ֵ��Ӧ����ֵ����
    uint8_t low = 0, high = THRESHOLD_CNT - 1, mid;
    while (low <= high) 
    {
        mid = (low + high) / 2;
        if (encoder_val > thresholds[mode][mid]) 
        {
            // ������ֵ���ڵ�ǰ��ֵ�����Ҹ�����ֵ����
            high = mid - 1;
        } 
        else 
        {
            // ������ֵС�ڵ��ڵ�ǰ��ֵ�����Ҹ�����ֵ����
            low = mid + 1;
        }
    }

    // ����ƥ����ٶ�ֵ��lowΪƥ��������������Χʱ�������һ���ٶȣ�
    return (low < THRESHOLD_CNT) ? speeds[mode][low] : speeds[mode][THRESHOLD_CNT - 1];
}
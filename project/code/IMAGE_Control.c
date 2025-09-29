#include "IMAGE_Control.h"

// ============================ ȫ�ֱ������� ============================
int16 Slope = 0;                            // ͼ��б�ʣ�����·�������ж�
uint8 Image_Mode = 0;                       // ͼ����ģʽѡ��
int Threshold;                              // ͼ���ֵ����ֵ
float Symmetry_Ratio = 0.0f;                // ͼ�����ҶԳ��ʣ����������ӵȳ���

// ============================ ����ʵ�� ============================
/**
 * ��    ����Image_Control
 * ��    �ܣ����ݵ�ǰͼ��ģʽִ����Ӧ��ͼ�����߼�
 * ��    ������
 * �� �� ֵ����
 * ��    ע��֧�ֶ���ͼ����ģʽ������Image_Mode�л�����
 */
void Image_Control(void)
{
    switch (Image_Mode)
    {
        case 0:  // ��׼ѭ��ģʽ������ʮ�ֺͻ�����⣩
            find_middle();         // ����·������
            Island_Detect();       // ��⻷��������
            Cross_Detect();        // ���ʮ�ֲ�����
            Slope = slope(Slope_Mode);  // ����·��б��
            break;
            
        case 3:  // ��ѭ��ģʽ����ѭ���������ʮ�ֺͻ�����
            find_middle();         // ����·������
            Slope = slope(Slope_Mode);  // ����·��б��
            break;
            
        case 4:  // �ȴ�ģʽ����ִ���κ�ͼ����
            break;
            
        case 7:  // �Գ��ʼ���ģʽ�����������ӵ���Ҫ�ԳƲ����ĳ�����
            // ����Ӧ�����ֵ����ֵ
            Threshold = my_adapt_threshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
            // ִ��ͼ���ֵ������
            Image_Binarization(Threshold);
            // ����ָ������ļ�Ȩ�Գ���
            Symmetry_Ratio = Get_Weighted_Symmetry(0, 45, MT9V03X_W, 75);
            break;
    }
}

/**
 * ��    ����pit_handler_2
 * ��    �ܣ��������жϴ�����������ͼ����
 * ��    ������
 * �� �� ֵ����
 * ��    ע����ͼ��ɼ���ɺ󣬵���ͼ������
 */
void pit_handler_2()
{
    // ���ͼ��ɼ���ɱ�־
    if (mt9v03x_finish_flag)
    {
        mt9v03x_finish_flag = 0;  // �����־λ
        Image_Control();          // ִ��ͼ����
    }
}

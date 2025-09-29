#ifndef _MY_IMAGE_H_
#define _MY_IMAGE_H_

#include "zf_common_headfile.h"

// ============================ �궨���� ============================
// ͼ��ߴ���߽����
#define middle                 (MT9V03X_W / 2)               // ͼ������X����
#define y_MIN                  (MT9V03X_H * 2 / 8)           // �߽������СYֵ����
#define SHORTEST               (MT9V03X_H - 20)              // ��������ֵ��ֵ

// �Աȶ���ֵ
#define Y_THRESHOLD            22                             // ����ɨ�߶Աȶ���ֵ
#define X_THRESHOLD            15                             // ����ɨ�߶Աȶ���ֵ

// ============================ ö�����Ͷ��� ============================
/**
 * @brief б�ʼ���ģʽö��
 * ���岻ͬ�ı߽��߼����߼�
 */
typedef enum {
    SLOPE_MIDDLE,            // ����Ѳ��ģʽ
    SLOPE_LEFT,              // �����Ѳ��ģʽ
    SLOPE_RIGHT              // �ұ���Ѳ��ģʽ
} SlopeMode;

// ============================ ȫ�ֱ������� ============================
/** ͼ����������� */
extern int Right_Guai[2], Left_Guai[2];        // �յ����� [0]Y���� [1]X����

/** ����ͷ���� */
extern uint16 OBJECT_light;                    // ��������ͷ�ع�ʱ��
extern uint16 COLOR_light;                     // ɫ��ʶ������ͷ�ع�ʱ��
extern uint16 mt9v03X_light;                   // MT9V03X����ͷ�ع�ʱ��

/** �߽������ */
extern int Left_Lost_Flag[MT9V03X_H];          // ���߱�־����(1:����)
extern int Right_Lost_Flag[MT9V03X_H];         // �Ҷ��߱�־����(1:����)
extern uint8 boder_L[MT9V03X_H - 5];           // ��߽�����(��ɫ)
extern uint8 boder_R[MT9V03X_H - 5];           // �ұ߽�����(��ɫ)
extern uint8 boder_M[MT9V03X_H - 5];           // ��������(��ɫ)
extern uint8 boder_U[MT9V03X_W];               // �ϱ�������(��ɫ)
extern uint8 boder_correct[60];                // ���߽���������

/** ���״̬���� */
extern uint8 longest, index;                   // �����Y�����X����
extern uint8 shortest, index_shortest;         // ��̰���Y�����X����
extern uint8 cross_flag, ramp_flag;            // ʮ��/�µ���־λ(1:��⵽)
extern uint8 lose_point_num_L, lose_point_num_R;// ���ұ߽綪�ߵ���
extern uint8 Both_Lost_Time;                   // ˫�߶��ߴ���
extern int Boundry_Start_Left, Boundry_Start_Right; // ���ұ߽���ʼ��

/** ����ģʽ���� */
extern uint8 Slope_Mode;                       // б�ʼ���ģʽ
extern uint8 isopenResetM;                     // �������ÿ���
extern uint8 found_left, found_right;          // �߽��ҵ���־

/** ���������� */
extern int start_column_Right_Island;          // �һ�����ʼ��

/** ͼ�����ݲ��� */
extern uint8 white_value;                       // ��ɫ�ο���ֵ

// ============================ �������� ============================
/**
 * ��ʼ������
 */
void my_image_init(void);                       // ͼ�񴫸�����ʼ��

/**
 * ͼ������
 */
uint8 find_white_point(uint8 image_array[][188]); // �����ɫ�ο���ֵ
void find_longest(uint8* longest, uint8* index);   // ���������
void find_middle(void);                           // �������ߺͱ߽�

/**
 * ��������
 */
void show_boder_line(void);                     // ��ʾ�߽���(������)

/**
 * ����б�ʺ���
 */
 int16 slope(uint8 slope_mode);                  // ����ͼ��б��

#endif
    
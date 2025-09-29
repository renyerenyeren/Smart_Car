#include "My_image.h"


// ============================ ȫ�ֱ��������� ============================
/** ͼ�������������ʼ�� */
int Right_Guai[2] = {0};
int Left_Guai[2] = {0};          // ����յ����꣬[0]��y��[1]��x

/** ����ͷ������ʼ�� */
uint16 OBJECT_light = ART_Exposure_Time;    // ��������ͷ�ع�ʱ��
uint16 COLOR_light = ART_Exposure_Time;     // ��ɫ������ͷ�ع�ʱ��
uint16 mt9v03X_light = Exposure_Time;       // MT9V03X����ͷ�ع�ʱ��(Ĭ��200-130)

/** �߽��������ʼ�� */
int Left_Lost_Flag[MT9V03X_H] = {0};  // �������飬������1��û������0
int Right_Lost_Flag[MT9V03X_H] = {0}; // �Ҷ������飬������1��û������0
uint8 boder_L[MT9V03X_H - 5] = {0};   // ���ɨ�ߣ���߽� ��ɫ��
uint8 boder_R[MT9V03X_H - 5] = {0};   // �ұ�ɨ�ߣ��ұ߽� ��ɫ��
uint8 boder_M[MT9V03X_H - 5] = {0};   // ���� ����ɫ��
uint8 boder_U[MT9V03X_W] = {0};       // �ϱ�ɨ�ߣ��ϱ��� ��ɫ��
uint8 boder_correct[60] = {0};        // ���ڱ��߽����Ĳ������ݵ㣨y���꣩
uint8 longest = 0, index = 0;         // ����еĶ����Ӧ��������ͺ�����
uint8 shortest = 0, index_shortest = 0;
uint8 cross_flag = 0, ramp_flag = 0;  // �µ�ʮ�ֵı�־λ��1Ϊ����˴�
uint8 lose_point_num_L = 0, lose_point_num_R = 0;
uint8 Both_Lost_Time = 0;             // ���ұ��߶��ߵ�����˫�߶��ߴ���
int Boundry_Start_Left = 0;
int Boundry_Start_Right = 0;          // ���ұ߽���ʼ��

/** ����ģʽ������ʼ�� */
uint8 Slope_Mode = 0;                       // ����б�ʵ�ģʽ
uint8 isopenResetM = 0;                     // �������ÿ���
uint8 found_left = 0, found_right = 0;      // ����Ƿ��ҵ���ʼ��

/** ���������س�ʼ�� */
int start_column_Right_Island = 0;

/** ͼ�����ݲ��� */
uint8 white_value = 120;                    // �ο��׵�Ĭ��ֵ

// ============================ ��ʼ����غ��� ============================
/**
  * ��    ����my_image_init
  * ��    �ܣ���ʼ��ͼ�񴫸����������ع�ʱ��
  * ��    ������
  * �� �� ֵ����
  * ��    ע��1. ��ʼ��ʧ�ܻ�������ԣ�ֱ���ɹ�Ϊֹ��
  *           2. ��ʼ����ɺ��ӳ�100msȷ���ȶ�
  */
void my_image_init()
{
    // ��ʼ��MT9V03X����ͷ
    while (1)
    {
        if (mt9v03x_init())
        {
            // tft180_show_string(0,0,"camera init error");
        }
        else
        {
            break;
        }
        system_delay_ms(500);
    }
    
    // ��������ͷ�ع�ʱ��
    while (1)
    {
        if (mt9v03x_set_exposure_time(mt9v03X_light))  // Ĭ��120-100
        {
            // tft180_show_string(0,0,"set exposure time error");
        }
        else
        {
            break;
        }
        system_delay_ms(500);
    }
    system_delay_ms(100);
}

// ============================ ͼ������غ��� ============================
/**
  * ��    ����find_white_point
  * ��    �ܣ�����ͼ���а�ɫ�ο���ֵ
  * ��    ����image_array[][188] - ����ͼ������
  * �� �� ֵ��uint8 - ��ɫ�ο���ֵ
  * ��    ע��1. ���ض�����(10�С�2��)���в�������ƽ��ֵ��
  *           2. ���˻Ҷ�ֵ����55�����ص㣬������ţ�
  *           3. ����Ч������ʱ����Ĭ��ֵ120
  */
uint8 find_white_point(uint8 image_array[][188])
{
    uint16 total = 0, cnt = 0;
    // ���ض�������в�����10�С�2�У�
    for (uint8 i = 0; i < 20; i += 2)
    {
        for (uint8 j = 0; j < 8; j += 4)
        {
            // ���˵��Ҷ�̫�͵����ص㣨��ֵ55��
            if (image_array[84 + i][100 + j] > 55)
            {
                cnt++;
                total += image_array[84 + i][100 + j];
            }
        }
    }
    // ������Ч�������򷵻�ƽ��ֵ�����򷵻�Ĭ��ֵ120
    return (cnt) ? (total / cnt) : 120;
}

/**
  * ��    ����find_longest
  * ��    �ܣ�����ͼ��������У�Yֵ��С�İ��У�
  * ��    ����longest - �����Y���꣨���������
  * ��    ����index - �����X���������������
  * �� �� ֵ����
  * ��    ע��1. ֧�ֻ��������µ�������Χ��̬������
  *           2. �������������ȡƽ��X������Ϊ�����
  *           3. ������ں����߽���Ļ�׼��
  */
void find_longest(uint8* longest, uint8* index)
{
    uint8 high_l = 0, high_r = 0, highest_count = 0;  // �������в���
    uint16 high_sum = 0;                              // ��������X�����ܺ�
    *longest = MT9V03X_H - 10;                        // ��ʼ�������Yֵ
    shortest = 10;                                    // ��ʼ����̰���Yֵ
    
    // ��̬�����ɫ�ο�ֵ����Ȩƽ����
    white_value = 0.7f * 130 + 0.3f * find_white_point(mt9v03x_image);
    // tft180_show_int(188, 90, white_value, 3);

    // ���ݻ���״̬����������Χ
    int start_column = 0;
    int end_column = MT9V03X_W;
    
    if (Right_Island_Flag == 1 && Island_State == 3)  // �һ�����
    {
        start_column = start_column_Right_Island;
        end_column = MT9V03X_W;
    }
    else if (Left_Island_Flag == 1 && Island_State == 3)  // �󻷳���
    {
        start_column = 0;
        end_column = MT9V03X_W - 20;
    }
    
    // ����ɨ��Ѱ�������
    for (uint8 i = start_column; i < end_column; i += 2)
    {
        for (int16 j = MT9V03X_H - 6; j >= 0; j--)
        {
            int16 a = j;
            int16 b = a + 4;

            // �������ɫ�ο�ֵ�ĶԱȶ�
            uint8 value_2 = 100 * abs(mt9v03x_image[a][i] - white_value) / 
                           (mt9v03x_image[a][i] + white_value + 1);
            
            // ����������У�����Ϊ��ɫ��
            if (value_2 < Y_THRESHOLD && a <= 2)
            {
                highest_count++;
                high_sum += i;
                // tft180_draw_point(i, j, RGB565_RED);
            }
            
            // �����б߽磨�Աȶȳ�����ֵ��
            if (value_2 > Y_THRESHOLD)
            {
                // tft180_draw_point(i, j, RGB565_RED);
                boder_U[i] = j;  // �����ϱ�������

                // ���߸����ĵ����ڱ߽����
                if (i >= middle - 30 && i < middle + 30)
                {
                    boder_correct[i - middle + 30] = (j < y_MIN) ? y_MIN : j;
                }

                // ���������
                if (j < *longest)
                {
                    *longest = j;
                    *index = i;
                }
                
                break;  // �ҵ��߽���˳���ǰ��ɨ��
            }
        }
    }

    // �������д���ȡƽ��X���꣩
    if (high_l && high_r) 
    {
        *index = (high_l + high_r) / 2;
    }
    if (highest_count != 0 && high_sum != 0 && 
        abs(high_sum / highest_count - *index) < 40)
    {
        *longest = 0;
        *index = high_sum / highest_count;
    }
}

/**
  * ��    ����find_middle
  * ��    �ܣ�����ͼ���е����ߺ����ұ߽�
  * ��    ������
  * �� �� ֵ����
  * ��    ע��1. ��ͼ��ײ��������д�������߽粢ͳ�ƶ��������
  *           2. ����й���ʱ��>SHORTEST��ֱ�ӷ��أ���Ϊ�ܳ�������
  *           3. ��¼�׸��Ƕ��ߵ���Ϊ�߽���ʼ�У����ں�������
  */
void find_middle()
{
    // ��ʼ��״̬����
    lose_point_num_L = 0;
    lose_point_num_R = 0;
    Both_Lost_Time = 0;
    Boundry_Start_Left = 0;
    Boundry_Start_Right = 0;
    found_left = 0;
    found_right = 0;

    // ���ҵ��������Ϊ��׼
    find_longest(&longest, &index);
    if (longest > SHORTEST)  // ����й��̣���Ϊ�ܳ�����
    {
        return;
    }

    // ��ͼ��ײ��������д���
    for (int16 i = MT9V03X_H - 6; i >= 0; i--)
    {
        // --- ��߽���� ---
        Left_Lost_Flag[i] = 0;  // Ĭ�ϲ�����
        for (int16 j = index; j >= 0; j--)
        {
            if (j < 5)  // �������Ե���ж�Ϊ����
            {
                boder_L[i] = 0;
                Left_Lost_Flag[i] = 1;
                break;
            }
            
            // �������Աȶ�
            uint8 a = j;
            uint8 b = j - 5;
            uint8 value_1 = 100 * abs(mt9v03x_image[i][a] - mt9v03x_image[i][b]) / 
                           (mt9v03x_image[i][a] + mt9v03x_image[i][b] + 1);
            uint8 value_2 = 100 * abs(mt9v03x_image[i][b] - white_value) / 
                           (mt9v03x_image[i][b] + white_value + 1);
            
            // �Աȶȼ�Ȩ�ж��߽�
            if (value_1 * 0.6f + value_2 * 0.4f > X_THRESHOLD)
            {
                boder_L[i] = b;
                break;
            }
        }

        // --- �ұ߽���� ---
        Right_Lost_Flag[i] = 0;  // Ĭ�ϲ�����
        for (int16 j = index; j < MT9V03X_W; j++)
        {
            if (j > MT9V03X_W - 6)  // �����ұ�Ե���ж�Ϊ����
            {
                boder_R[i] = MT9V03X_W - 1;
                Right_Lost_Flag[i] = 1;
                break;
            }
            
            // �������Աȶ�
            uint8 a = j;
            uint8 b = j + 5;
            uint8 value_1 = 100 * abs(mt9v03x_image[i][a] - mt9v03x_image[i][b]) / 
                           (mt9v03x_image[i][a] + mt9v03x_image[i][b] + 1);
            uint8 value_2 = 100 * abs(mt9v03x_image[i][b] - white_value) / 
                           (mt9v03x_image[i][b] + white_value + 1);
            
            // �Աȶȼ�Ȩ�ж��߽�
            if (value_1 * 0.6f + value_2 * 0.4f > X_THRESHOLD)
            {
                boder_R[i] = b;
                break;
            }
        }

        // �������ߣ����ұ߽��е㣩
        boder_M[i] = (boder_L[i] + boder_R[i]) / 2;

        // ͳ�ƶ������
        if (Left_Lost_Flag[i]) lose_point_num_L++;
        if (Right_Lost_Flag[i]) lose_point_num_R++;
        if (Left_Lost_Flag[i] && Right_Lost_Flag[i]) Both_Lost_Time++;

        // ��¼�׸��Ƕ��ߵ㣨�ӵײ����ϣ�
        if (!found_left && !Left_Lost_Flag[i])
        {
            Boundry_Start_Left = i;
            found_left = 1;
        }
        if (!found_right && !Right_Lost_Flag[i])
        {
            Boundry_Start_Right = i;
            found_right = 1;
        }

        // ����·��ע�͵��ı��ô��룩
        // Road_Wide[i] = boder_R[i] - boder_L[i];
    }

    // �߽���ʼ�㶵�״���δ�ҵ�ʱĬ�ϵײ���
    if (!found_left) Boundry_Start_Left = MT9V03X_H - 6;
    if (!found_right) Boundry_Start_Right = MT9V03X_H - 6;
}

// ============================ �������� ============================
/**
  * ��    ����show_boder_line
  * ��    �ܣ�����Ļ����ʾ�߽��ߣ������ã�
  * ��    ������
  * �� �� ֵ����
  * ��    ע��1. ��߽���ʾΪ��ɫ���ұ߽�Ϊ��ɫ������Ϊ��ɫ��
  *           2. ��������ʱ����رգ��Խ�ʡϵͳ��Դ��
  *           3. ������ѭ���е��ò��ܳ�����ʾ
  */
void show_boder_line()
{
    for (uint8 i = longest; i < MT9V03X_H - 5; i++)
    {
        ips200_draw_point(boder_L[i], i, RGB565_YELLOW);  // ��߽�-��ɫ
        ips200_draw_point(boder_R[i], i, RGB565_GREEN);   // �ұ߽�-��ɫ
        ips200_draw_point(boder_M[i], i, RGB565_BLACK);   // ����-��ɫ
    }
}


// ============================ б�ʼ��㺯�� ============================
/**
  * ��    ����slope
  * ��    �ܣ�����ͼ��߽��б�ʣ����ڿ���С��ת��
  * ��    ����slope_mode - б�ʼ���ģʽ��SLOPE_MIDDLE/SLOPE_LEFT/SLOPE_RIGHT��
  * �� �� ֵ��int16 - ����õ���б��ֵ�����޷�����
  * ��    ע��1. ����ģʽ�£�����й���ʱ�����ϴν����
  *           2. б�ʼ�����ü�Ȩ��ͷ�ʽ���ײ�Ȩ�ظ��ߣ�
  *           3. �����Χ�������ڡ�80֮��
  */
int16 slope(uint8 slope_mode)
{
    static int16 lastresult = 0;  // ������һ�ν�������쳣����

    // �������ô���
    if (isopenResetM == 1)
    {
        for (uint8 i = longest; i < MT9V03X_H - 5; i++)
        {
            boder_M[i] = (boder_L[i] + boder_R[i]) / 2;
        }
    }

    // ����Ѳ��ģʽ
    if (slope_mode == SLOPE_MIDDLE)
    {
        if (longest > SHORTEST)  // ���������ֵ�������ϴν��
        {
            return lastresult;
        }

        int16 sum1 = 0, sum2 = 0, result;
        
        // ���ݰ��г���ѡ��ͬ�Ĳ�����Χ
        if (longest < 15 && (cross_flag * Island_State) == 0)
        {
            for (uint8 i = longest + 30; i <= MT9V03X_H - 10; i++)
            {
                sum1 += (int16)(boder_M[i] - middle);  // �����ߵ�ƫ��
                sum2 += (int16)(MT9V03X_H - 10 - i);   // Ȩ�أ�����ײ��ľ��룩
            }
        }
        else
        {
            for (uint8 i = longest; i <= MT9V03X_H - 10; i++)
            {
                sum1 += (int16)(boder_M[i] - middle);
                sum2 += (int16)(MT9V03X_H - 10 - i);
            }
        }
        
        // ����б�ʣ���ֹ���㣩
        result = 50 * sum1 / (sum2 + 1);
        result = func_limit(result, 80);  // �޷�����
        lastresult = result;
        
        return result;
    }
    // �����Ѳ��ģʽ
    else if (slope_mode == SLOPE_LEFT)
    {
        int16 sum1 = 0, sum2 = 0, result;
        for (uint8 i = longest; i <= MT9V03X_H - 10; i++)
        {
            sum1 += (int16)(boder_L[i] - middle);
            sum2 += (int16)(MT9V03X_H - 10 - i);
        }
        result = 50 * sum1 / (sum2 + 1);
        result = func_limit(result, 80);
        return result;
    }
    // �ұ���Ѳ��ģʽ
    else if (slope_mode == SLOPE_RIGHT)
    {
        int16 sum1 = 0, sum2 = 0, result;
        for (uint8 i = longest; i <= MT9V03X_H - 10; i++)
        {
            sum1 += (int16)(boder_R[i] - middle);
            sum2 += (int16)(MT9V03X_H - 10 - i);
        }
        result = 50 * sum1 / (sum2 + 1);
        result = func_limit(result, 80);
        return result;
    }
    // ��Чģʽ
    else
    {
        return 0;
    }
}

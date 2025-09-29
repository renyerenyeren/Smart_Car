#include "Element.h"

// ============================ ȫ�ֱ��������� ============================
/** ʮ�ּ����ر��� */
int Left_Down_Find = 0;       // ���·��յ�������ʮ�ּ���ã�
int Left_Up_Find = 0;         // ���Ϸ��յ�������ʮ�ּ���ã�
int Right_Down_Find = 0;      // ���·��յ�������ʮ�ּ���ã�
int Right_Up_Find = 0;        // ���Ϸ��յ�������ʮ�ּ���ã�

/** ���������ر��� */
int Island_State = 0;         // ����״̬��־
int Left_Island_Flag = 0;     // �󻷵���־
int Right_Island_Flag = 0;    // �һ�����־
int left_down_guai[2] = {0};  // ���¹յ����꣨[0]��, [1]�У�
int right_down_guai[2] = {0}; // ���¹յ����꣨[0]��, [1]�У�
int monotonicity_change_line[2] = {0};  // �����Ա仯�����꣨[0]��, [1]�У�
int monotonicity_change_left_flag = 0;  // ��߽絥���Ա仯��־�����仯Ϊ0��
int monotonicity_change_right_flag = 0; // �ұ߽絥���Ա仯��־�����仯Ϊ0��
int continuity_change_right_flag = 0;   // �ұ߽������Ա�־������Ϊ0��
int continuity_change_left_flag = 0;    // ��߽������Ա�־������Ϊ0��
int monotonicity_change_column[2] = {0};// �����Ա仯�����꣨[0]��, [1]�У�

// ============================ ʼ���߼����غ��� ============================
/**
 * ��    ����find_start_finish_line
 * ��    �ܣ����ʼ���ߣ������ߣ�
 * ��    ������
 * �� �� ֵ��0-δ��⵽ʼ���ߣ�1-��⵽ʼ����
 * ��    ע��ͨ�����Ҷȱ仯ͳ�ƺ�ɫ�������жϣ���ֵ�ɵ���
 */
#define GrayThreshold 75  // �Ҷ���ֵ���ޣ�85-10����С�ڴ�ֵΪ�ڵ㣬����Ϊ�׵�
uint8 find_start_finish_line()
{
    uint8 judge_state = 0;      // �ж�״̬
    uint8 black_block_num = 0;  // ͳ�ư����ߺ�ɫ������
    
    // ��ʮ�ֺͻ���״̬�²ż��
    if (!(cross_flag || Island_State))
    {
        for (uint8 i = 10; i < MT9V03X_W - 10; i++)
        {
            switch (judge_state)
            {
                case 0:  // ��ʼ״̬����⵽�׵������һ״̬
                    if (mt9v03x_image[50][i] > GrayThreshold)
                        judge_state = 1;
                    break;
                case 1:  // ��⵽�ڵ������һ״̬
                    if (mt9v03x_image[50][i] < GrayThreshold)
                        judge_state = 2;
                    break;
                case 2:  // ��⵽�׵������һ���ڰ����ڣ�������1
                    if (mt9v03x_image[50][i] > GrayThreshold)
                    {
                        judge_state = 1;
                        black_block_num++;
                    }
                    break;
            }
        }
    }
    
    // ��ɫ����������5���ж�Ϊ������
    return (black_block_num > 5) ? 1 : 0;
}

// ============================ ʮ�ּ����غ��� ============================
/**
 * ��    ����Find_Down_Point
 * ��    �ܣ������·������յ㣨��ʮ�ּ���ã�
 * ��    ����start - ������Χ���
 *           end   - ������Χ�յ�
 * �� �� ֵ���ޣ�����洢��ȫ�ֱ���Left_Down_Find��Right_Down_Find�У�
 * ��    ע��δ�ҵ�ʱ��Ӧ����Ϊ0���ҵ�ʱ�洢�յ���������
 */
void Find_Down_Point(int start, int end)
{
    int i, t;
    Right_Down_Find = 0;
    Left_Down_Find = 0;
    
    // ȷ���������յ㣨��������������
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // �߽籣��
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // ���������¹յ�
    for (i = start; i >= end; i--)
    {
        // ������¹յ�
        if (Left_Down_Find == 0 &&
            abs(boder_L[i] - boder_L[i+1]) <= 5 &&
            abs(boder_L[i+1] - boder_L[i+2]) <= 5 &&
            abs(boder_L[i+2] - boder_L[i+3]) <= 5 &&
            (boder_L[i] - boder_L[i-2]) >= 8 &&
            (boder_L[i] - boder_L[i-3]) >= 15 &&
            (boder_L[i] - boder_L[i-4]) >= 15)
        {
            Left_Down_Find = i;
        }
        
        // ������¹յ�
        if (Right_Down_Find == 0 &&
            abs(boder_R[i] - boder_R[i+1]) <= 5 &&
            abs(boder_R[i+1] - boder_R[i+2]) <= 5 &&
            abs(boder_R[i+2] - boder_R[i+3]) <= 5 &&
            (boder_R[i] - boder_R[i-2]) <= -8 &&
            (boder_R[i] - boder_R[i-3]) <= -15 &&
            (boder_R[i] - boder_R[i-4]) <= -15)
        {
            Right_Down_Find = i;
        }
        
        // �ҵ������յ㼴�˳�
        if (Left_Down_Find != 0 && Right_Down_Find != 0)
            break;
    }
}

/**
 * ��    ����Find_Up_Point
 * ��    �ܣ������Ϸ������յ㣨��ʮ�ּ���ã�
 * ��    ����start - ������Χ���
 *           end   - ������Χ�յ�
 * �� �� ֵ���ޣ�����洢��ȫ�ֱ���Left_Up_Find��Right_Up_Find�У�
 * ��    ע��δ�ҵ�ʱ��Ӧ����Ϊ0������˺�ѹ�����Ϊ����
 */
void Find_Up_Point(int start, int end)
{
    int i, t;
    Left_Up_Find = 0;
    Right_Up_Find = 0;
    
    // ȷ���������յ㣨��������������
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // �߽籣��
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    
    // ���������Ϲյ�
    for (i = start; i >= end; i--)
    {
        // ������Ϲյ�
        if (Left_Up_Find == 0 &&
            abs(boder_L[i] - boder_L[i-1]) <= 5 &&
            abs(boder_L[i-1] - boder_L[i-2]) <= 5 &&
            abs(boder_L[i-2] - boder_L[i-3]) <= 5 &&
            (boder_L[i] - boder_L[i+2]) >= 8 &&
            (boder_L[i] - boder_L[i+3]) >= 15 &&
            (boder_L[i] - boder_L[i+4]) >= 15)
        {
            Left_Up_Find = i;
        }
        
        // ������Ϲյ�
        if (Right_Up_Find == 0 &&
            abs(boder_R[i] - boder_R[i-1]) <= 5 &&
            abs(boder_R[i-1] - boder_R[i-2]) <= 5 &&
            abs(boder_R[i-2] - boder_R[i-3]) <= 5 &&
            (boder_R[i] - boder_R[i+2]) <= -8 &&
            (boder_R[i] - boder_R[i+3]) <= -15 &&
            (boder_R[i] - boder_R[i+4]) <= -15)
        {
            Right_Up_Find = i;
        }
        
        // �ҵ������յ㼴�˳�
        if (Left_Up_Find != 0 && Right_Up_Find != 0)
            break;
    }
    
    // ����˺�ѹ���>=30�У���Ϊ����
    if (abs(Right_Up_Find - Left_Up_Find) >= 30)
    {
        Right_Up_Find = 0;
        Left_Up_Find = 0;
    }
}

/**
 * ��    ����Cross_Detect
 * ��    �ܣ�ʮ�ּ���벹�ߴ���
 * ��    ������
 * �� �� ֵ����
 * ��    ע�������ĸ��յ��б����������ҵ��Ĺյ�������в��ߴ���
 */
void Cross_Detect()
{
    int down_search_start = 0;  // �µ�������ʼ��
    cross_flag = 0;
    
    // �뻷��״̬����
    if (Island_State == 0 && ramp_flag == 0)
    {
        Left_Up_Find = 0;
        Right_Up_Find = 0;
        
        // ˫�߶���ʱ���㹻��ʱ�ſ�ʼ�ҽǵ�
        if (Both_Lost_Time >= 10)
        {
            Find_Up_Point(MT9V03X_H - 6, 0);
            // δͬʱ�ҵ������ϵ���ֱ�ӷ���
            if (Left_Up_Find == 0 && Right_Up_Find == 0)
                return;
        }
        
        // �ҵ������ϵ����ж�Ϊʮ��
        if (Left_Up_Find != 0 && Right_Up_Find != 0)
        {
            cross_flag = 1;  // ����ʮ�ֱ�־λ
            
            // ���µ���������Ϊ�����Ϲյ��п��µ�һ��
            down_search_start = (Left_Up_Find > Right_Up_Find) ? 
                               Left_Up_Find : Right_Up_Find;
            Find_Down_Point(MT9V03X_H - 6, down_search_start + 2);
            
            // ���˲�������¹յ㣨�µ㲻�ܱ��ϵ㿿�ϣ�
            if (Left_Down_Find <= Left_Up_Find)
                Left_Down_Find = 0;
            if (Right_Down_Find <= Right_Up_Find)
                Right_Down_Find = 0;
            
            // �����ҵ��Ĺյ�������в���
            isopenResetM = 1;  // �������¼�������
            if (Left_Down_Find != 0 && Right_Down_Find != 0)
            {
                // �ĸ��㶼�ҵ���ֱ������
                Left_Add_Line(boder_L[Left_Up_Find], Left_Up_Find,
                             boder_L[Left_Down_Find], Left_Down_Find);
                Right_Add_Line(boder_R[Right_Up_Find], Right_Up_Find,
                              boder_R[Right_Down_Find], Right_Down_Find);
            }
            else if (Left_Down_Find == 0 && Right_Down_Find != 0)
            {
                // ���µ���ȱʧ���ӳ���߽�
                Lengthen_Left_Boundry(Left_Up_Find - 1, MT9V03X_H - 6);
                Right_Add_Line(boder_R[Right_Up_Find], Right_Up_Find,
                              boder_R[Right_Down_Find], Right_Down_Find);
            }
            else if (Left_Down_Find != 0 && Right_Down_Find == 0)
            {
                // ���µ���ȱʧ���ӳ��ұ߽�
                Left_Add_Line(boder_L[Left_Up_Find], Left_Up_Find,
                             boder_L[Left_Down_Find], Left_Down_Find);
                Lengthen_Right_Boundry(Right_Up_Find - 1, MT9V03X_H - 6);
            }
            else
            {
                // �����µ㶼ȱʧ���ӳ����ұ߽�
                Lengthen_Left_Boundry(Left_Up_Find - 1, MT9V03X_H - 6);
                Lengthen_Right_Boundry(Right_Up_Find - 1, MT9V03X_H - 6);
            }
        }
        else
        {
            cross_flag = 0;
            isopenResetM = 0;  // �ر����¼�������
        }
    }
}

// ============================ ���������غ��� ============================
/**
 * ��    ����Island_Detect
 * ��    �ܣ����������״̬����
 * ��    ������
 * �� �� ֵ����
 * ��    ע��ͨ���յ��б𡢵����Ա仯�������Լ��ʵ�֣���Ϊ���״̬����
 */
void Island_Detect()
{ 
    static float k = 0;  // ״̬3��5ʹ�õ�б��
    
    // �����жϣ����߽������Ժ͵�����
    continuity_change_left_flag = Continuity_Change_Left(MT9V03X_H - 6, 15);
    continuity_change_right_flag = Continuity_Change_Right(MT9V03X_H - 6, 15);
    monotonicity_change_right_flag = Monotonicity_Change_Right(MT9V03X_H - 1 - 10, 15);
    monotonicity_change_left_flag = Monotonicity_Change_Left(MT9V03X_H - 1 - 10, 15);
    
    // ��ʮ�ֺ��µ�״̬����δ���뻷��ʱ���м��
    if (cross_flag == 0 && Island_State == 0 && ramp_flag == 0)
    {
        // �󻷵����
        if (Left_Island_Flag == 0)
        {
            if (monotonicity_change_right_flag == 0 &&       // �ұߵ���
                continuity_change_left_flag != 0 &&          // ��߲�����
                continuity_change_right_flag == 0 &&         // �ұ�����
                lose_point_num_L >= 10 && lose_point_num_L <= 50 &&  // ��߶�������
                lose_point_num_R <= 15 &&                    // �ұ߶�����
                longest <= 25 &&                             // ������ֹ�н�Զ
                Boundry_Start_Left >= MT9V03X_H - 30 && 
                Boundry_Start_Right >= MT9V03X_H - 30 &&     // �߽���ʼ�㿿��
                Both_Lost_Time <= 8)                         // ˫�߶���ʱ���
            {
                left_down_guai[0] = Find_Left_Down_Point(MT9V03X_H - 6, 20);
                // �յ�λ�ú������ж�Ϊ�󻷵�
                if (left_down_guai[0] >= 35)
                {
                    Island_State = 1;
                    Left_Island_Flag = 1;
                }
                else
                {
                    Island_State = 0;
                    Left_Island_Flag = 0;
                }
            }
        }
        
        // �һ������
        if (Right_Island_Flag == 0)
        {
            if (monotonicity_change_left_flag == 0 &&        // ��ߵ���
                continuity_change_left_flag == 0 &&          // �������
                continuity_change_right_flag != 0 &&         // �ұ߲�����
                lose_point_num_R >= 10 && lose_point_num_R <= 50 &&  // �ұ߶�������
                lose_point_num_L <= 15 &&                    // ��߶�����
                longest <= 15 &&                             // ������ֹ�н�Զ
                Boundry_Start_Left >= MT9V03X_H - 30 && 
                Boundry_Start_Right >= MT9V03X_H - 30 &&     // �߽���ʼ�㿿��
                Both_Lost_Time <= 8)                         // ˫�߶���ʱ���
            {
                right_down_guai[0] = Find_Right_Down_Point(MT9V03X_H - 6, 20);
                // �յ�λ�ú������ж�Ϊ�һ���
                if (right_down_guai[0] >= 35)
                {
                    Island_State = 1;
                    Right_Island_Flag = 1;
                }
                else
                {
                    Island_State = 0;
                    Right_Island_Flag = 0;
                }
            }
        }
    }
    
    // �󻷵������߼�
    if (Left_Island_Flag == 1 && get_push_flag() == 0)
    {
        switch (Island_State)
        {
            case 1:  // ״̬1���յ���ڣ�δ����
                monotonicity_change_line[0] = Monotonicity_Change_Left(30, 10);
                monotonicity_change_line[1] = boder_L[monotonicity_change_line[0]];
                left_down_guai[0] = Find_Left_Down_Point(MT9V03X_H - 6, 20);
                
                // ���½ǿ�ʼ����ʱ����״̬2
                if (Boundry_Start_Left <= 80 && Boundry_Start_Left >= 35)
                {
                    Gyro_Angle.Zdata = 0;
                    Island_State = 2;
                }
                isopenResetM = 1;  // �������¼�������
                Left_Add_Line((int)(monotonicity_change_line[1] * 0.15), 
                             MT9V03X_H - 6, 
                             monotonicity_change_line[1], 
                             monotonicity_change_line[0]);
                break;
                
            case 2:  // ״̬2���·����ߣ��Ϸ��������ִ���
                monotonicity_change_column[0] = Boundry_Start_Left;
                monotonicity_change_column[1] = boder_L[Boundry_Start_Left];
                Draw_Line(boder_R[MT9V03X_H - 10], MT9V03X_H - 10,
                         monotonicity_change_column[1], monotonicity_change_column[0]);
                find_middle();
                isopenResetM = 0;  // �ر����¼�������
                
                // �����ǽǶ��㹻��ʱ����״̬4
                if (abs(Angle_now) >= 60)
                {
                    Island_State = 4;
                    Motor_Control_Mode = 0;
                }
                break;
                
            case 4:  // ״̬4����ȫ���뻷��
                if (Angle_now >= 20)  // ���ֽǶ��㹻���ⵥ���Ա仯
                {
                    monotonicity_change_line[0] = Monotonicity_Change_Left(MT9V03X_H - 10, 40);
                    monotonicity_change_line[1] = boder_L[monotonicity_change_line[0]];
                    
                    // �����㿿��ʱ����״̬5
                    if (55 <= monotonicity_change_line[0] || Boundry_Start_Right < 55)
                    {
                        Island_State = 5;
                        Draw_Line(boder_R[MT9V03X_H - 10], MT9V03X_H - 10,
                                 MT9V03X_W - 1, 0);
                        find_middle();
                        isopenResetM = 0;
                    }
                }
                break;
                
            case 5:  // ״̬5��׼��������
                Draw_Line(boder_R[MT9V03X_H - 10], MT9V03X_H - 10,
                         MT9V03X_W - 1, 0);
                find_middle();
                isopenResetM = 0;
                
                // �Ƕ��㹻Сʱ��������
                if (abs(Angle_now) <= 25)
                {
                    Island_State = 9;
                    Left_Island_Flag = 0;
                    Motor_Control_Mode = 0;
                }
                break;
        }
    }
    // �һ��������߼�
    else if (Right_Island_Flag == 1 && get_push_flag() == 0)
    {
        switch (Island_State)
        {
            case 1:  // ״̬1���յ���ڣ�δ����
                monotonicity_change_line[0] = Monotonicity_Change_Right(30, 10);
                monotonicity_change_line[1] = boder_R[monotonicity_change_line[0]];
                right_down_guai[0] = Find_Right_Down_Point(MT9V03X_H - 6, 20);
                
                // ���½ǿ�ʼ����ʱ����״̬2
                if (Boundry_Start_Right <= 80 && Boundry_Start_Right >= 35)
                {
                    Gyro_Angle.Zdata = 0;
                    Island_State = 2;
                }
                isopenResetM = 1;  // �������¼�������
                Right_Add_Line((int)(MT9V03X_W - 1 - (monotonicity_change_line[1] * 0.15)),
                              MT9V03X_H - 6,
                              monotonicity_change_line[1],
                              monotonicity_change_line[0]);
                break;
                
            case 2:  // ״̬2���·����ߣ��Ϸ��������ִ���
                monotonicity_change_column[0] = Boundry_Start_Right;
                monotonicity_change_column[1] = boder_R[Boundry_Start_Right];
                Draw_Line(boder_L[MT9V03X_H - 10], MT9V03X_H - 10,
                         monotonicity_change_column[1], monotonicity_change_column[0]);
                find_middle();
                isopenResetM = 0;  // �ر����¼�������
                
                // �����ǽǶ��㹻��ʱ����״̬4
                if (abs(Angle_now) >= 60)
                {
                    Island_State = 4;
                    Motor_Control_Mode = 0;
                }
                break;
                
            case 4:  // ״̬4����ȫ���뻷��
                if (Angle_now <= -20)  // ���ֽǶ��㹻���ⵥ���Ա仯
                {
                    monotonicity_change_line[0] = Monotonicity_Change_Right(MT9V03X_H - 10, 40);
                    monotonicity_change_line[1] = boder_R[monotonicity_change_line[0]];
                    
                    // �����㿿��ʱ����״̬5
                    if (55 <= monotonicity_change_line[0] || Boundry_Start_Right < 55)
                    {
                        Island_State = 5;
                        Draw_Line(boder_L[MT9V03X_H - 10], MT9V03X_H - 10,
                                 MT9V03X_W - 1, 0);
                        find_middle();
                        isopenResetM = 0;
                    }
                }
                break;
                
            case 5:  // ״̬5��׼��������
                Draw_Line(boder_L[MT9V03X_H - 10], MT9V03X_H - 10,
                         MT9V03X_W - 1, 0);
                find_middle();
                isopenResetM = 0;
                
                // �Ƕ��㹻Сʱ��������
                if (abs(Angle_now) <= 25)
                {
                    Island_State = 9;
                    Right_Island_Flag = 0;
                    Motor_Control_Mode = 0;
                }
                break;
        }
    }
    
    // ��������״̬����
    if (Island_State == 9)
    {
        static uint8 i = 0;
        i++;
        if (i >= 25)
        {
            i = 0;
            Island_State = 0;
            isopenResetM = 0;  // �ر����¼�������
        }
    }
}
    
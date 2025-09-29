#include "Elem_proc.h"

// ============================ ȫ�ֱ��������� ============================
/** ��ֵ��ͼ������ */
uint8 image_two_value[MT9V03X_H][MT9V03X_W];//��ֵ�����ͼ������

// ============================ �궨���� ============================
/** ͼ����ɫ���� */
#define IMG_BLACK     0X00      // ��ɫ����ֵ
#define IMG_WHITE     0XFF      // ��ɫ����ֵ
#define GrayScale     256       // �Ҷȼ���Χ��0-255��

// ============================ �����벹����غ��� ============================
/**
  * ��    ����Draw_Line
  * ��    �ܣ���ͼ���ϻ���һ�����Ϊ2�ĺ���
  * ��    ����startX - ��ʼ��X����
  *           startY - ��ʼ��Y����
  *           endX   - �յ�X����
  *           endY   - �յ�Y����
  * �� �� ֵ����
  * ��    ע��1. �Զ����������޷�������ֹ����Խ��
  *           2. ֧��ˮƽ�ߡ���ֱ�ߺ�����б�߻���
  * ʾ    ����Draw_Line(0, 0, MT9V03X_W-1, MT9V03X_H-1);
  */
void Draw_Line(int startX, int startY, int endX, int endY)
{
    int i, x, y;
    int start = 0, end = 0;
    
    // �����޷�����
    if (startX >= MT9V03X_W - 1) startX = MT9V03X_W - 1;
    else if (startX <= 0) startX = 0;
    if (startY >= MT9V03X_H - 6) startY = MT9V03X_H - 6;
    else if (startY <= 0) startY = 0;
    if (endX >= MT9V03X_W - 1) endX = MT9V03X_W - 1;
    else if (endX <= 0) endX = 0;
    if (endY >= MT9V03X_H - 6) endY = MT9V03X_H - 6;
    else if (endY <= 0) endY = 0;
    
    // ��������
    if (startX == endX)
    {
        if (startY > endY) { start = endY; end = startY; }
        else { start = startY; end = endY; }
        
        for (i = start; i <= end; i++)
        {
            if (i <= 1) i = 1;
            mt9v03x_image[i][startX] = 0;
            mt9v03x_image[i-1][startX] = 0;
        }
    }
    // ���ƺ���
    else if (startY == endY)
    {
        if (startX > endX) { start = endX; end = startX; }
        else { start = startX; end = startX; }
        
        for (i = start; i <= end; i++)
        {
            if (startY <= 1) startY = 1;
            mt9v03x_image[startY][i] = 0;
            mt9v03x_image[startY-1][i] = 0;
        }
    }
    // ��������б��
    else
    {
        // ������
        if (startY > endY) { start = endY; end = startY; }
        else { start = startY; end = endY; }
        
        for (i = start; i <= end; i++)
        {
            x = (int)(startX + (endX - startX) * (i - startY) / (endY - startY));
            if (x >= MT9V03X_W - 1) x = MT9V03X_W - 1;
            else if (x <= 1) x = 1;
            mt9v03x_image[i][x] = 0;
            mt9v03x_image[i][x-1] = 0;
        }
        
        // ������
        if (startX > endX) { start = endX; end = startX; }
        else { start = startX; end = endX; }
        
        for (i = start; i <= end; i++)
        {
            y = (int)(startY + (endY - startY) * (i - startX) / (endX - startX));
            if (y >= MT9V03X_H - 6) y = MT9V03X_H - 6;
            else if (y <= 0) y = 0;
            mt9v03x_image[y][i] = 0;
        }
    }
}

/**
  * ��    ����K_Draw_Line
  * ��    �ܣ�����б�ʺͶ�����ƺ���
  * ��    ����k        - ֱ��б��
  *           startX   - ��ʼ��X����
  *           startY   - ��ʼ��Y����
  *           endY     - �յ�Y����
  * �� �� ֵ����
  * ��    ע��ͨ��б�ʼ����յ�X��������Draw_Lineʵ�ֻ���
  * ʾ    ����K_Draw_Line(k, 20, MT9V03X_H-1, 0);
  */
void K_Draw_Line(float k, int startX, int startY, int endY)
{
    int endX = 0;
    
    // �����޷�����
    if (startX >= MT9V03X_W - 1) startX = MT9V03X_W - 1;
    else if (startX <= 0) startX = 0;
    if (startY >= MT9V03X_H - 6) startY = MT9V03X_H - 6;
    else if (startY <= 0) startY = 0;
    if (endY >= MT9V03X_H - 6) endY = MT9V03X_H - 6;
    else if (endY <= 0) endY = 0;
    
    // �����յ�X���� (y-y1)=k(x-x1) �� x=(y-y1)/k + x1
    endX = (int)((endY - startY) / k + startX);
    Draw_Line(startX, startY, endX, endY);
}

// ============================ ���������Լ�⺯�� ============================
/**
  * ��    ����Continuity_Change_Left
  * ��    �ܣ��������߽������Լ��
  * ��    ����start - �����ʼ��
  *           end   - �����ֹ��
  * �� �� ֵ����������0�����������ض�����������
  * ��    ע��1. ��������ֵ����Ϊ7
  *           2. ���󲿷�������ʱֱ�ӷ���1
  * ʾ    ����int flag = Continuity_Change_Left(10, 50);
  */
int Continuity_Change_Left(int start, int end)
{
    int i, t;
    int continuity_change_flag = 0;
    
    // �󲿷�������ʱֱ�ӷ���
    if (lose_point_num_L >= 0.9 * MT9V03X_H)
        return 1;
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // ȷ����ʼ�д�����ֹ�У��������¼�⣩
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // �����Լ��
    for (i = start; i >= end; i--)
    {
        if (abs(boder_L[i] - boder_L[i-1]) >= 7)
        {
            continuity_change_flag = i;
            break;
        }
    }
    return continuity_change_flag;
}

/**
  * ��    ����Continuity_Change_Right
  * ��    �ܣ��������߽������Լ��
  * ��    ����start - �����ʼ��
  *           end   - �����ֹ��
  * �� �� ֵ����������0�����������ض�����������
  * ��    ע��1. ��������ֵ����Ϊ7
  *           2. ���󲿷�������ʱֱ�ӷ���1
  * ʾ    ����int flag = Continuity_Change_Right(10, 50);
  */
int Continuity_Change_Right(int start, int end)
{
    int i, t;
    int continuity_change_flag = 0;
    
    // �󲿷�������ʱֱ�ӷ���
    if (lose_point_num_R >= 0.9 * MT9V03X_H)
        return 1;
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // ȷ����ʼ�д�����ֹ�У��������¼�⣩
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // �����Լ��
    for (i = start; i >= end; i--)
    {
        if (abs(boder_R[i] - boder_R[i-1]) >= 7)
        {
            continuity_change_flag = i;
            break;
        }
    }
    return continuity_change_flag;
}

// ============================ �ǵ�����غ��� ============================
/**
  * ��    ����Find_Left_Down_Point
  * ��    �ܣ����·��ǵ���
  * ��    ����start - �����ʼ��
  *           end   - �����ֹ��
  * �� �� ֵ���ǵ������������Ҳ�������0
  * ��    ע���ǵ�����ֵ�ɸ���ʵ���������
  * ʾ    ����int line = Find_Left_Down_Point(10, 50);
  */
int Find_Left_Down_Point(int start, int end)
{
    int i, t;
    int left_down_line = 0;
    
    // �󲿷�������ʱֱ�ӷ���
    if (lose_point_num_L >= 0.9 * MT9V03X_H)
        return left_down_line;
    
    // ���������ȷ��start > end��
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // �ǵ���
    for (i = start; i >= end; i--)
    {
        if (left_down_line == 0 &&
            abs(boder_L[i] - boder_L[i+1]) <= 5 &&
            abs(boder_L[i+1] - boder_L[i+2]) <= 5 &&
            abs(boder_L[i+2] - boder_L[i+3]) <= 5 &&
            (boder_L[i] - boder_L[i-2]) >= 5 &&
            (boder_L[i] - boder_L[i-3]) >= 10 &&
            (boder_L[i] - boder_L[i-4]) >= 10)
        {
            left_down_line = i;
            break;
        }
    }
    return left_down_line;
}

/**
  * ��    ����Find_Left_Up_Point
  * ��    �ܣ����Ϸ��ǵ���
  * ��    ����start - �����ʼ��
  *           end   - �����ֹ��
  * �� �� ֵ���ǵ������������Ҳ�������0
  * ��    ע���ǵ�����ֵ�ɸ���ʵ���������
  * ʾ    ����int line = Find_Left_Up_Point(10, 50);
  */
int Find_Left_Up_Point(int start, int end)
{
    int i, t;
    int left_up_line = 0;
    
    // �󲿷�������ʱֱ�ӷ���
    if (lose_point_num_L >= 0.9 * MT9V03X_H)
        return left_up_line;
    
    // ���������ȷ��start > end��
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // �ǵ���
    for (i = start; i >= end; i--)
    {
        if (left_up_line == 0 &&
            abs(boder_L[i] - boder_L[i-1]) <= 5 &&
            abs(boder_L[i-1] - boder_L[i-2]) <= 5 &&
            abs(boder_L[i-2] - boder_L[i-3]) <= 5 &&
            (boder_L[i] - boder_L[i+2]) >= 8 &&
            (boder_L[i] - boder_L[i+3]) >= 15 &&
            (boder_L[i] - boder_L[i+4]) >= 15)
        {
            left_up_line = i;
            break;
        }
    }
    return left_up_line;
}

/**
  * ��    ����Find_Right_Down_Point
  * ��    �ܣ����·��ǵ���
  * ��    ����start - �����ʼ��
  *           end   - �����ֹ��
  * �� �� ֵ���ǵ������������Ҳ�������0
  * ��    ע���ǵ�����ֵ�ɸ���ʵ���������
  * ʾ    ����int line = Find_Right_Down_Point(10, 50);
  */
int Find_Right_Down_Point(int start, int end)
{
    int i, t;
    int right_down_line = 0;
    
    // �󲿷�������ʱֱ�ӷ���
    if (lose_point_num_R >= 0.9 * MT9V03X_H)
        return right_down_line;
    
    // ���������ȷ��start > end��
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // �ǵ���
    for (i = start; i >= end; i--)
    {
        if (right_down_line == 0 &&
            abs(boder_R[i] - boder_R[i+1]) <= 5 &&
            abs(boder_R[i+1] - boder_R[i+2]) <= 5 &&
            abs(boder_R[i+2] - boder_R[i+3]) <= 5 &&
            (boder_R[i] - boder_R[i-2]) <= -5 &&
            (boder_R[i] - boder_R[i-3]) <= -10 &&
            (boder_R[i] - boder_R[i-4]) <= -10)
        {
            right_down_line = i;
            break;
        }
    }
    return right_down_line;
}

/**
  * ��    ����Find_Right_Up_Point
  * ��    �ܣ����Ϸ��ǵ���
  * ��    ����start - �����ʼ��
  *           end   - �����ֹ��
  * �� �� ֵ���ǵ������������Ҳ�������0
  * ��    ע���ǵ�����ֵ�ɸ���ʵ���������
  * ʾ    ����int line = Find_Right_Up_Point(10, 50);
  */
int Find_Right_Up_Point(int start, int end)
{
    int i, t;
    int right_up_line = 0;
    
    // �󲿷�������ʱֱ�ӷ���
    if (lose_point_num_R >= 0.9 * MT9V03X_H)
        return right_up_line;
    
    // ���������ȷ��start > end��
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // �ǵ���
    for (i = start; i >= end; i--)
    {
        if (right_up_line == 0 &&
            abs(boder_R[i] - boder_R[i-1]) <= 5 &&
            abs(boder_R[i-1] - boder_R[i-2]) <= 5 &&
            abs(boder_R[i-2] - boder_R[i-3]) <= 5 &&
            (boder_R[i] - boder_R[i+2]) <= -8 &&
            (boder_R[i] - boder_R[i+3]) <= -15 &&
            (boder_R[i] - boder_R[i+4]) <= -15)
        {
            right_up_line = i;
            break;
        }
    }
    return right_up_line;
}

// ============================ �����Լ����غ��� ============================
/**
  * ��    ����Monotonicity_Change_Right_Land
  * ��    �ܣ��ұ߽絥����ͻ����
  * ��    ����start - �����ʼ��
  *           end   - �����ֹ��
  * �� �� ֵ��ͻ��������������Ҳ�������94
  * ��    ע��ͨ���Ƚ�ǰ��5�����ж��Ƿ�Ϊ��ֵ��
  * ʾ    ����int col = Monotonicity_Change_Right_Land(10, 100);
  */
int Monotonicity_Change_Right_Land(int start, int end)
{
    int i;
    int monotonicity_change_column = 94;
    
    // ����Խ�籣��
    if (end >= MT9V03X_W - 1)
        end = MT9V03X_W - 1;
    if (start <= 16)
        start = 16;
    
    // �����Լ��
    for (i = start; i <= end; i += 2)
    {
        // �ų�������ֵͬ�����
        if (boder_U[i] == boder_U[i+10] && boder_U[i] == boder_U[i-10] &&
            boder_U[i] == boder_U[i+8] && boder_U[i] == boder_U[i-8] &&
            boder_U[i] == boder_U[i+6] && boder_U[i] == boder_U[i-6] &&
            boder_U[i] == boder_U[i+4] && boder_U[i] == boder_U[i-4] &&
            boder_U[i] == boder_U[i+2] && boder_U[i] == boder_U[i-2])
        {
            continue;
        }
        // �ж��Ƿ�Ϊ�ֲ����ֵ
        else if (boder_U[i] >= boder_U[i+10] && boder_U[i] >= boder_U[i-10] &&
                 boder_U[i] >= boder_U[i+8] && boder_U[i] >= boder_U[i-8] &&
                 boder_U[i] >= boder_U[i+6] && boder_U[i] >= boder_U[i-6] &&
                 boder_U[i] >= boder_U[i+4] && boder_U[i] >= boder_U[i-4] &&
                 boder_U[i] >= boder_U[i+2] && boder_U[i] >= boder_U[i-2])
        {
            monotonicity_change_column = i;
            break;
        }
    }
    return monotonicity_change_column;
}

/**
  * ��    ����Monotonicity_Change_Left
  * ��    �ܣ���߽絥����ͻ����
  * ��    ����start - �����ʼ��
  *           end   - �����ֹ��
  * �� �� ֵ��ͻ��������������Ҳ�������0
  * ��    ע��ͨ���Ƚ�ǰ��5�����ж��Ƿ�Ϊ��ֵ��
  * ʾ    ����int line = Monotonicity_Change_Left(10, 50);
  */
int Monotonicity_Change_Left(int start, int end)
{
    int i;
    int monotonicity_change_line = 0;
    
    // �󲿷�������ʱֱ�ӷ���
    if (lose_point_num_L >= 0.9 * MT9V03X_H)
        return monotonicity_change_line;
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // ��ʼ�б��������ֹ��
    if (start <= end)
        return monotonicity_change_line;
    
    // �����Լ��
    for (i = start; i >= end; i--)
    {
        // �ų�������ֵͬ�����
        if (boder_L[i] == boder_L[i+5] && boder_L[i] == boder_L[i-5] &&
            boder_L[i] == boder_L[i+4] && boder_L[i] == boder_L[i-4] &&
            boder_L[i] == boder_L[i+3] && boder_L[i] == boder_L[i-3] &&
            boder_L[i] == boder_L[i+2] && boder_L[i] == boder_L[i-2] &&
            boder_L[i] == boder_L[i+1] && boder_L[i] == boder_L[i-1])
        {
            continue;
        }
        // �ж��Ƿ�Ϊ�ֲ����ֵ
        else if (boder_L[i] >= boder_L[i+5] && boder_L[i] >= boder_L[i-5] &&
                 boder_L[i] >= boder_L[i+4] && boder_L[i] >= boder_L[i-4] &&
                 boder_L[i] >= boder_L[i+3] && boder_L[i] >= boder_L[i-3] &&
                 boder_L[i] >= boder_L[i+2] && boder_L[i] >= boder_L[i-2] &&
                 boder_L[i] >= boder_L[i+1] && boder_L[i] >= boder_L[i-1])
        {
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}

/**
  * ��    ����Monotonicity_Change_Right
  * ��    �ܣ��ұ߽絥����ͻ����
  * ��    ����start - �����ʼ��
  *           end   - �����ֹ��
  * �� �� ֵ��ͻ��������������Ҳ�������0
  * ��    ע��ͨ���Ƚ�ǰ��5�����ж��Ƿ�Ϊ��ֵ��
  * ʾ    ����int line = Monotonicity_Change_Right(10, 50);
  */
int Monotonicity_Change_Right(int start, int end)
{
    int i;
    int monotonicity_change_line = 0;
    
    // �󲿷�������ʱֱ�ӷ���
    if (lose_point_num_R >= 0.9 * MT9V03X_H)
        return monotonicity_change_line;
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // ��ʼ�б��������ֹ��
    if (start <= end)
        return monotonicity_change_line;
    
    // �����Լ��
    for (i = start; i >= end; i--)
    {
        // �ų�������ֵͬ�����
        if (boder_R[i] == boder_R[i+5] && boder_R[i] == boder_R[i-5] &&
            boder_R[i] == boder_R[i+4] && boder_R[i] == boder_R[i-4] &&
            boder_R[i] == boder_R[i+3] && boder_R[i] == boder_R[i-3] &&
            boder_R[i] == boder_R[i+2] && boder_R[i] == boder_R[i-2] &&
            boder_R[i] == boder_R[i+1] && boder_R[i] == boder_R[i-1])
        {
            continue;
        }
        // �ж��Ƿ�Ϊ�ֲ���Сֵ
        else if (boder_R[i] <= boder_R[i+5] && boder_R[i] <= boder_R[i-5] &&
                 boder_R[i] <= boder_R[i+4] && boder_R[i] <= boder_R[i-4] &&
                 boder_R[i] <= boder_R[i+3] && boder_R[i] <= boder_R[i-3] &&
                 boder_R[i] <= boder_R[i+2] && boder_R[i] <= boder_R[i-2] &&
                 boder_R[i] <= boder_R[i+1] && boder_R[i] <= boder_R[i-1])
        {
            monotonicity_change_line = i;
            break;
        }
    }
    return monotonicity_change_line;
}

// ============================ �߽粹�����ӳ����� ============================
/**
  * ��    ����K_Add_Boundry_Left
  * ��    �ܣ���б�ʲ�����߽�
  * ��    ����k        - �߽�б��
  *           startX   - ��ʼ��X����
  *           startY   - ��ʼ��Y����
  *           endY     - ��ֹ��Y����
  * �� �� ֵ����
  * ��    ע�����߽��ֱ�Ӹ��µ���߽�����boder_L
  * ʾ    ����K_Add_Boundry_Left(k, 20, 50, 10);
  */
void K_Add_Boundry_Left(float k, int startX, int startY, int endY)
{
    int i, t;
    
    // �����޷�����
    if (startY >= MT9V03X_H - 6)
        startY = MT9V03X_H - 6;
    else if (startY <= 0)
        startY = 0;
    if (endY >= MT9V03X_H - 6)
        endY = MT9V03X_H - 6;
    else if (endY <= 0)
        endY = 0;
    
    // ȷ����ʼ�д�����ֹ��
    if (startY < endY)
    {
        t = startY;
        startY = endY;
        endY = t;
    }
    
    // ��б�ʼ��㲢����߽�
    for (i = startY; i >= endY; i--)
    {
        boder_L[i] = (int)((i - startY) / k + startX);
        if (boder_L[i] >= MT9V03X_W - 1)
            boder_L[i] = MT9V03X_W - 1;
        else if (boder_L[i] <= 0)
            boder_L[i] = 0;
    }
}

/**
  * ��    ����K_Add_Boundry_Right
  * ��    �ܣ���б�ʲ����ұ߽�
  * ��    ����k        - �߽�б��
  *           startX   - ��ʼ��X����
  *           startY   - ��ʼ��Y����
  *           endY     - ��ֹ��Y����
  * �� �� ֵ����
  * ��    ע�����߽��ֱ�Ӹ��µ��ұ߽�����boder_R
  * ʾ    ����K_Add_Boundry_Right(k, 100, 50, 10);
  */
void K_Add_Boundry_Right(float k, int startX, int startY, int endY)
{
    int i, t;
    
    // �����޷�����
    if (startY >= MT9V03X_H - 6)
        startY = MT9V03X_H - 6;
    else if (startY <= 0)
        startY = 0;
    if (endY >= MT9V03X_H - 6)
        endY = MT9V03X_H - 6;
    else if (endY <= 0)
        endY = 0;
    
    // ȷ����ʼ�д�����ֹ��
    if (startY < endY)
    {
        t = startY;
        startY = endY;
        endY = t;
    }
    
    // ��б�ʼ��㲢����߽�
    for (i = startY; i >= endY; i--)
    {
        boder_R[i] = (int)((i - startY) / k + startX);
        if (boder_R[i] >= MT9V03X_W - 1)
            boder_R[i] = MT9V03X_W - 1;
        else if (boder_R[i] <= 0)
            boder_R[i] = 0;
    }
}

/**
  * ��    ����Get_Left_K
  * ��    �ܣ�������߽�б��
  * ��    ����start_line - ��ʼ��
  *           end_line   - ��ֹ��
  * �� �� ֵ��������б��
  * ��    ע��Ĭ����ʼ��С����ֹ�У��Զ���������˳��
  * ʾ    ����float k = Get_Left_K(10, 20);
  */
float Get_Left_K(int start_line, int end_line)
{
    // ����Խ�籣��
    if (start_line >= MT9V03X_H - 6)
        start_line = MT9V03X_H - 6;
    else if (start_line <= longest)
        start_line = longest;
    if (end_line >= MT9V03X_H - 6)
        end_line = MT9V03X_H - 6;
    else if (end_line <= longest)
        end_line = longest;
    
    float k = 0;
    int t = 0;
    
    // ȷ����ʼ��С����ֹ��
    if (start_line > end_line)
    {
        t = start_line;
        start_line = end_line;
        end_line = t;
    }
    
    // ����б��
    k = (float)(boder_L[start_line] - boder_L[end_line]) / (end_line - start_line + 1);
    return k;
}

/**
  * ��    ����Get_Right_K
  * ��    �ܣ������ұ߽�б��
  * ��    ����start_line - ��ʼ��
  *           end_line   - ��ֹ��
  * �� �� ֵ��������б��
  * ��    ע��Ĭ����ʼ��С����ֹ�У��Զ���������˳��
  * ʾ    ����float k = Get_Right_K(10, 20);
  */
float Get_Right_K(int start_line, int end_line)
{
    // ����Խ�籣��
    if (start_line >= MT9V03X_H - 6)
        start_line = MT9V03X_H - 6;
    else if (start_line <= longest)
        start_line = longest;
    if (end_line >= MT9V03X_H - 6)
        end_line = MT9V03X_H - 6;
    else if (end_line <= longest)
        end_line = longest;
    
    float k = 0;
    int t = 0;
    
    // ȷ����ʼ��С����ֹ��
    if (start_line > end_line)
    {
        t = start_line;
        start_line = end_line;
        end_line = t;
    }
    
    // ����б��
    k = (float)(boder_R[start_line] - boder_R[end_line]) / (end_line - start_line + 1);
    return k;
}

/**
  * ��    ����Lengthen_Left_Boundry
  * ��    �ܣ��ӳ���߽�
  * ��    ����start - �ӳ���ʼ��
  *           end   - �ӳ���ֹ��
  * �� �� ֵ����
  * ��    ע��ͨ��������ʼ�㸽��б�����ӳ��߽�
  * ʾ    ����Lengthen_Left_Boundry(20, 50);
  */
void Lengthen_Left_Boundry(int start, int end)
{
    int i, t;
    float k = 0;
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 6)
        end = MT9V03X_H - 6;
    else if (end <= 0)
        end = 0;
    
    // ȷ����ֹ�д�����ʼ��
    if (end < start)
    {
        t = end;
        end = start;
        start = t;
    }
    
    // ��ʼ���Сʱֱ������
    if (start <= 5)
    {
        Left_Add_Line(boder_L[start], start, boder_L[end], end);
    }
    // ����б���ӳ�
    else
    {
        k = (float)(boder_L[start] - boder_L[start-4]) / 5.0f;
        for (i = start; i <= end; i++)
        {
            boder_L[i] = (int)(i - start) * k + boder_L[start];
            if (boder_L[i] >= MT9V03X_W - 1)
                boder_L[i] = MT9V03X_W - 1;
            else if (boder_L[i] <= 0)
                boder_L[i] = 0;
        }
    }
}

/**
  * ��    ����Lengthen_Right_Boundry
  * ��    �ܣ��ӳ��ұ߽�
  * ��    ����start - �ӳ���ʼ��
  *           end   - �ӳ���ֹ��
  * �� �� ֵ����
  * ��    ע��ͨ��������ʼ�㸽��б�����ӳ��߽�
  * ʾ    ����Lengthen_Right_Boundry(20, 50);
  */
void Lengthen_Right_Boundry(int start, int end)
{
    int i, t;
    float k = 0;
    
    // ����Խ�籣��
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 6)
        end = MT9V03X_H - 6;
    else if (end <= 0)
        end = 0;
    
    // ȷ����ֹ�д�����ʼ��
    if (end < start)
    {
        t = end;
        end = start;
        start = t;
    }
    
    // ��ʼ���Сʱֱ������
    if (start <= 5)
    {
        Right_Add_Line(boder_R[start], start, boder_R[end], end);
    }
    // ����б���ӳ�
    else
    {
        k = (float)(boder_R[start] - boder_R[start-4]) / 5.0f;
        for (i = start; i <= end; i++)
        {
            boder_R[i] = (int)(i - start) * k + boder_R[start];
            if (boder_R[i] >= MT9V03X_W - 1)
                boder_R[i] = MT9V03X_W - 1;
            else if (boder_R[i] <= 0)
                boder_R[i] = 0;
        }
    }
}

/**
  * ��    ����Left_Add_Line
  * ��    �ܣ�����䲹����߽�
  * ��    ����x1, y1 - �������
  *           x2, y2 - �յ�����
  * �� �� ֵ����
  * ��    ע��ֱ�Ӹ�����߽�����boder_L
  * ʾ    ����Left_Add_Line(20, 10, 50, 30);
  */
void Left_Add_Line(int x1, int y1, int x2, int y2)
{
    int i, max, a1, a2;
    int hx;
    
    // �����޷�����
    if (x1 >= MT9V03X_W - 1) x1 = MT9V03X_W - 1;
    else if (x1 <= 0) x1 = 0;
    if (y1 >= MT9V03X_H - 6) y1 = MT9V03X_H - 6;
    else if (y1 <= 0) y1 = 0;
    if (x2 >= MT9V03X_W - 1) x2 = MT9V03X_W - 1;
    else if (x2 <= 0) x2 = 0;
    if (y2 >= MT9V03X_H - 6) y2 = MT9V03X_H - 6;
    else if (y2 <= 0) y2 = 0;
    
    // ȷ��a1 < a2
    a1 = y1;
    a2 = y2;
    if (a1 > a2)
    {
        max = a1;
        a1 = a2;
        a2 = max;
    }
    
    // ��������Բ�ֵ����߽�
    for (i = a1; i <= a2; i++)
    {
        hx = (i - y1) * (x2 - x1) / (y2 - y1) + x1;
        if (hx >= MT9V03X_W)
            hx = MT9V03X_W;
        else if (hx <= 0)
            hx = 0;
        boder_L[i] = hx;
    }
}

/**
  * ��    ����Right_Add_Line
  * ��    �ܣ�����䲹���ұ߽�
  * ��    ����x1, y1 - �������
  *           x2, y2 - �յ�����
  * �� �� ֵ����
  * ��    ע��ֱ�Ӹ����ұ߽�����boder_R
  * ʾ    ����Right_Add_Line(100, 10, 150, 30);
  */
void Right_Add_Line(int x1, int y1, int x2, int y2)
{
    int i, max, a1, a2;
    int hx;
    
    // �����޷�����
    if (x1 >= MT9V03X_W - 1) x1 = MT9V03X_W - 1;
    else if (x1 <= 0) x1 = 0;
    if (y1 >= MT9V03X_H - 1) y1 = MT9V03X_H - 1;
    else if (y1 <= 0) y1 = 0;
    if (x2 >= MT9V03X_W - 1) x2 = MT9V03X_W - 1;
    else if (x2 <= 0) x2 = 0;
    if (y2 >= MT9V03X_H - 1) y2 = MT9V03X_H - 1;
    else if (y2 <= 0) y2 = 0;
    
    // ȷ��a1 < a2
    a1 = y1;
    a2 = y2;
    if (a1 > a2)
    {
        max = a1;
        a1 = a2;
        a2 = max;
    }
    
    // ��������Բ�ֵ����߽�
    for (i = a1; i <= a2; i++)
    {
        hx = (i - y1) * (x2 - x1) / (y2 - y1) + x1;
        if (hx >= MT9V03X_W)
            hx = MT9V03X_W;
        else if (hx <= 0)
            hx = 0;
        boder_R[i] = hx;
    }
}

// ============================ ͼ���ֵ����غ��� ============================
/**
  * ��    ����my_adapt_threshold
  * ��    �ܣ����ٴ������ֵ
  * ��    ����image - ͼ������ָ��
  *           col   - ͼ����
  *           row   - ͼ��߶�
  * �� �� ֵ������õ�����ֵ
  * ��    ע���ȴ�ͳ��򷨼����ٶȸ���
  * ʾ    ����int th = my_adapt_threshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
  */
int my_adapt_threshold(uint8 *image, uint16 col, uint16 row)
{
    uint16 width = col;
    uint16 height = row;
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height / 4;
    int threshold = 0;
    uint8 (*data)[width] = (uint8 (*)[width])image;
    
    // ��ʼ������
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    
    uint32 gray_sum = 0;
    
    // ͳ�ƻҶ�ֵ�ֲ�
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            pixelCount[(int)data[i][j]]++;
            gray_sum += (int)data[i][j];
        }
    }
    
    // ����Ҷ�ֵ����
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    
    // ���������䷽���Ӧ����ֵ
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    
    for (j = 0; j < GrayScale; j++)
    {
        w0 += pixelPro[j];
        u0tmp += j * pixelPro[j];
        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;
        
        if (w0 == 0 || w1 == 0)
            continue;
        
        u0 = u0tmp / w0;
        u1 = u1tmp / w1;
        u = u0tmp + u1tmp;
        
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }
    
    return threshold;
}

/**
  * ��    ����My_Adapt_Threshold
  * ��    �ܣ���ͨ�������ֵ
  * ��    ����image  - ͼ������ָ��
  *           width  - ͼ����
  *           height - ͼ��߶�
  * �� �� ֵ������õ�����ֵ
  * ʾ    ����int th = My_Adapt_Threshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
  */
int My_Adapt_Threshold(uint8* image, uint16 width, uint16 height)
{
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height / 4;
    int threshold = 0;
    uint8 (*data)[width] = (uint8 (*)[width])image;
    
    // ��ʼ������
    memset(pixelCount, 0, sizeof(pixelCount));
    memset(pixelPro, 0, sizeof(pixelPro));
    
    uint32 gray_sum = 0;
    
    // ͳ�ƻҶ�ֵ�ֲ�
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            pixelCount[(int)data[i][j]]++;
            gray_sum += (int)data[i][j];
        }
    }
    
    // ����Ҷ�ֵ����
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    
    // ���������䷽���Ӧ����ֵ
    float w0, w1, u0tmp, u1tmp, u0, u1, u, deltaTmp, deltaMax = 0;
    w0 = w1 = u0tmp = u1tmp = u0 = u1 = u = deltaTmp = 0;
    
    for (j = 0; j < GrayScale; j++)
    {
        w0 += pixelPro[j];
        u0tmp += j * pixelPro[j];
        w1 = 1 - w0;
        u1tmp = gray_sum / pixelSum - u0tmp;
        
        if (w0 == 0 || w1 == 0)
            continue;
        
        u0 = u0tmp / w0;
        u1 = u1tmp / w1;
        u = u0tmp + u1tmp;
        
        deltaTmp = w0 * pow((u0 - u), 2) + w1 * pow((u1 - u), 2);
        
        if (deltaTmp > deltaMax)
        {
            deltaMax = deltaTmp;
            threshold = j;
        }
        if (deltaTmp < deltaMax)
        {
            break;
        }
    }
    
    // ��ֵ��Χ����
    if (threshold > 255)
        threshold = 255;
    
    return threshold;
}

/**
  * ��    ����Image_Binarization
  * ��    �ܣ�ͼ���ֵ������
  * ��    ����threshold - ��ֵ����ֵ
  * �� �� ֵ����
  * ��    ע������洢��image_two_value������
  * ʾ    ����Image_Binarization(128);
  */
void Image_Binarization(int threshold)
{
    uint16 i, j;
    for (i = 0; i < MT9V03X_H; i++)
    {
        for (j = 0; j < MT9V03X_W; j++)
        {
            if (mt9v03x_image[i][j] >= threshold)
                image_two_value[i][j] = IMG_WHITE;
            else
                image_two_value[i][j] = IMG_BLACK;
        }
    }
}

// ============================ ͼ��Գ��Է������� ============================
/**
  * ��    ����Get_Weighted_Symmetry
  * ��    �ܣ�����ָ��ROI����Ĵ�ֱ�Գ��ʣ���Ȩ�أ�
  * ��    ����x - ROI���Ͻ�X����
  *           y - ROI���Ͻ�Y����
  *           w - ROI���
  *           h - ROI�߶�
  * �� �� ֵ���Գ��ʣ�0.0~1.0����-1��ʾ��������
  * ��    ע����Ե����Ȩ�ظߣ��м�����Ȩ�ص�
  * ʾ    ����float sym = Get_Weighted_Symmetry(10, 10, 100, 50);
  */
float Get_Weighted_Symmetry(int x, int y, int w, int h)
{
    // ����У��
    if (x < 0 || y < 0 || w <= 0 || h <= 0 ||
        x + w > MT9V03X_W || y + h > MT9V03X_H)
    {
        return -1.0f;
    }
    
    // Ȩ�������ʼ��
    #define MAX_WIDTH 640
    static float weight[MAX_WIDTH];
    if (w > MAX_WIDTH)
        return -1.0f;
    
    static bool weights_initialized = false;
    if (!weights_initialized)
    {
        for (int j = 0; j < w; j++)
        {
            float pos = (float)j / (w - 1);
            if (pos < 0.2659f)      weight[j] = 0.9f;
            else if (pos < 0.4255f) weight[j] = 0.1f;
            else if (pos < 0.5691f) weight[j] = 0.05f;
            else if (pos < 0.7287f) weight[j] = 0.1f;
            else                    weight[j] = 0.9f;
        }
        weights_initialized = true;
    }
    
    // �����Ȩ�Գ���
    float weighted_original = 0.0f;
    float weighted_flipped = 0.0f;
    float weighted_matching = 0.0f;
    
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            float wj = weight[j];
            uint8_t orig = image_two_value[y + i][x + j];
            uint8_t flipped = image_two_value[y + i][x + (w - 1 - j)];
            
            if (orig) weighted_original += wj;
            if (flipped) weighted_flipped += wj;
            if (orig && flipped) weighted_matching += wj;
        }
    }
    
    // ����������
    float denominator = weighted_original + weighted_flipped;
    if (denominator < 1e-6f)
        return 0.0f;
    
    // ���㲢���ƽ����Χ
    float result = (2.0f * weighted_matching) / denominator;
    result = fmaxf(0.0f, fminf(1.0f, result));
    
    // �ضϵ�3λС��
    return floorf(result * 1000.f) / 1000.f;
}
    
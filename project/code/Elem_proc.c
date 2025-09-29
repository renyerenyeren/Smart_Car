#include "Elem_proc.h"

// ============================ 全局变量定义区 ============================
/** 二值化图像数组 */
uint8 image_two_value[MT9V03X_H][MT9V03X_W];//二值化后的图像数组

// ============================ 宏定义区 ============================
/** 图像颜色定义 */
#define IMG_BLACK     0X00      // 黑色像素值
#define IMG_WHITE     0XFF      // 白色像素值
#define GrayScale     256       // 灰度级范围（0-255）

// ============================ 画线与补线相关函数 ============================
/**
  * 函    数：Draw_Line
  * 功    能：在图像上绘制一条宽度为2的黑线
  * 参    数：startX - 起始点X坐标
  *           startY - 起始点Y坐标
  *           endX   - 终点X坐标
  *           endY   - 终点Y坐标
  * 返 回 值：无
  * 备    注：1. 自动进行坐标限幅处理，防止数组越界
  *           2. 支持水平线、垂直线和任意斜线绘制
  * 示    例：Draw_Line(0, 0, MT9V03X_W-1, MT9V03X_H-1);
  */
void Draw_Line(int startX, int startY, int endX, int endY)
{
    int i, x, y;
    int start = 0, end = 0;
    
    // 坐标限幅处理
    if (startX >= MT9V03X_W - 1) startX = MT9V03X_W - 1;
    else if (startX <= 0) startX = 0;
    if (startY >= MT9V03X_H - 6) startY = MT9V03X_H - 6;
    else if (startY <= 0) startY = 0;
    if (endX >= MT9V03X_W - 1) endX = MT9V03X_W - 1;
    else if (endX <= 0) endX = 0;
    if (endY >= MT9V03X_H - 6) endY = MT9V03X_H - 6;
    else if (endY <= 0) endY = 0;
    
    // 绘制竖线
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
    // 绘制横线
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
    // 绘制任意斜线
    else
    {
        // 纵向补线
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
        
        // 横向补线
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
  * 函    数：K_Draw_Line
  * 功    能：根据斜率和定点绘制黑线
  * 参    数：k        - 直线斜率
  *           startX   - 起始点X坐标
  *           startY   - 起始点Y坐标
  *           endY     - 终点Y坐标
  * 返 回 值：无
  * 备    注：通过斜率计算终点X坐标后调用Draw_Line实现绘制
  * 示    例：K_Draw_Line(k, 20, MT9V03X_H-1, 0);
  */
void K_Draw_Line(float k, int startX, int startY, int endY)
{
    int endX = 0;
    
    // 坐标限幅处理
    if (startX >= MT9V03X_W - 1) startX = MT9V03X_W - 1;
    else if (startX <= 0) startX = 0;
    if (startY >= MT9V03X_H - 6) startY = MT9V03X_H - 6;
    else if (startY <= 0) startY = 0;
    if (endY >= MT9V03X_H - 6) endY = MT9V03X_H - 6;
    else if (endY <= 0) endY = 0;
    
    // 计算终点X坐标 (y-y1)=k(x-x1) → x=(y-y1)/k + x1
    endX = (int)((endY - startY) / k + startX);
    Draw_Line(startX, startY, endX, endY);
}

// ============================ 赛道连续性检测函数 ============================
/**
  * 函    数：Continuity_Change_Left
  * 功    能：左赛道边界连续性检测
  * 参    数：start - 检测起始行
  *           end   - 检测终止行
  * 返 回 值：连续返回0，不连续返回断线所在行数
  * 备    注：1. 连续性阈值设置为7
  *           2. 当大部分区域丢线时直接返回1
  * 示    例：int flag = Continuity_Change_Left(10, 50);
  */
int Continuity_Change_Left(int start, int end)
{
    int i, t;
    int continuity_change_flag = 0;
    
    // 大部分区域丢线时直接返回
    if (lose_point_num_L >= 0.9 * MT9V03X_H)
        return 1;
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // 确保起始行大于终止行（从上往下检测）
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // 连续性检测
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
  * 函    数：Continuity_Change_Right
  * 功    能：右赛道边界连续性检测
  * 参    数：start - 检测起始行
  *           end   - 检测终止行
  * 返 回 值：连续返回0，不连续返回断线所在行数
  * 备    注：1. 连续性阈值设置为7
  *           2. 当大部分区域丢线时直接返回1
  * 示    例：int flag = Continuity_Change_Right(10, 50);
  */
int Continuity_Change_Right(int start, int end)
{
    int i, t;
    int continuity_change_flag = 0;
    
    // 大部分区域丢线时直接返回
    if (lose_point_num_R >= 0.9 * MT9V03X_H)
        return 1;
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // 确保起始行大于终止行（从上往下检测）
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // 连续性检测
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

// ============================ 角点检测相关函数 ============================
/**
  * 函    数：Find_Left_Down_Point
  * 功    能：左下方角点检测
  * 参    数：start - 检测起始行
  *           end   - 检测终止行
  * 返 回 值：角点所在行数，找不到返回0
  * 备    注：角点检测阈值可根据实际情况调整
  * 示    例：int line = Find_Left_Down_Point(10, 50);
  */
int Find_Left_Down_Point(int start, int end)
{
    int i, t;
    int left_down_line = 0;
    
    // 大部分区域丢线时直接返回
    if (lose_point_num_L >= 0.9 * MT9V03X_H)
        return left_down_line;
    
    // 坐标调整（确保start > end）
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // 角点检测
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
  * 函    数：Find_Left_Up_Point
  * 功    能：左上方角点检测
  * 参    数：start - 检测起始行
  *           end   - 检测终止行
  * 返 回 值：角点所在行数，找不到返回0
  * 备    注：角点检测阈值可根据实际情况调整
  * 示    例：int line = Find_Left_Up_Point(10, 50);
  */
int Find_Left_Up_Point(int start, int end)
{
    int i, t;
    int left_up_line = 0;
    
    // 大部分区域丢线时直接返回
    if (lose_point_num_L >= 0.9 * MT9V03X_H)
        return left_up_line;
    
    // 坐标调整（确保start > end）
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // 角点检测
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
  * 函    数：Find_Right_Down_Point
  * 功    能：右下方角点检测
  * 参    数：start - 检测起始行
  *           end   - 检测终止行
  * 返 回 值：角点所在行数，找不到返回0
  * 备    注：角点检测阈值可根据实际情况调整
  * 示    例：int line = Find_Right_Down_Point(10, 50);
  */
int Find_Right_Down_Point(int start, int end)
{
    int i, t;
    int right_down_line = 0;
    
    // 大部分区域丢线时直接返回
    if (lose_point_num_R >= 0.9 * MT9V03X_H)
        return right_down_line;
    
    // 坐标调整（确保start > end）
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // 角点检测
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
  * 函    数：Find_Right_Up_Point
  * 功    能：右上方角点检测
  * 参    数：start - 检测起始行
  *           end   - 检测终止行
  * 返 回 值：角点所在行数，找不到返回0
  * 备    注：角点检测阈值可根据实际情况调整
  * 示    例：int line = Find_Right_Up_Point(10, 50);
  */
int Find_Right_Up_Point(int start, int end)
{
    int i, t;
    int right_up_line = 0;
    
    // 大部分区域丢线时直接返回
    if (lose_point_num_R >= 0.9 * MT9V03X_H)
        return right_up_line;
    
    // 坐标调整（确保start > end）
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // 角点检测
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

// ============================ 单调性检测相关函数 ============================
/**
  * 函    数：Monotonicity_Change_Right_Land
  * 功    能：右边界单调性突变检测
  * 参    数：start - 检测起始列
  *           end   - 检测终止列
  * 返 回 值：突变点所在列数，找不到返回94
  * 备    注：通过比较前后5个点判断是否为极值点
  * 示    例：int col = Monotonicity_Change_Right_Land(10, 100);
  */
int Monotonicity_Change_Right_Land(int start, int end)
{
    int i;
    int monotonicity_change_column = 94;
    
    // 坐标越界保护
    if (end >= MT9V03X_W - 1)
        end = MT9V03X_W - 1;
    if (start <= 16)
        start = 16;
    
    // 单调性检测
    for (i = start; i <= end; i += 2)
    {
        // 排除连续相同值的情况
        if (boder_U[i] == boder_U[i+10] && boder_U[i] == boder_U[i-10] &&
            boder_U[i] == boder_U[i+8] && boder_U[i] == boder_U[i-8] &&
            boder_U[i] == boder_U[i+6] && boder_U[i] == boder_U[i-6] &&
            boder_U[i] == boder_U[i+4] && boder_U[i] == boder_U[i-4] &&
            boder_U[i] == boder_U[i+2] && boder_U[i] == boder_U[i-2])
        {
            continue;
        }
        // 判断是否为局部最大值
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
  * 函    数：Monotonicity_Change_Left
  * 功    能：左边界单调性突变检测
  * 参    数：start - 检测起始行
  *           end   - 检测终止行
  * 返 回 值：突变点所在行数，找不到返回0
  * 备    注：通过比较前后5个点判断是否为极值点
  * 示    例：int line = Monotonicity_Change_Left(10, 50);
  */
int Monotonicity_Change_Left(int start, int end)
{
    int i;
    int monotonicity_change_line = 0;
    
    // 大部分区域丢线时直接返回
    if (lose_point_num_L >= 0.9 * MT9V03X_H)
        return monotonicity_change_line;
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // 起始行必须大于终止行
    if (start <= end)
        return monotonicity_change_line;
    
    // 单调性检测
    for (i = start; i >= end; i--)
    {
        // 排除连续相同值的情况
        if (boder_L[i] == boder_L[i+5] && boder_L[i] == boder_L[i-5] &&
            boder_L[i] == boder_L[i+4] && boder_L[i] == boder_L[i-4] &&
            boder_L[i] == boder_L[i+3] && boder_L[i] == boder_L[i-3] &&
            boder_L[i] == boder_L[i+2] && boder_L[i] == boder_L[i-2] &&
            boder_L[i] == boder_L[i+1] && boder_L[i] == boder_L[i-1])
        {
            continue;
        }
        // 判断是否为局部最大值
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
  * 函    数：Monotonicity_Change_Right
  * 功    能：右边界单调性突变检测
  * 参    数：start - 检测起始行
  *           end   - 检测终止行
  * 返 回 值：突变点所在行数，找不到返回0
  * 备    注：通过比较前后5个点判断是否为极值点
  * 示    例：int line = Monotonicity_Change_Right(10, 50);
  */
int Monotonicity_Change_Right(int start, int end)
{
    int i;
    int monotonicity_change_line = 0;
    
    // 大部分区域丢线时直接返回
    if (lose_point_num_R >= 0.9 * MT9V03X_H)
        return monotonicity_change_line;
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // 起始行必须大于终止行
    if (start <= end)
        return monotonicity_change_line;
    
    // 单调性检测
    for (i = start; i >= end; i--)
    {
        // 排除连续相同值的情况
        if (boder_R[i] == boder_R[i+5] && boder_R[i] == boder_R[i-5] &&
            boder_R[i] == boder_R[i+4] && boder_R[i] == boder_R[i-4] &&
            boder_R[i] == boder_R[i+3] && boder_R[i] == boder_R[i-3] &&
            boder_R[i] == boder_R[i+2] && boder_R[i] == boder_R[i-2] &&
            boder_R[i] == boder_R[i+1] && boder_R[i] == boder_R[i-1])
        {
            continue;
        }
        // 判断是否为局部最小值
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

// ============================ 边界补线与延长函数 ============================
/**
  * 函    数：K_Add_Boundry_Left
  * 功    能：按斜率补充左边界
  * 参    数：k        - 边界斜率
  *           startX   - 起始点X坐标
  *           startY   - 起始点Y坐标
  *           endY     - 终止点Y坐标
  * 返 回 值：无
  * 备    注：补线结果直接更新到左边界数组boder_L
  * 示    例：K_Add_Boundry_Left(k, 20, 50, 10);
  */
void K_Add_Boundry_Left(float k, int startX, int startY, int endY)
{
    int i, t;
    
    // 坐标限幅处理
    if (startY >= MT9V03X_H - 6)
        startY = MT9V03X_H - 6;
    else if (startY <= 0)
        startY = 0;
    if (endY >= MT9V03X_H - 6)
        endY = MT9V03X_H - 6;
    else if (endY <= 0)
        endY = 0;
    
    // 确保起始行大于终止行
    if (startY < endY)
    {
        t = startY;
        startY = endY;
        endY = t;
    }
    
    // 按斜率计算并补充边界
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
  * 函    数：K_Add_Boundry_Right
  * 功    能：按斜率补充右边界
  * 参    数：k        - 边界斜率
  *           startX   - 起始点X坐标
  *           startY   - 起始点Y坐标
  *           endY     - 终止点Y坐标
  * 返 回 值：无
  * 备    注：补线结果直接更新到右边界数组boder_R
  * 示    例：K_Add_Boundry_Right(k, 100, 50, 10);
  */
void K_Add_Boundry_Right(float k, int startX, int startY, int endY)
{
    int i, t;
    
    // 坐标限幅处理
    if (startY >= MT9V03X_H - 6)
        startY = MT9V03X_H - 6;
    else if (startY <= 0)
        startY = 0;
    if (endY >= MT9V03X_H - 6)
        endY = MT9V03X_H - 6;
    else if (endY <= 0)
        endY = 0;
    
    // 确保起始行大于终止行
    if (startY < endY)
    {
        t = startY;
        startY = endY;
        endY = t;
    }
    
    // 按斜率计算并补充边界
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
  * 函    数：Get_Left_K
  * 功    能：计算左边界斜率
  * 参    数：start_line - 起始行
  *           end_line   - 终止行
  * 返 回 值：两点间的斜率
  * 备    注：默认起始行小于终止行，自动处理坐标顺序
  * 示    例：float k = Get_Left_K(10, 20);
  */
float Get_Left_K(int start_line, int end_line)
{
    // 坐标越界保护
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
    
    // 确保起始行小于终止行
    if (start_line > end_line)
    {
        t = start_line;
        start_line = end_line;
        end_line = t;
    }
    
    // 计算斜率
    k = (float)(boder_L[start_line] - boder_L[end_line]) / (end_line - start_line + 1);
    return k;
}

/**
  * 函    数：Get_Right_K
  * 功    能：计算右边界斜率
  * 参    数：start_line - 起始行
  *           end_line   - 终止行
  * 返 回 值：两点间的斜率
  * 备    注：默认起始行小于终止行，自动处理坐标顺序
  * 示    例：float k = Get_Right_K(10, 20);
  */
float Get_Right_K(int start_line, int end_line)
{
    // 坐标越界保护
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
    
    // 确保起始行小于终止行
    if (start_line > end_line)
    {
        t = start_line;
        start_line = end_line;
        end_line = t;
    }
    
    // 计算斜率
    k = (float)(boder_R[start_line] - boder_R[end_line]) / (end_line - start_line + 1);
    return k;
}

/**
  * 函    数：Lengthen_Left_Boundry
  * 功    能：延长左边界
  * 参    数：start - 延长起始行
  *           end   - 延长终止行
  * 返 回 值：无
  * 备    注：通过计算起始点附近斜率来延长边界
  * 示    例：Lengthen_Left_Boundry(20, 50);
  */
void Lengthen_Left_Boundry(int start, int end)
{
    int i, t;
    float k = 0;
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 6)
        end = MT9V03X_H - 6;
    else if (end <= 0)
        end = 0;
    
    // 确保终止行大于起始行
    if (end < start)
    {
        t = end;
        end = start;
        start = t;
    }
    
    // 起始点过小时直接连线
    if (start <= 5)
    {
        Left_Add_Line(boder_L[start], start, boder_L[end], end);
    }
    // 否则按斜率延长
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
  * 函    数：Lengthen_Right_Boundry
  * 功    能：延长右边界
  * 参    数：start - 延长起始行
  *           end   - 延长终止行
  * 返 回 值：无
  * 备    注：通过计算起始点附近斜率来延长边界
  * 示    例：Lengthen_Right_Boundry(20, 50);
  */
void Lengthen_Right_Boundry(int start, int end)
{
    int i, t;
    float k = 0;
    
    // 坐标越界保护
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    else if (start <= 0)
        start = 0;
    if (end >= MT9V03X_H - 6)
        end = MT9V03X_H - 6;
    else if (end <= 0)
        end = 0;
    
    // 确保终止行大于起始行
    if (end < start)
    {
        t = end;
        end = start;
        start = t;
    }
    
    // 起始点过小时直接连线
    if (start <= 5)
    {
        Right_Add_Line(boder_R[start], start, boder_R[end], end);
    }
    // 否则按斜率延长
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
  * 函    数：Left_Add_Line
  * 功    能：两点间补充左边界
  * 参    数：x1, y1 - 起点坐标
  *           x2, y2 - 终点坐标
  * 返 回 值：无
  * 备    注：直接更新左边界数组boder_L
  * 示    例：Left_Add_Line(20, 10, 50, 30);
  */
void Left_Add_Line(int x1, int y1, int x2, int y2)
{
    int i, max, a1, a2;
    int hx;
    
    // 坐标限幅处理
    if (x1 >= MT9V03X_W - 1) x1 = MT9V03X_W - 1;
    else if (x1 <= 0) x1 = 0;
    if (y1 >= MT9V03X_H - 6) y1 = MT9V03X_H - 6;
    else if (y1 <= 0) y1 = 0;
    if (x2 >= MT9V03X_W - 1) x2 = MT9V03X_W - 1;
    else if (x2 <= 0) x2 = 0;
    if (y2 >= MT9V03X_H - 6) y2 = MT9V03X_H - 6;
    else if (y2 <= 0) y2 = 0;
    
    // 确保a1 < a2
    a1 = y1;
    a2 = y2;
    if (a1 > a2)
    {
        max = a1;
        a1 = a2;
        a2 = max;
    }
    
    // 两点间线性插值补充边界
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
  * 函    数：Right_Add_Line
  * 功    能：两点间补充右边界
  * 参    数：x1, y1 - 起点坐标
  *           x2, y2 - 终点坐标
  * 返 回 值：无
  * 备    注：直接更新右边界数组boder_R
  * 示    例：Right_Add_Line(100, 10, 150, 30);
  */
void Right_Add_Line(int x1, int y1, int x2, int y2)
{
    int i, max, a1, a2;
    int hx;
    
    // 坐标限幅处理
    if (x1 >= MT9V03X_W - 1) x1 = MT9V03X_W - 1;
    else if (x1 <= 0) x1 = 0;
    if (y1 >= MT9V03X_H - 1) y1 = MT9V03X_H - 1;
    else if (y1 <= 0) y1 = 0;
    if (x2 >= MT9V03X_W - 1) x2 = MT9V03X_W - 1;
    else if (x2 <= 0) x2 = 0;
    if (y2 >= MT9V03X_H - 1) y2 = MT9V03X_H - 1;
    else if (y2 <= 0) y2 = 0;
    
    // 确保a1 < a2
    a1 = y1;
    a2 = y2;
    if (a1 > a2)
    {
        max = a1;
        a1 = a2;
        a2 = max;
    }
    
    // 两点间线性插值补充边界
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

// ============================ 图像二值化相关函数 ============================
/**
  * 函    数：my_adapt_threshold
  * 功    能：快速大津法求阈值
  * 参    数：image - 图像数组指针
  *           col   - 图像宽度
  *           row   - 图像高度
  * 返 回 值：计算得到的阈值
  * 备    注：比传统大津法计算速度更快
  * 示    例：int th = my_adapt_threshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
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
    
    // 初始化数组
    for (i = 0; i < GrayScale; i++)
    {
        pixelCount[i] = 0;
        pixelPro[i] = 0;
    }
    
    uint32 gray_sum = 0;
    
    // 统计灰度值分布
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            pixelCount[(int)data[i][j]]++;
            gray_sum += (int)data[i][j];
        }
    }
    
    // 计算灰度值比例
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    
    // 计算最大类间方差对应的阈值
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
  * 函    数：My_Adapt_Threshold
  * 功    能：普通大津法求阈值
  * 参    数：image  - 图像数组指针
  *           width  - 图像宽度
  *           height - 图像高度
  * 返 回 值：计算得到的阈值
  * 示    例：int th = My_Adapt_Threshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);
  */
int My_Adapt_Threshold(uint8* image, uint16 width, uint16 height)
{
    int pixelCount[GrayScale];
    float pixelPro[GrayScale];
    int i, j;
    int pixelSum = width * height / 4;
    int threshold = 0;
    uint8 (*data)[width] = (uint8 (*)[width])image;
    
    // 初始化数组
    memset(pixelCount, 0, sizeof(pixelCount));
    memset(pixelPro, 0, sizeof(pixelPro));
    
    uint32 gray_sum = 0;
    
    // 统计灰度值分布
    for (i = 0; i < height; i += 2)
    {
        for (j = 0; j < width; j += 2)
        {
            pixelCount[(int)data[i][j]]++;
            gray_sum += (int)data[i][j];
        }
    }
    
    // 计算灰度值比例
    for (i = 0; i < GrayScale; i++)
    {
        pixelPro[i] = (float)pixelCount[i] / pixelSum;
    }
    
    // 计算最大类间方差对应的阈值
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
    
    // 阈值范围限制
    if (threshold > 255)
        threshold = 255;
    
    return threshold;
}

/**
  * 函    数：Image_Binarization
  * 功    能：图像二值化处理
  * 参    数：threshold - 二值化阈值
  * 返 回 值：无
  * 备    注：结果存储在image_two_value数组中
  * 示    例：Image_Binarization(128);
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

// ============================ 图像对称性分析函数 ============================
/**
  * 函    数：Get_Weighted_Symmetry
  * 功    能：计算指定ROI区域的垂直对称率（带权重）
  * 参    数：x - ROI左上角X坐标
  *           y - ROI左上角Y坐标
  *           w - ROI宽度
  *           h - ROI高度
  * 返 回 值：对称率（0.0~1.0），-1表示参数错误
  * 备    注：边缘区域权重高，中间区域权重低
  * 示    例：float sym = Get_Weighted_Symmetry(10, 10, 100, 50);
  */
float Get_Weighted_Symmetry(int x, int y, int w, int h)
{
    // 参数校验
    if (x < 0 || y < 0 || w <= 0 || h <= 0 ||
        x + w > MT9V03X_W || y + h > MT9V03X_H)
    {
        return -1.0f;
    }
    
    // 权重数组初始化
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
    
    // 计算加权对称率
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
    
    // 避免除零错误
    float denominator = weighted_original + weighted_flipped;
    if (denominator < 1e-6f)
        return 0.0f;
    
    // 计算并限制结果范围
    float result = (2.0f * weighted_matching) / denominator;
    result = fmaxf(0.0f, fminf(1.0f, result));
    
    // 截断到3位小数
    return floorf(result * 1000.f) / 1000.f;
}
    
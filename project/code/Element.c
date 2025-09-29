#include "Element.h"

// ============================ 全局变量定义区 ============================
/** 十字检测相关变量 */
int Left_Down_Find = 0;       // 左下方拐点行数（十字检测用）
int Left_Up_Find = 0;         // 左上方拐点行数（十字检测用）
int Right_Down_Find = 0;      // 右下方拐点行数（十字检测用）
int Right_Up_Find = 0;        // 右上方拐点行数（十字检测用）

/** 环岛检测相关变量 */
int Island_State = 0;         // 环岛状态标志
int Left_Island_Flag = 0;     // 左环岛标志
int Right_Island_Flag = 0;    // 右环岛标志
int left_down_guai[2] = {0};  // 左下拐点坐标（[0]行, [1]列）
int right_down_guai[2] = {0}; // 右下拐点坐标（[0]行, [1]列）
int monotonicity_change_line[2] = {0};  // 单调性变化点坐标（[0]行, [1]列）
int monotonicity_change_left_flag = 0;  // 左边界单调性变化标志（不变化为0）
int monotonicity_change_right_flag = 0; // 右边界单调性变化标志（不变化为0）
int continuity_change_right_flag = 0;   // 右边界连续性标志（连续为0）
int continuity_change_left_flag = 0;    // 左边界连续性标志（连续为0）
int monotonicity_change_column[2] = {0};// 单调性变化点坐标（[0]行, [1]列）

// ============================ 始终线检测相关函数 ============================
/**
 * 函    数：find_start_finish_line
 * 功    能：检测始终线（斑马线）
 * 参    数：无
 * 返 回 值：0-未检测到始终线，1-检测到始终线
 * 备    注：通过检测灰度变化统计黑色块数量判断，阈值可调整
 */
#define GrayThreshold 75  // 灰度阈值界限（85-10），小于此值为黑点，大于为白点
uint8 find_start_finish_line()
{
    uint8 judge_state = 0;      // 判断状态
    uint8 black_block_num = 0;  // 统计斑马线黑色块数量
    
    // 非十字和环岛状态下才检测
    if (!(cross_flag || Island_State))
    {
        for (uint8 i = 10; i < MT9V03X_W - 10; i++)
        {
            switch (judge_state)
            {
                case 0:  // 初始状态，检测到白点进入下一状态
                    if (mt9v03x_image[50][i] > GrayThreshold)
                        judge_state = 1;
                    break;
                case 1:  // 检测到黑点进入下一状态
                    if (mt9v03x_image[50][i] < GrayThreshold)
                        judge_state = 2;
                    break;
                case 2:  // 检测到白点则完成一个黑白周期，计数加1
                    if (mt9v03x_image[50][i] > GrayThreshold)
                    {
                        judge_state = 1;
                        black_block_num++;
                    }
                    break;
            }
        }
    }
    
    // 黑色块数量大于5个判定为斑马线
    return (black_block_num > 5) ? 1 : 0;
}

// ============================ 十字检测相关函数 ============================
/**
 * 函    数：Find_Down_Point
 * 功    能：查找下方两个拐点（供十字检测用）
 * 参    数：start - 搜索范围起点
 *           end   - 搜索范围终点
 * 返 回 值：无（结果存储在全局变量Left_Down_Find和Right_Down_Find中）
 * 备    注：未找到时对应变量为0，找到时存储拐点所在行数
 */
void Find_Down_Point(int start, int end)
{
    int i, t;
    Right_Down_Find = 0;
    Left_Down_Find = 0;
    
    // 确保起点大于终点（从上往下搜索）
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // 边界保护
    if (start >= MT9V03X_H - 1 - 5)
        start = MT9V03X_H - 1 - 5;
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    
    // 搜索左右下拐点
    for (i = start; i >= end; i--)
    {
        // 检测左下拐点
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
        
        // 检测右下拐点
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
        
        // 找到两个拐点即退出
        if (Left_Down_Find != 0 && Right_Down_Find != 0)
            break;
    }
}

/**
 * 函    数：Find_Up_Point
 * 功    能：查找上方两个拐点（供十字检测用）
 * 参    数：start - 搜索范围起点
 *           end   - 搜索范围终点
 * 返 回 值：无（结果存储在全局变量Left_Up_Find和Right_Up_Find中）
 * 备    注：未找到时对应变量为0，纵向撕裂过大视为误判
 */
void Find_Up_Point(int start, int end)
{
    int i, t;
    Left_Up_Find = 0;
    Right_Up_Find = 0;
    
    // 确保起点大于终点（从上往下搜索）
    if (start < end)
    {
        t = start;
        start = end;
        end = t;
    }
    
    // 边界保护
    if (end <= 5)
        end = 5;
    if (end <= longest)
        end = longest;
    if (start >= MT9V03X_H - 6)
        start = MT9V03X_H - 6;
    
    // 搜索左右上拐点
    for (i = start; i >= end; i--)
    {
        // 检测左上拐点
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
        
        // 检测右上拐点
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
        
        // 找到两个拐点即退出
        if (Left_Up_Find != 0 && Right_Up_Find != 0)
            break;
    }
    
    // 纵向撕裂过大（>=30行）视为误判
    if (abs(Right_Up_Find - Left_Up_Find) >= 30)
    {
        Right_Up_Find = 0;
        Left_Up_Find = 0;
    }
}

/**
 * 函    数：Cross_Detect
 * 功    能：十字检测与补线处理
 * 参    数：无
 * 返 回 值：无
 * 备    注：利用四个拐点判别函数，根据找到的拐点个数进行补线处理
 */
void Cross_Detect()
{
    int down_search_start = 0;  // 下点搜索开始行
    cross_flag = 0;
    
    // 与环岛状态互斥
    if (Island_State == 0 && ramp_flag == 0)
    {
        Left_Up_Find = 0;
        Right_Up_Find = 0;
        
        // 双边丢线时间足够长时才开始找角点
        if (Both_Lost_Time >= 10)
        {
            Find_Up_Point(MT9V03X_H - 6, 0);
            // 未同时找到两个上点则直接返回
            if (Left_Up_Find == 0 && Right_Up_Find == 0)
                return;
        }
        
        // 找到两个上点则判定为十字
        if (Left_Up_Find != 0 && Right_Up_Find != 0)
        {
            cross_flag = 1;  // 设置十字标志位
            
            // 以下点搜索上限为两个上拐点中靠下的一个
            down_search_start = (Left_Up_Find > Right_Up_Find) ? 
                               Left_Up_Find : Right_Up_Find;
            Find_Down_Point(MT9V03X_H - 6, down_search_start + 2);
            
            // 过滤不合理的下拐点（下点不能比上点靠上）
            if (Left_Down_Find <= Left_Up_Find)
                Left_Down_Find = 0;
            if (Right_Down_Find <= Right_Up_Find)
                Right_Down_Find = 0;
            
            // 根据找到的拐点情况进行补线
            isopenResetM = 1;  // 开启重新计算中线
            if (Left_Down_Find != 0 && Right_Down_Find != 0)
            {
                // 四个点都找到，直接连线
                Left_Add_Line(boder_L[Left_Up_Find], Left_Up_Find,
                             boder_L[Left_Down_Find], Left_Down_Find);
                Right_Add_Line(boder_R[Right_Up_Find], Right_Up_Find,
                              boder_R[Right_Down_Find], Right_Down_Find);
            }
            else if (Left_Down_Find == 0 && Right_Down_Find != 0)
            {
                // 左下单点缺失，延长左边界
                Lengthen_Left_Boundry(Left_Up_Find - 1, MT9V03X_H - 6);
                Right_Add_Line(boder_R[Right_Up_Find], Right_Up_Find,
                              boder_R[Right_Down_Find], Right_Down_Find);
            }
            else if (Left_Down_Find != 0 && Right_Down_Find == 0)
            {
                // 右下单点缺失，延长右边界
                Left_Add_Line(boder_L[Left_Up_Find], Left_Up_Find,
                             boder_L[Left_Down_Find], Left_Down_Find);
                Lengthen_Right_Boundry(Right_Up_Find - 1, MT9V03X_H - 6);
            }
            else
            {
                // 两个下点都缺失，延长左右边界
                Lengthen_Left_Boundry(Left_Up_Find - 1, MT9V03X_H - 6);
                Lengthen_Right_Boundry(Right_Up_Find - 1, MT9V03X_H - 6);
            }
        }
        else
        {
            cross_flag = 0;
            isopenResetM = 0;  // 关闭重新计算中线
        }
    }
}

// ============================ 环岛检测相关函数 ============================
/**
 * 函    数：Island_Detect
 * 功    能：环岛检测与状态管理
 * 参    数：无
 * 返 回 值：无
 * 备    注：通过拐点判别、单调性变化和连续性检测实现，分为多个状态处理
 */
void Island_Detect()
{ 
    static float k = 0;  // 状态3和5使用的斜率
    
    // 常规判断：检测边界连续性和单调性
    continuity_change_left_flag = Continuity_Change_Left(MT9V03X_H - 6, 15);
    continuity_change_right_flag = Continuity_Change_Right(MT9V03X_H - 6, 15);
    monotonicity_change_right_flag = Monotonicity_Change_Right(MT9V03X_H - 1 - 10, 15);
    monotonicity_change_left_flag = Monotonicity_Change_Left(MT9V03X_H - 1 - 10, 15);
    
    // 非十字和坡道状态，且未进入环岛时进行检测
    if (cross_flag == 0 && Island_State == 0 && ramp_flag == 0)
    {
        // 左环岛检测
        if (Left_Island_Flag == 0)
        {
            if (monotonicity_change_right_flag == 0 &&       // 右边单调
                continuity_change_left_flag != 0 &&          // 左边不连续
                continuity_change_right_flag == 0 &&         // 右边连续
                lose_point_num_L >= 10 && lose_point_num_L <= 50 &&  // 左边丢线适中
                lose_point_num_R <= 15 &&                    // 右边丢线少
                longest <= 25 &&                             // 搜索截止行较远
                Boundry_Start_Left >= MT9V03X_H - 30 && 
                Boundry_Start_Right >= MT9V03X_H - 30 &&     // 边界起始点靠下
                Both_Lost_Time <= 8)                         // 双边丢线时间短
            {
                left_down_guai[0] = Find_Left_Down_Point(MT9V03X_H - 6, 20);
                // 拐点位置合理则判定为左环岛
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
        
        // 右环岛检测
        if (Right_Island_Flag == 0)
        {
            if (monotonicity_change_left_flag == 0 &&        // 左边单调
                continuity_change_left_flag == 0 &&          // 左边连续
                continuity_change_right_flag != 0 &&         // 右边不连续
                lose_point_num_R >= 10 && lose_point_num_R <= 50 &&  // 右边丢线适中
                lose_point_num_L <= 15 &&                    // 左边丢线少
                longest <= 15 &&                             // 搜索截止行较远
                Boundry_Start_Left >= MT9V03X_H - 30 && 
                Boundry_Start_Right >= MT9V03X_H - 30 &&     // 边界起始点靠下
                Both_Lost_Time <= 8)                         // 双边丢线时间短
            {
                right_down_guai[0] = Find_Right_Down_Point(MT9V03X_H - 6, 20);
                // 拐点位置合理则判定为右环岛
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
    
    // 左环岛处理逻辑
    if (Left_Island_Flag == 1 && get_push_flag() == 0)
    {
        switch (Island_State)
        {
            case 1:  // 状态1：拐点存在，未丢线
                monotonicity_change_line[0] = Monotonicity_Change_Left(30, 10);
                monotonicity_change_line[1] = boder_L[monotonicity_change_line[0]];
                left_down_guai[0] = Find_Left_Down_Point(MT9V03X_H - 6, 20);
                
                // 左下角开始丢线时进入状态2
                if (Boundry_Start_Left <= 80 && Boundry_Start_Left >= 35)
                {
                    Gyro_Angle.Zdata = 0;
                    Island_State = 2;
                }
                isopenResetM = 1;  // 开启重新计算中线
                Left_Add_Line((int)(monotonicity_change_line[1] * 0.15), 
                             MT9V03X_H - 6, 
                             monotonicity_change_line[1], 
                             monotonicity_change_line[0]);
                break;
                
            case 2:  // 状态2：下方丢线，上方即将出现大弧线
                monotonicity_change_column[0] = Boundry_Start_Left;
                monotonicity_change_column[1] = boder_L[Boundry_Start_Left];
                Draw_Line(boder_R[MT9V03X_H - 10], MT9V03X_H - 10,
                         monotonicity_change_column[1], monotonicity_change_column[0]);
                find_middle();
                isopenResetM = 0;  // 关闭重新计算中线
                
                // 陀螺仪角度足够大时进入状态4
                if (abs(Angle_now) >= 60)
                {
                    Island_State = 4;
                    Motor_Control_Mode = 0;
                }
                break;
                
            case 4:  // 状态4：完全进入环岛
                if (Angle_now >= 20)  // 积分角度足够后检测单调性变化
                {
                    monotonicity_change_line[0] = Monotonicity_Change_Left(MT9V03X_H - 10, 40);
                    monotonicity_change_line[1] = boder_L[monotonicity_change_line[0]];
                    
                    // 单调点靠下时进入状态5
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
                
            case 5:  // 状态5：准备出环岛
                Draw_Line(boder_R[MT9V03X_H - 10], MT9V03X_H - 10,
                         MT9V03X_W - 1, 0);
                find_middle();
                isopenResetM = 0;
                
                // 角度足够小时结束环岛
                if (abs(Angle_now) <= 25)
                {
                    Island_State = 9;
                    Left_Island_Flag = 0;
                    Motor_Control_Mode = 0;
                }
                break;
        }
    }
    // 右环岛处理逻辑
    else if (Right_Island_Flag == 1 && get_push_flag() == 0)
    {
        switch (Island_State)
        {
            case 1:  // 状态1：拐点存在，未丢线
                monotonicity_change_line[0] = Monotonicity_Change_Right(30, 10);
                monotonicity_change_line[1] = boder_R[monotonicity_change_line[0]];
                right_down_guai[0] = Find_Right_Down_Point(MT9V03X_H - 6, 20);
                
                // 右下角开始丢线时进入状态2
                if (Boundry_Start_Right <= 80 && Boundry_Start_Right >= 35)
                {
                    Gyro_Angle.Zdata = 0;
                    Island_State = 2;
                }
                isopenResetM = 1;  // 开启重新计算中线
                Right_Add_Line((int)(MT9V03X_W - 1 - (monotonicity_change_line[1] * 0.15)),
                              MT9V03X_H - 6,
                              monotonicity_change_line[1],
                              monotonicity_change_line[0]);
                break;
                
            case 2:  // 状态2：下方丢线，上方即将出现大弧线
                monotonicity_change_column[0] = Boundry_Start_Right;
                monotonicity_change_column[1] = boder_R[Boundry_Start_Right];
                Draw_Line(boder_L[MT9V03X_H - 10], MT9V03X_H - 10,
                         monotonicity_change_column[1], monotonicity_change_column[0]);
                find_middle();
                isopenResetM = 0;  // 关闭重新计算中线
                
                // 陀螺仪角度足够大时进入状态4
                if (abs(Angle_now) >= 60)
                {
                    Island_State = 4;
                    Motor_Control_Mode = 0;
                }
                break;
                
            case 4:  // 状态4：完全进入环岛
                if (Angle_now <= -20)  // 积分角度足够后检测单调性变化
                {
                    monotonicity_change_line[0] = Monotonicity_Change_Right(MT9V03X_H - 10, 40);
                    monotonicity_change_line[1] = boder_R[monotonicity_change_line[0]];
                    
                    // 单调点靠下时进入状态5
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
                
            case 5:  // 状态5：准备出环岛
                Draw_Line(boder_L[MT9V03X_H - 10], MT9V03X_H - 10,
                         MT9V03X_W - 1, 0);
                find_middle();
                isopenResetM = 0;
                
                // 角度足够小时结束环岛
                if (abs(Angle_now) <= 25)
                {
                    Island_State = 9;
                    Right_Island_Flag = 0;
                    Motor_Control_Mode = 0;
                }
                break;
        }
    }
    
    // 环岛结束状态处理
    if (Island_State == 9)
    {
        static uint8 i = 0;
        i++;
        if (i >= 25)
        {
            i = 0;
            Island_State = 0;
            isopenResetM = 0;  // 关闭重新计算中线
        }
    }
}
    
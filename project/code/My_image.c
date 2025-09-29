#include "My_image.h"


// ============================ 全局变量定义区 ============================
/** 图像特征点参数初始化 */
int Right_Guai[2] = {0};
int Left_Guai[2] = {0};          // 储存拐点坐标，[0]存y，[1]存x

/** 摄像头参数初始化 */
uint16 OBJECT_light = ART_Exposure_Time;    // 分类摄像头曝光时间
uint16 COLOR_light = ART_Exposure_Time;     // 找色块摄像头曝光时间
uint16 mt9v03X_light = Exposure_Time;       // MT9V03X摄像头曝光时间(默认200-130)

/** 边界检测参数初始化 */
int Left_Lost_Flag[MT9V03X_H] = {0};  // 左丢线数组，丢线置1，没丢线置0
int Right_Lost_Flag[MT9V03X_H] = {0}; // 右丢线数组，丢线置1，没丢线置0
uint8 boder_L[MT9V03X_H - 5] = {0};   // 左边扫线（左边界 黄色）
uint8 boder_R[MT9V03X_H - 5] = {0};   // 右边扫线（右边界 绿色）
uint8 boder_M[MT9V03X_H - 5] = {0};   // 中线 （蓝色）
uint8 boder_U[MT9V03X_W] = {0};       // 上边扫线（上边线 红色）
uint8 boder_correct[60] = {0};        // 用于边线矫正的采样数据点（y坐标）
uint8 longest = 0, index = 0;         // 最长白列的顶点对应的纵坐标和横坐标
uint8 shortest = 0, index_shortest = 0;
uint8 cross_flag = 0, ramp_flag = 0;  // 坡道十字的标志位，1为到达此处
uint8 lose_point_num_L = 0, lose_point_num_R = 0;
uint8 Both_Lost_Time = 0;             // 左、右边线丢线点数及双边丢线次数
int Boundry_Start_Left = 0;
int Boundry_Start_Right = 0;          // 左右边界起始行

/** 控制模式参数初始化 */
uint8 Slope_Mode = 0;                       // 计算斜率的模式
uint8 isopenResetM = 0;                     // 中线重置开关
uint8 found_left = 0, found_right = 0;      // 标记是否找到起始点

/** 环岛检测相关初始化 */
int start_column_Right_Island = 0;

/** 图像数据参数 */
uint8 white_value = 120;                    // 参考白点默认值

// ============================ 初始化相关函数 ============================
/**
  * 函    数：my_image_init
  * 功    能：初始化图像传感器并设置曝光时间
  * 参    数：无
  * 返 回 值：无
  * 备    注：1. 初始化失败会持续重试，直到成功为止；
  *           2. 初始化完成后延迟100ms确保稳定
  */
void my_image_init()
{
    // 初始化MT9V03X摄像头
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
    
    // 设置摄像头曝光时间
    while (1)
    {
        if (mt9v03x_set_exposure_time(mt9v03X_light))  // 默认120-100
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

// ============================ 图像处理相关函数 ============================
/**
  * 函    数：find_white_point
  * 功    能：计算图像中白色参考阈值
  * 参    数：image_array[][188] - 输入图像数组
  * 返 回 值：uint8 - 白色参考阈值
  * 备    注：1. 对特定区域(10列×2行)进行采样计算平均值；
  *           2. 过滤灰度值低于55的像素点，避免干扰；
  *           3. 无有效采样点时返回默认值120
  */
uint8 find_white_point(uint8 image_array[][188])
{
    uint16 total = 0, cnt = 0;
    // 对特定区域进行采样（10列×2行）
    for (uint8 i = 0; i < 20; i += 2)
    {
        for (uint8 j = 0; j < 8; j += 4)
        {
            // 过滤掉灰度太低的像素点（阈值55）
            if (image_array[84 + i][100 + j] > 55)
            {
                cnt++;
                total += image_array[84 + i][100 + j];
            }
        }
    }
    // 若有有效采样点则返回平均值，否则返回默认值120
    return (cnt) ? (total / cnt) : 120;
}

/**
  * 函    数：find_longest
  * 功    能：查找图像中最长白列（Y值最小的白列）
  * 参    数：longest - 最长白列Y坐标（输出参数）
  * 参    数：index - 最长白列X索引（输出参数）
  * 返 回 值：无
  * 备    注：1. 支持环岛场景下的搜索范围动态调整；
  *           2. 满屏白列情况下取平均X坐标作为结果；
  *           3. 结果用于后续边界检测的基准点
  */
void find_longest(uint8* longest, uint8* index)
{
    uint8 high_l = 0, high_r = 0, highest_count = 0;  // 满屏白列参数
    uint16 high_sum = 0;                              // 满屏白列X坐标总和
    *longest = MT9V03X_H - 10;                        // 初始化最长白列Y值
    shortest = 10;                                    // 初始化最短白列Y值
    
    // 动态计算白色参考值（加权平均）
    white_value = 0.7f * 130 + 0.3f * find_white_point(mt9v03x_image);
    // tft180_show_int(188, 90, white_value, 3);

    // 根据环岛状态调整搜索范围
    int start_column = 0;
    int end_column = MT9V03X_W;
    
    if (Right_Island_Flag == 1 && Island_State == 3)  // 右环场景
    {
        start_column = start_column_Right_Island;
        end_column = MT9V03X_W;
    }
    else if (Left_Island_Flag == 1 && Island_State == 3)  // 左环场景
    {
        start_column = 0;
        end_column = MT9V03X_W - 20;
    }
    
    // 逐列扫描寻找最长白列
    for (uint8 i = start_column; i < end_column; i += 2)
    {
        for (int16 j = MT9V03X_H - 6; j >= 0; j--)
        {
            int16 a = j;
            int16 b = a + 4;

            // 计算与白色参考值的对比度
            uint8 value_2 = 100 * abs(mt9v03x_image[a][i] - white_value) / 
                           (mt9v03x_image[a][i] + white_value + 1);
            
            // 检测满屏白列（顶部为白色）
            if (value_2 < Y_THRESHOLD && a <= 2)
            {
                highest_count++;
                high_sum += i;
                // tft180_draw_point(i, j, RGB565_RED);
            }
            
            // 检测白列边界（对比度超过阈值）
            if (value_2 > Y_THRESHOLD)
            {
                // tft180_draw_point(i, j, RGB565_RED);
                boder_U[i] = j;  // 保存上边线坐标

                // 中线附近的点用于边界矫正
                if (i >= middle - 30 && i < middle + 30)
                {
                    boder_correct[i - middle + 30] = (j < y_MIN) ? y_MIN : j;
                }

                // 更新最长白列
                if (j < *longest)
                {
                    *longest = j;
                    *index = i;
                }
                
                break;  // 找到边界后退出当前列扫描
            }
        }
    }

    // 满屏白列处理（取平均X坐标）
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
  * 函    数：find_middle
  * 功    能：计算图像中的中线和左右边界
  * 参    数：无
  * 返 回 值：无
  * 备    注：1. 从图像底部向上逐行处理，计算边界并统计丢线情况；
  *           2. 最长白列过短时（>SHORTEST）直接返回，认为跑出赛道；
  *           3. 记录首个非丢线点作为边界起始行，用于后续控制
  */
void find_middle()
{
    // 初始化状态变量
    lose_point_num_L = 0;
    lose_point_num_R = 0;
    Both_Lost_Time = 0;
    Boundry_Start_Left = 0;
    Boundry_Start_Right = 0;
    found_left = 0;
    found_right = 0;

    // 先找到最长白列作为基准
    find_longest(&longest, &index);
    if (longest > SHORTEST)  // 最长白列过短，认为跑出赛道
    {
        return;
    }

    // 从图像底部向上逐行处理
    for (int16 i = MT9V03X_H - 6; i >= 0; i--)
    {
        // --- 左边界查找 ---
        Left_Lost_Flag[i] = 0;  // 默认不丢线
        for (int16 j = index; j >= 0; j--)
        {
            if (j < 5)  // 靠近左边缘，判定为丢线
            {
                boder_L[i] = 0;
                Left_Lost_Flag[i] = 1;
                break;
            }
            
            // 计算横向对比度
            uint8 a = j;
            uint8 b = j - 5;
            uint8 value_1 = 100 * abs(mt9v03x_image[i][a] - mt9v03x_image[i][b]) / 
                           (mt9v03x_image[i][a] + mt9v03x_image[i][b] + 1);
            uint8 value_2 = 100 * abs(mt9v03x_image[i][b] - white_value) / 
                           (mt9v03x_image[i][b] + white_value + 1);
            
            // 对比度加权判定边界
            if (value_1 * 0.6f + value_2 * 0.4f > X_THRESHOLD)
            {
                boder_L[i] = b;
                break;
            }
        }

        // --- 右边界查找 ---
        Right_Lost_Flag[i] = 0;  // 默认不丢线
        for (int16 j = index; j < MT9V03X_W; j++)
        {
            if (j > MT9V03X_W - 6)  // 靠近右边缘，判定为丢线
            {
                boder_R[i] = MT9V03X_W - 1;
                Right_Lost_Flag[i] = 1;
                break;
            }
            
            // 计算横向对比度
            uint8 a = j;
            uint8 b = j + 5;
            uint8 value_1 = 100 * abs(mt9v03x_image[i][a] - mt9v03x_image[i][b]) / 
                           (mt9v03x_image[i][a] + mt9v03x_image[i][b] + 1);
            uint8 value_2 = 100 * abs(mt9v03x_image[i][b] - white_value) / 
                           (mt9v03x_image[i][b] + white_value + 1);
            
            // 对比度加权判定边界
            if (value_1 * 0.6f + value_2 * 0.4f > X_THRESHOLD)
            {
                boder_R[i] = b;
                break;
            }
        }

        // 计算中线（左右边界中点）
        boder_M[i] = (boder_L[i] + boder_R[i]) / 2;

        // 统计丢线情况
        if (Left_Lost_Flag[i]) lose_point_num_L++;
        if (Right_Lost_Flag[i]) lose_point_num_R++;
        if (Left_Lost_Flag[i] && Right_Lost_Flag[i]) Both_Lost_Time++;

        // 记录首个非丢线点（从底部向上）
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

        // 计算路宽（注释掉的备用代码）
        // Road_Wide[i] = boder_R[i] - boder_L[i];
    }

    // 边界起始点兜底处理（未找到时默认底部）
    if (!found_left) Boundry_Start_Left = MT9V03X_H - 6;
    if (!found_right) Boundry_Start_Right = MT9V03X_H - 6;
}

// ============================ 辅助函数 ============================
/**
  * 函    数：show_boder_line
  * 功    能：在屏幕上显示边界线（调试用）
  * 参    数：无
  * 返 回 值：无
  * 备    注：1. 左边界显示为黄色，右边界为绿色，中线为黑色；
  *           2. 正常运行时建议关闭，以节省系统资源；
  *           3. 需在主循环中调用才能持续显示
  */
void show_boder_line()
{
    for (uint8 i = longest; i < MT9V03X_H - 5; i++)
    {
        ips200_draw_point(boder_L[i], i, RGB565_YELLOW);  // 左边界-黄色
        ips200_draw_point(boder_R[i], i, RGB565_GREEN);   // 右边界-绿色
        ips200_draw_point(boder_M[i], i, RGB565_BLACK);   // 中线-黑色
    }
}


// ============================ 斜率计算函数 ============================
/**
  * 函    数：slope
  * 功    能：计算图像边界的斜率（用于控制小车转向）
  * 参    数：slope_mode - 斜率计算模式（SLOPE_MIDDLE/SLOPE_LEFT/SLOPE_RIGHT）
  * 返 回 值：int16 - 计算得到的斜率值（已限幅处理）
  * 备    注：1. 中线模式下，最长白列过短时返回上次结果；
  *           2. 斜率计算采用加权求和方式，底部权重更高；
  *           3. 结果范围被限制在±80之间
  */
int16 slope(uint8 slope_mode)
{
    static int16 lastresult = 0;  // 保存上一次结果用于异常处理

    // 中线重置处理
    if (isopenResetM == 1)
    {
        for (uint8 i = longest; i < MT9V03X_H - 5; i++)
        {
            boder_M[i] = (boder_L[i] + boder_R[i]) / 2;
        }
    }

    // 中线巡线模式
    if (slope_mode == SLOPE_MIDDLE)
    {
        if (longest > SHORTEST)  // 超出最短阈值，返回上次结果
        {
            return lastresult;
        }

        int16 sum1 = 0, sum2 = 0, result;
        
        // 根据白列长度选择不同的采样范围
        if (longest < 15 && (cross_flag * Island_State) == 0)
        {
            for (uint8 i = longest + 30; i <= MT9V03X_H - 10; i++)
            {
                sum1 += (int16)(boder_M[i] - middle);  // 与中线的偏差
                sum2 += (int16)(MT9V03X_H - 10 - i);   // 权重（距离底部的距离）
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
        
        // 计算斜率（防止除零）
        result = 50 * sum1 / (sum2 + 1);
        result = func_limit(result, 80);  // 限幅处理
        lastresult = result;
        
        return result;
    }
    // 左边线巡线模式
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
    // 右边线巡线模式
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
    // 无效模式
    else
    {
        return 0;
    }
}

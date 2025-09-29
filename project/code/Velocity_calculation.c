#include "Velocity_calculation.h"

// ============================ 全局变量定义区 ============================
/** 速度控制相关参数初始化 */
float V = 0.0f;                // 期望合速度大小初始化为0
float theta = 0.0f;            // 速度方向夹角初始化为0度
float w = 0.0f;                // 车身旋转角速度初始化为0
float Vx = 0.0f;               // X轴分速度初始化为0     //###使用后记得转换为V和theta###
float Vy = 0.0f;               // Y轴分速度初始化为0

/** 位移解算相关参数初始化 */
bool calculate_flag = false;   // 默认关闭位移解算（重置状态）
float shift_yaw = 0.0f;        // 累计偏航角度初始化为0度
float shift_distance = 0.0f;   // 累计总位移初始化为0
float x_distance = 0.0f;       // X轴累计位移初始化为0
float y_distance = 0.0f;       // Y轴累计位移初始化为0

// ============================ 速度解算相关函数 ============================
/**
  * 函    数：calculateSpeeds
  * 功    能：根据运动模式计算三轮速度
  * 参    数：mode    - 运动模式（NORMAL/TURN/CORRECT）
  * 参    数：V       - 期望合速度大小（无单位，需与电机控制匹配）
  * 参    数：theta   - 速度方向夹角（单位：度，与基准方向的夹角）
  * 参    数：w       - 期望角速度（无单位，转向模式下生效）
  * 参    数：V1      - 轮1速度（输出参数，整数型电机控制值）
  * 参    数：V2      - 轮2速度（输出参数，整数型电机控制值）
  * 参    数：V3      - 轮3速度（输出参数，整数型电机控制值）
  * 返 回 值：无
  * 备    注：1. 转向/校正模式下V3不含角速度补偿，正常模式含补偿；
  *           2. 最终速度值经浮点计算后强制转换为整数
  */
void calculateSpeeds(MotionMode mode, float V, float theta, float w, int* V1, int* V2, int* V3) 
{
    float L_parameter = 1.0f;  // 轮1角速度加权参数（默认1.0，无加权）
    float R_parameter = 1.0f;  // 轮2角速度加权参数（默认1.0，无加权）
    
    // 根据运动模式配置加权参数
    switch (mode) 
    {
        case TURN:
            if (w > 0)  { L_parameter = 0.6f; R_parameter = 1.4f; } 
            else        { R_parameter = 0.6f; L_parameter = 1.4f; }
            break;
            
        case CORRECT:
            if (w > 0)  { L_parameter = 0.85f; R_parameter = 1.15f; } 
            else        { R_parameter = 0.85f; L_parameter = 1.15f; }
            break;
            
        case NORMAL:
        default:
            L_parameter = 1.0f;
            R_parameter = 1.0f;
            break;
    }
    
    // 计算各轮速度（含方向分量+角速度补偿）
    *V1 = (int)((2.0f / 3.0f) * cos((30.0f - theta) * PI / 180.0f) * V + L_parameter * L * w);
    *V2 = (int)((2.0f / 3.0f) * cos((150.0f - theta) * PI / 180.0f) * V + R_parameter * L * w);
    // V3仅正常模式含角速度补偿
    *V3 = (int)((2.0f / 3.0f) * cos((270.0f - theta) * PI / 180.0f) * V + (mode == NORMAL ? L * w : 0.0f));
}

/**
  * 函    数：calculate_angle_with_y_axis
  * 功    能：将x/y速度分量转换为合速度及与y轴夹角
  * 参    数：x      - x方向速度分量（无单位，与输入速度匹配）
  * 参    数：y      - y方向速度分量（无单位，与输入速度匹配）
  * 参    数：V      - 合速度大小（输出参数，无单位）
  * 参    数：theta  - 与y轴正轴夹角（输出参数，单位：度，范围[-180, 180]）
  * 返 回 值：无
  * 备    注：1. 角度定义：顺时针为正，y轴正轴为0度；
  *           2. 当x、y均为0时，theta默认返回0（原点无有效方向）
  */
void calculate_angle_with_y_axis(float x, float y, float *V, float *theta) 
{
    *V = (float)sqrt(x*x + y*y);
    
    if (x == 0 && y == 0) 
    {
        *theta = 0.0f;  // 原点特殊处理：返回默认角度0
        return;
    }

    // 1. 计算x轴正轴为基准的弧度角
    double angle_rad = atan2(y, x);
    // 2. 转换为y轴正轴为基准，顺时针为正
    double angle_rad_adjusted = - (angle_rad - PI / 2.0);
    // 3. 弧度转角度并归一化到[-180, 180]
    double angle_deg = angle_rad_adjusted * (180.0 / PI);
    angle_deg = (angle_deg > 180.0f) ? (angle_deg - 360.0f) : (angle_deg < -180.0f ? angle_deg + 360.0f : angle_deg);

    *theta = (float)angle_deg;
}

// ============================ 运动控制相关函数 ============================
/**
  * 函    数：turn_around
  * 功    能：设置小车旋转运动参数
  * 参    数：R        - 旋转半径（无单位，需与机械结构匹配）
  * 参    数：direction - 旋转方向（-1：逆时针；1：顺时针）
  * 参    数：want_Vx   - 期望线速度（无单位，旋转时的切线速度）
  * 返 回 值：无
  * 备    注：1. 计算结果存储于全局变量Vx（线速度）和w（角速度）；
  *           2. 方向映射：向左推→逆时针（-1），向右推→顺时针（1）
  */
void turn_around(float R, int8_t direction, float want_Vx, float *Vx, float *w)
{     
    *Vx = want_Vx * direction * (-1.0f);
    // 计算角速度（v = ωR → ω = v/R）
    *w = (want_Vx / R) * direction;
}

/**
  * 函    数：move
  * 功    能：设置小车直线移动参数
  * 参    数：angle  - 移动方向夹角（单位：度，与基准方向的夹角）
  * 参    数：speed  - 移动速度大小（无单位，需与电机控制匹配）
  * 返 回 值：无
  * 备    注：参数存储于全局变量theta（角度）和V（速度），供速度解算函数调用
  */
void move(int16_t angle, int8_t speed)
{
    theta = (float)angle;
    V = (float)speed;
}

// ============================ 位移解算相关函数 ============================
/**
  * 函    数：translate_shift
  * 功    能：解算平动位移、偏航角度及总位移
  * 参    数：motor_1_speed - 电机1速度（编码器读数，无单位）
  * 参    数：motor_2_speed - 电机2速度（编码器读数，无单位）
  * 参    数：motor_3_speed - 电机3速度（编码器读数，无单位）
  * 返 回 值：无
  * 备    注：1. 依赖全局标志位calculate_flag：true-积分计算，false-重置；
  *           2. 积分公式：位移 = 速度 × 时间 × 校正系数
  */
void translate_shift(int motor_1_speed, int motor_2_speed, int motor_3_speed)
{
    if (calculate_flag)
    {
        // 1. 计算x、y方向速度分量（基于电机速度合成）
        float x_speed = (motor_1_speed * cos60 + motor_2_speed * cos60 - motor_3_speed);
        float y_speed = (motor_1_speed * sin60 - motor_2_speed * sin60);
        
        // 2. 位移积分（采样时间×校正系数，将编码器读数转换为位移）
        x_distance += x_speed * IT_MS * CORRECTION;
        y_distance += y_speed * IT_MS * CORRECTION;
        
        // 3. 计算偏航角度（基于x/y位移的反正切）
        shift_yaw = RADtoDEG(atan(x_distance / y_distance));
        // 4. 象限修正（atan默认范围[-90,90]，扩展到全象限）
        if (x_distance > 0 && y_distance < 0)
        {
            shift_yaw += 180.0f;
        }
        else if (x_distance < 0 && y_distance < 0)
        {
            shift_yaw -= 180.0f;
        }
        
        // 5. 计算总位移（欧氏距离+最终修正）
        shift_distance = sqrt(x_distance * x_distance + y_distance * y_distance) * CACULATE_REVISE;
    }
    else
    {
        // 重置所有位移数据
        x_distance = 0.0f;
        y_distance = 0.0f;
        shift_yaw = 0.0f;
        shift_distance = 0.0f;
    }
}
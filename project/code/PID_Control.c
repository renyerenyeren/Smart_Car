#include "PID_Control.h" 

// ============================ 全局变量定义区 ============================
/** 控制模式与状态标志初始化 */
bool SPEED_ENABLE = false;                         // 速度开关标志位
float Target_slope = 0;                            // 目标斜率

/** 速度相关参数初始化 */
int16 Speed_factory = DEFAULT_SPEED;
int16 tracking_speed;                              // 循迹速度，y方向
int16 last_speed;                                  // 循迹模式下的速度一阶滤波
int speed_compensation_offset = 10;                // 速度补偿偏移量（用于补偿机械摩擦等）

/** 运动控制参数初始化 */
int8 rotation_direction;                           // 车身旋转方向（-1：逆时针，1：顺时针）
int8 tangential_linear_speed;                      // 旋转时的切向线速度

/** 电机控制参数初始化 */
int Read_wheelspeed[3] = {0, 0, 0};                // 读取的三轮速度
int wheel[3] = {0};                                // 解算的三个轮子相应的目标速度
uint8_t Motor_Control_Mode;                        // 控制模式变量

// ============================ 函数定义区 ============================
// 冲刺冲！！！！！！！
void rushrushrush(void)
{
	Speed_factory = SPEED_FASTER;
}	



/**
  * 函    数：Motor_Control
  * 功    能：电机控制主函数，根据不同控制模式计算电机输出
  * 参    数：无
  * 返 回 值：无
  * 备    注：1. 支持多种控制模式：循迹、转向、位置校正等；
  *           2. 最终通过速度环PID控制电机输出；
  *           3. 依赖SPEED_ENABLE标志位控制是否实际输出
  */
void Motor_Control(void)
{
    static int Last_Now_Angle_X;
    
    // 根据控制模式计算期望速度和角速度
    switch (Motor_Control_Mode)
    {
        case 0:  // 基本循迹模式
            w = turnPID_Calculate(&hturn_pid, Slope, Target_slope);
            break;
            
        case 1:  // 等待模式
            w = 0;
            move(0, 0);
            break;
            
        case 2:  // 控制车身到目标角度
            w = StdPID_Calculate(&hangle_pid, Angle_now, angle_before_turn + angle_turn);
            break;
            
        case 9:  // 旋转运动模式
            Vy = 0;
            turn_around(DEFAULT_TURNING_RADIUS, rotation_direction, tangential_linear_speed, &Vx, &w);
            calculate_angle_with_y_axis(Vx, Vy, &V, &theta);  // 坐标转换
            break;
            
        case 16: // 位置保持模式（含横向约束）
            if (Now_Position_Y)
            {
                Vy = StdPID_Calculate(&hposy_keep_pid, Now_Position_Y, TARGET_POSITION_Y);
                turn_around(DEFAULT_TURNING_RADIUS, rotation_direction, tangential_linear_speed, &Vx, &w);
                Vx += StdPID_Calculate(&hposx_keep_pid, Now_Angle_X, TARGET_ANGLE_X);  // 横向约束
            }
            else
            {
                Vy = 0;
                turn_around(DEFAULT_TURNING_RADIUS, rotation_direction, tangential_linear_speed, &Vx, &w);
            }
            calculate_angle_with_y_axis(Vx, Vy, &V, &theta);  // 坐标转换
            break;
            
        case 12: // 方块位置矫正模式（侧面对齐）
            Vx = StdPID_Calculate(&hposx_keep_pid, Now_Angle_X, TARGET_ANGLE_X);  // 横向约束
            Vy = 0;
            calculate_angle_with_y_axis(Vx, Vy, &V, &theta);  // 坐标转换
            w = StdPID_Calculate(&hangle_pid, Angle_now, angle_before_turn + angle_turn);
            break;
            
        case 14: // 自适应位置校正模式
            if (Now_Position_Y)
            {
                V = AdaptivePID_Calculate(&hadaptive_posy_pid, Now_Position_Y, TARGET_POSITION_Y);
                w = AdaptivePID_Calculate(&hadaptive_anglex_pid, Now_Angle_X, TARGET_ANGLE_X);  // 车身方向矫正
                Last_Now_Angle_X = Now_Angle_X;
            }
            else
            {
                V = last_speed * 0.5f;
                w = AdaptivePID_Calculate(&hadaptive_anglex_pid, Now_Angle_X, TARGET_ANGLE_X) * 1.5;
            }
            theta = 0;
            break;
            
        case 15: // 降速循迹模式
            w = turnPID_Calculate(&hturn_pid, Slope, Target_slope);
            break;
    }
    
    // 根据控制模式计算各轮目标速度
    switch (Motor_Control_Mode)
    {
        case 0:  // 普通循迹模式
        {
            float tracking_speed = Speed_factory / (100.0f + fabsf(Slope));
            w *= tracking_speed / 32.5f;
            
            V = 0.7 * tracking_speed + 0.3 * last_speed + speed_compensation_offset;
            last_speed = V;
            theta = 0;
            calculateSpeeds(TURN, V, theta, w, &wheel[0], &wheel[1], &wheel[2]);
            break;
        }
        
        case 14: // 自适应校正模式
        {
            calculateSpeeds(CORRECT, V, theta, w, &wheel[0], &wheel[1], &wheel[2]);
            break;
        }
        
        case 15: // 降速循迹模式
        {
            tracking_speed = 4500 / (100 + abs(Slope));
            w *= (float)tracking_speed / 25;
            
            V = 0.7 * tracking_speed + 0.3 * last_speed;
            last_speed = V;
            theta = 0;
            calculateSpeeds(TURN, V, theta, w, &wheel[0], &wheel[1], &wheel[2]);
            break;
        }
        
        default: // 其他全向运动模式
        {
            calculateSpeeds(NORMAL, V, theta, w, &wheel[0], &wheel[1], &wheel[2]);
        }
    }
    
    // 根据速度使能标志控制电机实际输出
    if (SPEED_ENABLE)
    {
        motor_set_duty(MOTOR_NUM_1, VelocityPID_Calculate(&hvel_pid[0], Read_wheelspeed[0], wheel[0]));
        motor_set_duty(MOTOR_NUM_2, VelocityPID_Calculate(&hvel_pid[1], Read_wheelspeed[1], wheel[1]));
        motor_set_duty(MOTOR_NUM_3, VelocityPID_Calculate(&hvel_pid[2], Read_wheelspeed[2], wheel[2]));
    }
}

/**
  * 函    数：pit_handler_0
  * 功    能：PIT定时器中断处理函数
  * 参    数：无
  * 返 回 值：无
  * 备    注：1. 读取编码器速度；
  *           2. 进行位移解算；
  *           3. 获取灰度传感器数据；
  *           4. 执行电机控制
  */
void pit_handler_0(void)
{
    // 读取三轮编码器速度
    Read_wheelspeed[0] = Encoder_ReadSpeed(ENCODER_NUM_1);
    Read_wheelspeed[1] = Encoder_ReadSpeed(ENCODER_NUM_2);
    Read_wheelspeed[2] = Encoder_ReadSpeed(ENCODER_NUM_3);
    
    // 位移解算
    translate_shift(Read_wheelspeed[0], Read_wheelspeed[1], Read_wheelspeed[2]);
    
    // 获取灰度传感器数据
    Gray_level = Gray_Get();
    
    // 执行电机控制
    Motor_Control();
}
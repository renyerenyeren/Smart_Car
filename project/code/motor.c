#include "motor.h"

// ============================ 静态数据定义区 ============================
/**
 * 电机硬件参数结构体数组
 * 将每个电机的PWM引脚、方向引脚与编号绑定，通过枚举索引直接访问
 */
typedef struct {
    pwm_channel_enum pwm_pin;   // PWM输出引脚
    gpio_pin_enum    dir_pin;   // 方向控制引脚
} MotorHardware;

static const MotorHardware g_motor_hw[MOTOR_COUNT] = {
    {MOTOR1_PWM_PIN, MOTOR1_DIR_PIN},  // 电机1：绑定PWM和方向引脚
    {MOTOR2_PWM_PIN, MOTOR2_DIR_PIN},  // 电机2：绑定PWM和方向引脚
    {MOTOR3_PWM_PIN, MOTOR3_DIR_PIN}   // 电机3：绑定PWM和方向引脚
};

// ============================ 电机初始化函数 ============================
/**
  * 函    数：motor_Init
  * 功    能：初始化所有电机的PWM通道和方向控制引脚
  * 参    数：无
  * 返 回 值：无
  * 备    注：1. 按电机编号顺序初始化，统一使用MOTOR_PWM_FREQ频率；
  *           2. 方向引脚默认初始为高电平（与电机硬件转向匹配）
  */
void motor_Init(void)
{
    // 初始化电机1：PWM + 方向引脚
    pwm_init(g_motor_hw[MOTOR_NUM_1].pwm_pin, MOTOR_PWM_FREQ, MOTOR_INIT_DUTY);
    gpio_init(g_motor_hw[MOTOR_NUM_1].dir_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    
    // 初始化电机2：PWM + 方向引脚
    pwm_init(g_motor_hw[MOTOR_NUM_2].pwm_pin, MOTOR_PWM_FREQ, MOTOR_INIT_DUTY);
    gpio_init(g_motor_hw[MOTOR_NUM_2].dir_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    
    // 初始化电机3：PWM + 方向引脚
    pwm_init(g_motor_hw[MOTOR_NUM_3].pwm_pin, MOTOR_PWM_FREQ, MOTOR_INIT_DUTY);
    gpio_init(g_motor_hw[MOTOR_NUM_3].dir_pin, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}

// ============================ 电机占空比控制函数 ============================
/**
  * 函    数：motor_set_duty
  * 功    能：设置指定电机的占空比（含方向控制）
  * 参    数：motor_num - 电机编号（MOTOR_NUM_1~MOTOR_NUM_3）
  * 参    数：duty      - 占空比（正：正转；负：反转；范围[-MOTOR_MAX_DUTY, MOTOR_MAX_DUTY]）
  * 返 回 值：无
  * 备    注：1. 自动限制占空比在最大范围内，防止电机过载；
  *           2. 非法电机编号直接退出，增强容错性
  */
void motor_set_duty(MotorNum motor_num, int duty)
{
    // 参数合法性校验：电机编号超出范围则退出
    if (motor_num >= MOTOR_COUNT) 
	{
        return;
    }

    // 占空比限幅：防止超过电机最大承受值
    duty = func_limit(duty, MOTOR_MAX_DUTY);

    // 方向控制：根据占空比正负设置方向引脚电平
    if (duty > 0)	{gpio_set_level(g_motor_hw[motor_num].dir_pin, GPIO_LOW);}
    else 			{gpio_set_level(g_motor_hw[motor_num].dir_pin, GPIO_HIGH); duty = -duty;}
    // 输出PWM占空比
    pwm_set_duty(g_motor_hw[motor_num].pwm_pin, (uint32_t)duty);
}
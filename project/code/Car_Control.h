#ifndef CAR_CONTROL_H_
#define CAR_CONTROL_H_

#include "zf_common_headfile.h"


// ============================ 宏定义区 ============================
/** 对称率参数 */
#define DEFAULT_SYMMETRY               0.92f
/** 速度阈值参数 */
#define DEFLUAT_DISTANCE               90
/** 角度容差参数 */
#define ANGLE_TOLERANCE                7
#define ANGLE_THRESHOLD                150
#define ANGLE_HYSTERESIS               5
#define X_TOLERANCE                    5
/** 图像处理参数 */
#define LINE_START                     45
#define LINE_END                       5
#define MIN_LINE_LENGTH                20
#define SIDE_THRESHOLD                 30
/** 位置控制参数 */
#define XERROR_THRESHOLD               6
#define YERROR_THRESHOLD               6
#define X_SPEED_THRESHOLD              1
#define Y_SPEED_THRESHOLD              1
#define ERROR_STABLE_SAMPLES           3
#define STABLE_SAMPLES                 6
/** 灰度传感器参数 */
#define HISTORY_SIZE                   8
#define MIN_ZEROS                      7
#define MIN_ONES                       1

// ============================ 枚举类型定义区 ============================
/**
 * @brief 推箱子状态机状态枚举
 */
typedef enum 
{
    PUSH_STATE_INIT,                   // 初始化状态
    PUSH_STATE_MOVE_1,                 // 旋转车身
    PUSH_STATE_MOVE_2,                 // 侧面对齐
    PUSH_STATE_MOVE_3,                 // 推动箱子
    PUSH_STATE_MOVE_4,                 // 后退至赛道
    PUSH_STATE_MOVE_5,                 // 角度矫正
    PUSH_STATE_COMPLETE,               // 完成状态
    PUSH_STATE_TEST                    // 调试状态
} PushState;

/**
 * @brief 角度检测状态枚举
 */
typedef enum 
{
    STATE_ABOVE_THRESHOLD,             // 检测角度高于阈值
    STATE_BELOW_THRESHOLD              // 检测角度低于阈值
} AngleState;


// ============================ 状态管理接口 ============================
// 只读状态获取接口
bool is_right_position(void);
bool is_xpos_correct(void);
bool is_position_correct(void);
bool is_camera_activated(void);
uint8_t get_push_flag(void);
uint8_t get_push_time(void);
uint8_t get_find_times(void);
uint8_t get_max_box(void);
bool is_stopped(void);

// 状态设置接口（受限访问）
void set_right_position(bool value);
void set_xpos_correct(bool value);
void set_position_correct(bool value);
void set_camera_activated(bool value);
void set_push_flag(int8_t value);
void set_push_time(uint8_t value);
void set_find_times(uint8_t value);
void set_max_box(uint8_t value);
void set_stop_flag(bool value);

// 状态操作接口
void increment_push_time(void);
void increment_find_times(void);
void reset_push_state(void);
void reset_detection_flags(void);
void reset_all_state(void);

// ============================ 功能函数接口 ============================
// 推箱子动作辅助函数
bool adjust_angle(int16_t target_angle_offset);
bool push_until_leave(uint8_t speed);
bool go_back(uint8_t speed);
bool is_turrn_right_postion(void);
bool is_Continuous(void);
void Check_angle(bool task_end);
void is_turn_right(void);
bool Xpos_Correct(void);

// 推箱子动作执行函数
bool push(void);

// 状态转换函数
bool PositonCorrect_TO_Art(void);
bool isCorrcect(void);

// 主控制函数
void Push_Control(void);
void start_finish_line_control(void);
void change_speed(void);

#endif
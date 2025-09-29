#include "Car_Control.h"

// ============================ 状态私有变量 ============================
static struct {
    // 控制标志位
    uint8_t stop_flag;
    int8_t push_flag;
    uint8_t push_time;
    uint8_t find_times;
    uint8_t max_box;
    
    // 状态标志位
    bool right_position;
    bool xpos_correct_flag;
    bool position_correct_flag;
    bool camera_activated;
} push_state = {
    .stop_flag = 0,
    .push_flag = 0,
    .push_time = 0,
    .find_times = 0,
    .max_box = DefaultMaxBox,
    .right_position = false,
    .xpos_correct_flag = false,
    .position_correct_flag = false,
    .camera_activated = false
};

// ============================ 状态管理接口实现 ============================
// 只读状态获取接口
bool is_right_position(void) { return push_state.right_position; }
bool is_xpos_correct(void) { return push_state.xpos_correct_flag; }
bool is_position_correct(void) { return push_state.position_correct_flag; }
bool is_camera_activated(void) { return push_state.camera_activated; }
uint8_t get_push_flag(void) { return push_state.push_flag; }
uint8_t get_push_time(void) { return push_state.push_time; }
uint8_t get_find_times(void) { return push_state.find_times; }
uint8_t get_max_box(void) { return push_state.max_box; }
bool is_stopped(void) { return push_state.stop_flag; }

// 状态设置接口
void set_right_position(bool value) { push_state.right_position = value; }
void set_xpos_correct(bool value) { push_state.xpos_correct_flag = value; }
void set_position_correct(bool value) { push_state.position_correct_flag = value; }
void set_camera_activated(bool value) { push_state.camera_activated = value; }
void set_push_flag(int8_t value) { push_state.push_flag = value; }
void set_push_time(uint8_t value) { push_state.push_time = value; }
void set_find_times(uint8_t value) { push_state.find_times = value; }
void set_max_box(uint8_t value) { push_state.max_box = value; }
void set_stop_flag(bool value) { push_state.stop_flag = value; }

// 状态操作接口
void increment_push_time(void) { 
    if (push_state.push_time < 255) {
        push_state.push_time++; 
    }
}

void increment_find_times(void) { 
    if (push_state.find_times < 255) {
        push_state.find_times++; 
    }
}

void reset_push_state(void) {
    push_state.push_flag = 0;
    push_state.push_time = 0;
}

void reset_detection_flags(void) {
    push_state.right_position = false;
    push_state.xpos_correct_flag = false;
    push_state.position_correct_flag = false;
    push_state.camera_activated = false;
}

void reset_all_state(void) {
    push_state.stop_flag = 0;
    push_state.push_flag = 0;
    push_state.push_time = 0;
    push_state.find_times = 0;
    push_state.max_box = DefaultMaxBox;
    push_state.right_position = false;
    push_state.xpos_correct_flag = false;
    push_state.position_correct_flag = false;
    push_state.camera_activated = false;
}

// ============================ 推箱子动作辅助函数 ============================
bool adjust_angle(int16_t target_angle_offset)
{
    if (abs(angle_before_turn + target_angle_offset - Angle_now) <= ANGLE_TOLERANCE)
    {
        return true;
    }
    else
    {
        Image_Mode = 3;
        Motor_Control_Mode = 2;
        return false;
    }
}

bool push_until_leave(uint8_t speed)
{
    static uint8_t history[HISTORY_SIZE] = {0};
    static uint8_t index = 0;
    static bool initialized = false;
    
    if (!initialized) 
    {
        for (uint8_t i = 0; i < HISTORY_SIZE; i++) 
        {
            history[i] = Gray_level;
        }
        initialized = true;
    }
    
    history[index] = Gray_level;
    index = (index + 1) % HISTORY_SIZE;
    
    uint8_t zero_count = 0;
    for (uint8_t i = 0; i < HISTORY_SIZE; i++) 
    {
        if (history[i] == 0) zero_count++;
    }
    bool condition1 = (zero_count >= MIN_ZEROS);
    
    uint8_t consecutive_ones = 0;
    for (int8_t i = 1; i <= MIN_ONES; i++) 
    {
        uint8_t pos = (index - i + HISTORY_SIZE) % HISTORY_SIZE;
        if (history[pos] == 1) consecutive_ones++;
        else break;
    }
    bool condition2 = (consecutive_ones >= MIN_ONES);
    
    if (condition1 && condition2) 
    {
        move(0, 0);
        return true;
    }
    
    Motor_Control_Mode = 2;
    move(0, speed);
    return false;
}

bool go_back(uint8_t speed)
{
    static uint8 Before_gray[4] = {0, 0, 0, 0};

    for (uint8 i = 3; i > 0; i--)
    {
        Before_gray[i] = Before_gray[i-1];
    }
    Before_gray[0] = Gray_level;
    
    if (Gray_level == 0 && Before_gray[3] + Before_gray[2] + Before_gray[1] == 3)
    {
        system_delay_ms(5);
        move(0, 0);
        return true; 
    }
    
    Motor_Control_Mode = 2;
    move(180, speed);
    return false;
}

bool is_turrn_right_postion(void)
{
    static uint8_t postion_history[5] = {0, 0, 0, 0, 0};
    static uint8_t index = 0;
    bool right_postion_Flag;
    
    static float last_Symmetry = 0;
    float now_Symmetry = floorf(Symmetry_Ratio * 100.f) / 100.f;
    
    uint8_t current_flag = now_Symmetry - last_Symmetry >= 0 ? 1 : 0; 

    for (uint8 i = 4; i > 0; i--)
    {
        postion_history[i] = postion_history[i-1];
    }
    postion_history[0] = current_flag;
    
    if (last_Symmetry >= DEFAULT_SYMMETRY && current_flag == 0 && 
        postion_history[1] + postion_history[2] + postion_history[3] + postion_history[4] == 4)
    {
        right_postion_Flag = true;
        last_Symmetry = 0;
        memset(&postion_history, 0, sizeof(postion_history));
    }
    else 
    {
        right_postion_Flag = false;
    }
    
    last_Symmetry = now_Symmetry;
    return right_postion_Flag;
}

bool is_Continuous(void) 
{
    const int center_col = 100;
    const int left_col = 0;
    const int right_col = MT9V03X_W - 1;
    int left_black_count = 0;
    int right_black_count = 0;
    int current_run = 0;
    int max_run = 0;
    
    const uint8_t (*row_ptr)[MT9V03X_W] = &image_two_value[LINE_START];
    
    for (int i = LINE_START; i >= LINE_END; --i) 
    {
        const uint8_t *center_pixel = &(*row_ptr)[center_col];
        
        if (*center_pixel) 
        {
            if (++current_run > max_run) 
            {
                max_run = current_run;
                if (max_run >= MIN_LINE_LENGTH) 
                {
                    break;
                }
            }
        } 
        else 
        {
            current_run = 0;
        }
        
        row_ptr--;
        
        if (max_run + (i - LINE_END) < MIN_LINE_LENGTH) 
        {
            break;
        }
    }
    
    row_ptr = &image_two_value[LINE_START];
    for (int i = LINE_START; i < MT9V03X_H; i++) 
    {
        const uint8_t *left_pixel = &(*row_ptr)[left_col];
        const uint8_t *right_pixel = &(*row_ptr)[right_col];
        
        if (!(*left_pixel)) 
        {
            left_black_count++;
        }
        if (!(*right_pixel)) 
        {
            right_black_count++;
        }
        
        row_ptr++;
        
        if (left_black_count >= SIDE_THRESHOLD && right_black_count >= SIDE_THRESHOLD)
        {
            break;
        }
    }
    
    return (max_run >= MIN_LINE_LENGTH) && 
           (left_black_count >= SIDE_THRESHOLD) && 
           (right_black_count >= SIDE_THRESHOLD);
}

void Check_angle(bool task_end) 
{
    static AngleState state = STATE_ABOVE_THRESHOLD;
    int angle_diff = abs(angle_before_turn - Angle_now);

    if (task_end)
    {
        state = STATE_ABOVE_THRESHOLD;
        return;
    }
    
    switch (state) 
    {
        case STATE_ABOVE_THRESHOLD:
            if (angle_diff >= ANGLE_THRESHOLD) 
            {
                state = STATE_BELOW_THRESHOLD;
                rotation_direction = -rotation_direction;
                Motor_Control_Mode = 16;
                tangential_linear_speed = 25;
            }
            break;

        case STATE_BELOW_THRESHOLD:
            if ((rotation_direction == 1 && Angle_now <= angle_before_turn - ANGLE_HYSTERESIS) || 
                (rotation_direction == -1 && Angle_now >= angle_before_turn + ANGLE_HYSTERESIS)) 
            {
                state = STATE_ABOVE_THRESHOLD;
                rotation_direction = -rotation_direction;
                Motor_Control_Mode = 16;
                tangential_linear_speed = 25;
            }
            break;
            
        default:
            state = STATE_ABOVE_THRESHOLD;
            break;
    }
}

void is_turn_right(void)
{
    bool postion_flag = is_turrn_right_postion();
    
    Check_angle(postion_flag);
    if (postion_flag && abs(Angle_now - angle_before_turn) >= 35) 
    {
        if (!is_Continuous()) 
        {
            move(0, 0);
            angle_before_turn = Angle_now;
            angle_turn = 0;
            change_Velocity(Strong);
            Motor_Control_Mode = 12;
            set_right_position(true);
            pit_disable(PIT_CH3);
        }
    }
}

bool Xpos_Correct(void) 
{
    static uint8_t history[STABLE_SAMPLES] = {0};
    static uint8_t index = 0;
    static int16_t prev_error_Xpos = 0;
    static bool first_call = true;
    uint8_t all_qualified = 1;

    if (first_call) 
    {
        prev_error_Xpos = Now_Angle_X - TARGET_ANGLE_X;
        first_call = false;
    }

    int16_t error_Xpos = Now_Angle_X - TARGET_ANGLE_X;
    int16_t delta_Xpos = error_Xpos - prev_error_Xpos;
    prev_error_Xpos = error_Xpos;

    uint8_t current_flag = abs(delta_Xpos) <= 1;
    
    history[index] = current_flag;
    index = (index + 1) % STABLE_SAMPLES;

    for (int i = 0; i < STABLE_SAMPLES; i++)
    {
        if (history[i] != 1) 
        {
            all_qualified = 0;
            break;
        }
    }

    if (all_qualified) 
    {
        memset(history, 0, sizeof(history));
        index = 0;
        first_call = true;
    }

    set_xpos_correct(all_qualified);
    return is_xpos_correct();
}

// ============================ 推箱子动作执行函数 ============================
bool push(void) 
{
    static PushState push_state_machine = PUSH_STATE_INIT;
    bool push_done = false;

    switch (push_state_machine)
    {
        case PUSH_STATE_INIT:
            Image_Mode = 7;
            angle_before_turn = Angle_now;
            push_state_machine = PUSH_STATE_MOVE_1;
            tangential_linear_speed = 35;
            Motor_Control_Mode = 9;
            pit_enable(PIT_CH3);         // 开启中断判断是否进入下一状态
            change_Velocity(Hard);
            break;

        case PUSH_STATE_MOVE_1:
            if (is_right_position())
            {
                set_right_position(false);
                Image_Mode = 3;
                push_state_machine = PUSH_STATE_MOVE_2;
            }
            break;
            
        case PUSH_STATE_MOVE_2:
            Motor_Control_Mode = 12;
            if (Xpos_Correct())
            {
                Motor_Control_Mode = 2;
                calculate_flag = 1;
                push_state_machine = PUSH_STATE_MOVE_3;
                change_Velocity(Withy);
            }
            break;
            
        case PUSH_STATE_MOVE_3:
        {
            bool move_done = push_until_leave(speed_smooth(calculate_flag, y_distance, SPEED_MODE_PUSH));
            if (move_done)
            {    
                calculate_flag = 1;
                push_state_machine = PUSH_STATE_MOVE_4;
            }
            break;
        }
            
        case PUSH_STATE_MOVE_4:
        {
            bool move_done = go_back(speed_smooth(calculate_flag, y_distance, SPEED_MODE_BACK));
            if (move_done)
            {
                calculate_flag = 0;
                angle_turn = -95 * (rotation_direction);
                push_state_machine = PUSH_STATE_MOVE_5;
                change_Velocity(Hard);
            }
            break;
        }
            
        case PUSH_STATE_MOVE_5:
        {
            bool move_done = adjust_angle(angle_turn);
            if (move_done)
            {
                change_Velocity(Soft);
                push_state_machine = PUSH_STATE_COMPLETE;
                Motor_Control_Mode = 0;
            }
            break;
        }
            
        case PUSH_STATE_COMPLETE:
            push_state_machine = PUSH_STATE_INIT;
            Motor_Control_Mode = 0;
            push_done = true;
            break;
            
        case PUSH_STATE_TEST:
            push_state_machine = PUSH_STATE_INIT;
            Motor_Control_Mode = 1;
            push_done = true;
            break;
    }

    return push_done;
}

// ============================ 状态转换函数 ============================
bool PositonCorrect_TO_Art(void) 
{
    static uint8_t error_history[ERROR_STABLE_SAMPLES] = {0};
    static uint8_t full_condition_history[STABLE_SAMPLES] = {0};
    static uint8_t error_index = 0;
    static uint8_t full_index = 0;
    static int16_t prev_error_x = 0, prev_error_y = 0;
    
    int16_t error_x = uart1_data_arr[0] - 45 - TARGET_ANGLE_X;
    int16_t error_y = uart1_data_arr[1] - TARGET_POSITION_Y;
    
    int16_t delta_x = error_x - prev_error_x;
    int16_t delta_y = error_y - prev_error_y;
    prev_error_x = error_x;
    prev_error_y = error_y;
    
    bool error_condition = (abs(error_x) <= XERROR_THRESHOLD) && 
                           (abs(error_y) <= YERROR_THRESHOLD);
                           
    bool full_condition = error_condition && 
                         (abs(delta_x) <= X_SPEED_THRESHOLD) && 
                         (abs(delta_y) <= Y_SPEED_THRESHOLD);
    
    error_history[error_index] = error_condition;
    error_index = (error_index + 1) % ERROR_STABLE_SAMPLES;
    
    full_condition_history[full_index] = full_condition;
    full_index = (full_index + 1) % STABLE_SAMPLES;
    
    bool error_stable = true;
    for (int i = 0; i < ERROR_STABLE_SAMPLES; i++) {
        if (!error_history[i]) {
            error_stable = false;
            break;
        }
    }
    
    bool full_stable = true;
    for (int i = 0; i < STABLE_SAMPLES; i++) {
        if (!full_condition_history[i]) {
            full_stable = false;
            break;
        }
    }
    
    if (error_stable && !is_camera_activated()) {
        uart_write_byte(UART_4, 'a');
        uart_rx_interrupt(UART_4, 1);
        set_camera_activated(true);
    }
    
    set_position_correct(full_stable);
    return is_position_correct();
}

bool isCorrcect(void)
{
    static uint8_t history[5] = {0};
    static uint8_t index = 0;
    uint8_t all_qualified = 1;
    bool isCorrcect_Flag;
    
    uint8_t current_flag = abs(Now_Angle_X) <= X_TOLERANCE ? 1 : 0; 
    
    history[index] = current_flag;
    index = (index + 1) % 5;

    for (int i = 0; i < 5; i++) 
    {
        if (history[i] != 1)
        {
            all_qualified = 0;
            break;
        }
    }

    if (all_qualified)
    {
        isCorrcect_Flag = true;
    }
    else
    {
        isCorrcect_Flag = false;
    }

    return isCorrcect_Flag;
}

// ============================ 推箱子控制状态机 ============================
void Push_Control(void)
{ 
    if (get_push_flag() == 0 && uart1_data_arr[1] && !is_stopped())              
    {
        if (Island_State != 0 && Island_State != 4 && Island_State != 5 && Island_State != 9)
        {
            set_push_flag(0);
            uart1_data_arr[1] = 0;
            return;
        }
        
        if (abs(Now_Angle_X) <= 40)
        {
            Image_Mode = 3;
            Motor_Control_Mode = 1;
            set_push_flag(1);
        }
        else
        {
            Image_Mode = 3;
            Motor_Control_Mode = 15;
            set_push_flag(2);
        }
    }
    else if (get_push_flag() == 1)         
    {                
        if (abs(Read_wheelspeed[0]) <= 15 && abs(Read_wheelspeed[1]) <= 15 && abs(Read_wheelspeed[2]) <= 15)
        {
            change_Velocity(Strong);
            Motor_Control_Mode = 14;
            set_push_flag(3);
        }
    }
    else if (get_push_flag() == 2)         
    {        
        if (uart1_data_arr[1] >= DEFLUAT_DISTANCE)
        {
            change_Velocity(Strong);
            Motor_Control_Mode = 14;
            set_push_flag(3);
        }
    }
    else if (get_push_flag() == 3)
    {        
        bool is_Correct = PositonCorrect_TO_Art();
        if (is_Correct)    
        {
            angle_before_turn = Angle_now;
            angle_turn = 0;
            Motor_Control_Mode = 1;
            set_push_flag(4);
            set_position_correct(false);
            set_camera_activated(false);
        }    
    }
    else if (get_push_flag() == 4)
    {
        rotation_direction = ART_Recognition();
        if (rotation_direction == 1 || rotation_direction == -1)
        {
            set_push_flag(5);
            uart_rx_interrupt(UART_4, 0);
            uart4_data_arr[0] = -1;    
        }
    }
    else if (get_push_flag() == 5)
    {
        bool push_completed = push();

        if (push_completed) 
        {
            Image_Mode = 0;
            Motor_Control_Mode = 0;
            set_push_flag(6);
        }
    }
    else if (get_push_flag() == 6)
    {
        static uint8 i;
        i++;
        if (i >= 55)
        {
            i = 0;
            set_push_flag(0);
            increment_push_time();
            isopenResetM = 0;
            uart1_data_arr[1] = 0;
        }
    }
    else if (get_push_flag() == 77)
    {
        Image_Mode = 7;
        Motor_Control_Mode = 1;
    }
}

// ============================ 始终线发车停车控制 ============================
void start_finish_line_control(void)
{
    if (find_start_finish_line() && !(cross_flag || Island_State || get_push_flag()))
    {
        if (lose_point_num_L < 15 && lose_point_num_R < 15)
        {
            increment_find_times();
        }
        
        if (get_find_times() == 1)
        {
            Image_Mode = 3;
            system_delay_ms(600);
            Image_Mode = 0;
        }
        else if (get_find_times() == 2)
        {
            system_delay_ms(400);
            Motor_Control_Mode = 1;
            move(0, 0);
            set_stop_flag(true);
        }
    }
}

// ============================ 冲刺冲刺冲刺！！！！ ============================
void change_speed()
{
	if(get_push_time() >= get_max_box())
	{
		rushrushrush(); 
	}
}


// ============================ 中断处理函数 ============================
void pit_handler_3(void)
{
    is_turn_right();
}
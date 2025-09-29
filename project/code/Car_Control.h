#ifndef CAR_CONTROL_H_
#define CAR_CONTROL_H_

#include "zf_common_headfile.h"


// ============================ �궨���� ============================
/** �Գ��ʲ��� */
#define DEFAULT_SYMMETRY               0.92f
/** �ٶ���ֵ���� */
#define DEFLUAT_DISTANCE               90
/** �Ƕ��ݲ���� */
#define ANGLE_TOLERANCE                7
#define ANGLE_THRESHOLD                150
#define ANGLE_HYSTERESIS               5
#define X_TOLERANCE                    5
/** ͼ������� */
#define LINE_START                     45
#define LINE_END                       5
#define MIN_LINE_LENGTH                20
#define SIDE_THRESHOLD                 30
/** λ�ÿ��Ʋ��� */
#define XERROR_THRESHOLD               6
#define YERROR_THRESHOLD               6
#define X_SPEED_THRESHOLD              1
#define Y_SPEED_THRESHOLD              1
#define ERROR_STABLE_SAMPLES           3
#define STABLE_SAMPLES                 6
/** �Ҷȴ��������� */
#define HISTORY_SIZE                   8
#define MIN_ZEROS                      7
#define MIN_ONES                       1

// ============================ ö�����Ͷ����� ============================
/**
 * @brief ������״̬��״̬ö��
 */
typedef enum 
{
    PUSH_STATE_INIT,                   // ��ʼ��״̬
    PUSH_STATE_MOVE_1,                 // ��ת����
    PUSH_STATE_MOVE_2,                 // �������
    PUSH_STATE_MOVE_3,                 // �ƶ�����
    PUSH_STATE_MOVE_4,                 // ����������
    PUSH_STATE_MOVE_5,                 // �ǶȽ���
    PUSH_STATE_COMPLETE,               // ���״̬
    PUSH_STATE_TEST                    // ����״̬
} PushState;

/**
 * @brief �Ƕȼ��״̬ö��
 */
typedef enum 
{
    STATE_ABOVE_THRESHOLD,             // ���Ƕȸ�����ֵ
    STATE_BELOW_THRESHOLD              // ���Ƕȵ�����ֵ
} AngleState;


// ============================ ״̬����ӿ� ============================
// ֻ��״̬��ȡ�ӿ�
bool is_right_position(void);
bool is_xpos_correct(void);
bool is_position_correct(void);
bool is_camera_activated(void);
uint8_t get_push_flag(void);
uint8_t get_push_time(void);
uint8_t get_find_times(void);
uint8_t get_max_box(void);
bool is_stopped(void);

// ״̬���ýӿڣ����޷��ʣ�
void set_right_position(bool value);
void set_xpos_correct(bool value);
void set_position_correct(bool value);
void set_camera_activated(bool value);
void set_push_flag(int8_t value);
void set_push_time(uint8_t value);
void set_find_times(uint8_t value);
void set_max_box(uint8_t value);
void set_stop_flag(bool value);

// ״̬�����ӿ�
void increment_push_time(void);
void increment_find_times(void);
void reset_push_state(void);
void reset_detection_flags(void);
void reset_all_state(void);

// ============================ ���ܺ����ӿ� ============================
// �����Ӷ�����������
bool adjust_angle(int16_t target_angle_offset);
bool push_until_leave(uint8_t speed);
bool go_back(uint8_t speed);
bool is_turrn_right_postion(void);
bool is_Continuous(void);
void Check_angle(bool task_end);
void is_turn_right(void);
bool Xpos_Correct(void);

// �����Ӷ���ִ�к���
bool push(void);

// ״̬ת������
bool PositonCorrect_TO_Art(void);
bool isCorrcect(void);

// �����ƺ���
void Push_Control(void);
void start_finish_line_control(void);
void change_speed(void);

#endif
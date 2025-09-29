#ifndef __MY_UART_H__
#define __MY_UART_H__

#include "zf_common_headfile.h"

// ============================ �궨���� ============================
/**
 * ����Ӳ���뻺������
 */
#define UART_BUFFER_SIZE 20  // ����FIFO�����С������3�����ݵ����ݰ����ɰ�����չ��
#define UART_BAUDRATE    115200  // ͨ�ò����ʣ����д���ͳһ��

/**
 * ���ڹ���ӳ�䣨��ȷ������;�����������
 */
#define COLOR_UART   UART_1    // ��ɫ��ARTģ��ͨ�Ŵ���
#define OBJECT_UART  UART_4    // ����ARTģ��ͨ�Ŵ���

/**
 * ģ�鹤��ģʽ�궨��
 */
#define DEFAULT_OBJECT_MODE  0  // ����ģʽ��0-ʮλ+��λ��1-�ٷ���
#define DEFAULT_COLOR_MODE   0  // ��ɫ��ģʽ��0-�Ͻ����룻1-��Զ����
#define DEFAULT_ON_TRACK_MODE 0 // �����ж�ģʽ��0-��������1-����

/**
 * ģʽ��Ӧ����ֵ��ͳһ����ģʽ����ֵӳ�䣩
 */
#define OBJECT_VALUE(param)  ((param) == 0 ? -1000 : -2000)
#define COLOR_VALUE(param)   ((param) == 0 ? -3000 : -4000)
#define TRACK_VALUE(param)   ((param) == 0 ? -6000 : -5000)

/**
 * ����ͨ��Э��꣨��ͷ/��β��ͳһЭ�鶨�壩
 */
#define UART1_HEAD  '@'  // ����1���ݰ���ͷ
#define UART1_TAIL  '#'  // ����1���ݰ���β
#define UART4_HEAD  'Y'  // ����4���ݰ���ͷ
#define UART4_TAIL  'Z'  // ����4���ݰ���β

// ============================ ö�����Ͷ����� ============================
/**
 * @brief ���ڽ���״̬ö��
 * ͳһ�������д��ڵĽ���״̬��״̬
 */
typedef enum {
    UART_RX_STATE_IDLE = 0,  // ����״̬���ȴ���ͷ��
    UART_RX_STATE_DATA       // ���ݽ���״̬���ȴ���β��
} UartRxState;

// ============================ ȫ�ֱ��������� ============================
// ���ݰ�������ɱ�־
extern uint8_t packge1_finish_flag;  // ����1���ݰ�������ɱ�־
extern uint8_t packge4_finish_flag;  // ����4���ݰ�������ɱ�־

// ����FIFO��������
extern uint8_t uart1_buffer[UART_BUFFER_SIZE];  // ����1����
extern uint8_t uart4_buffer[UART_BUFFER_SIZE];  // ����4����

// ���������������
extern int16_t uart1_data_arr[2];  // ����1��[X�Ƕ�, Yλ��]
extern int16_t uart4_data_arr[1];  // ����4��[������]

extern int Now_Angle_X;          //��ǰX������
extern int Now_Position_Y;       //��ǰY������


// ============================ ���������� ============================
/**
 * ���ڳ�ʼ����غ���
 */
void my_uart_init(void);  // ��ʼ�����д��ڣ���FIFO���жϣ�

/**
 * �������ݷ�����غ���
 */
void uart_send_int16_with_delimiter(uart_index_enum uart_n, int16_t data);  // ��Э�鷢��int16����

/**
 * �����жϻص��������ڲ�ʹ�ã��ⲿ������ã�
 */
void my_uart_callback(uart_index_enum uart_n);

#endif
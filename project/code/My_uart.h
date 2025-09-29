#ifndef __MY_UART_H__
#define __MY_UART_H__

#include "zf_common_headfile.h"

// ============================ 宏定义区 ============================
/**
 * 串口硬件与缓存配置
 */
#define UART_BUFFER_SIZE 20  // 串口FIFO缓存大小（适配3个数据的数据包，可按需扩展）
#define UART_BAUDRATE    115200  // 通用波特率（所有串口统一）

/**
 * 串口功能映射（明确串口用途，避免混淆）
 */
#define COLOR_UART   UART_1    // 找色块ART模块通信串口
#define OBJECT_UART  UART_4    // 分类ART模块通信串口

/**
 * 模块工作模式宏定义
 */
#define DEFAULT_OBJECT_MODE  0  // 分类模式：0-十位+个位；1-百分类
#define DEFAULT_COLOR_MODE   0  // 找色块模式：0-较近距离；1-较远距离
#define DEFAULT_ON_TRACK_MODE 0 // 赛道判断模式：0-不开启；1-开启

/**
 * 模式对应参数值（统一管理模式与数值映射）
 */
#define OBJECT_VALUE(param)  ((param) == 0 ? -1000 : -2000)
#define COLOR_VALUE(param)   ((param) == 0 ? -3000 : -4000)
#define TRACK_VALUE(param)   ((param) == 0 ? -6000 : -5000)

/**
 * 串口通信协议宏（包头/包尾，统一协议定义）
 */
#define UART1_HEAD  '@'  // 串口1数据包包头
#define UART1_TAIL  '#'  // 串口1数据包包尾
#define UART4_HEAD  'Y'  // 串口4数据包包头
#define UART4_TAIL  'Z'  // 串口4数据包包尾

// ============================ 枚举类型定义区 ============================
/**
 * @brief 串口接收状态枚举
 * 统一管理所有串口的接收状态机状态
 */
typedef enum {
    UART_RX_STATE_IDLE = 0,  // 空闲状态（等待包头）
    UART_RX_STATE_DATA       // 数据接收状态（等待包尾）
} UartRxState;

// ============================ 全局变量声明区 ============================
// 数据包接收完成标志
extern uint8_t packge1_finish_flag;  // 串口1数据包接收完成标志
extern uint8_t packge4_finish_flag;  // 串口4数据包接收完成标志

// 串口FIFO缓存数组
extern uint8_t uart1_buffer[UART_BUFFER_SIZE];  // 串口1缓存
extern uint8_t uart4_buffer[UART_BUFFER_SIZE];  // 串口4缓存

// 解析后的数据数组
extern int16_t uart1_data_arr[2];  // 串口1：[X角度, Y位置]
extern int16_t uart4_data_arr[1];  // 串口4：[分类结果]

extern int Now_Angle_X;          //当前X轴数据
extern int Now_Position_Y;       //当前Y轴数据


// ============================ 函数声明区 ============================
/**
 * 串口初始化相关函数
 */
void my_uart_init(void);  // 初始化所有串口（含FIFO与中断）

/**
 * 串口数据发送相关函数
 */
void uart_send_int16_with_delimiter(uart_index_enum uart_n, int16_t data);  // 带协议发送int16数据

/**
 * 串口中断回调函数（内部使用，外部无需调用）
 */
void my_uart_callback(uart_index_enum uart_n);

#endif
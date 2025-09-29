#include "ART.h"
#include "zf_common_headfile.h"  // 确保包含UART相关接口定义

// ============================ 全局变量定义区 ============================
int ART[QUEUE_SIZE];          // ART识别结果缓存队列
uint8_t head = 0;             // 队列头指针（初始指向0号位置）
uint8_t tail = 0;             // 队列尾指针（初始指向0号位置）

// ============================ 核心功能函数区 ============================
/**
  * 函    数：ART_Recognition
  * 功    能：ART识别数据接收、缓存与方向判断
  * 参    数：无
  * 返 回 值：int8_t - 方向结果（-1：向左推；1：向右推；22：数据无效）
  * 备    注：1. 数据有效性判断：uart4_data_arr[0] != -1 为有效数据；
  *           2. 队列逻辑：尾指针循环移动，满队列时覆盖头部最早数据；
  *           3. 方向判断：bit0为1→向左推，bit0为0→向右推，均发送停止识别指令
  */
int8_t ART_Recognition() 
{
    // 1. 数据有效性校验：无效则返回错误码22
    if (uart4_data_arr[0] == -1)
    {
        return 22;
    }

    // 2. 有效数据入队：将最新识别结果存入队列尾
    ART[tail] = uart4_data_arr[0];
    // 尾指针循环后移（超出队列容量时回绕到0）
    tail = (tail + 1) % QUEUE_SIZE;

    // 3. 队列满处理：覆盖最早数据（头指针随尾指针后移）
    if (tail == head) 
    {
        head = (head + 1) % QUEUE_SIZE;
    }

    // 4. 方向判断：根据数据bit0位判断推料方向
    uart_write_byte(UART_4, 'b');  // 向摄像头发送指令，停止识别
    if (uart4_data_arr[0] & 0x01)  // bit0为1 → 向左推
    {
        return -1;
    }
    else                           // bit0为0 → 向右推
    {
        return 1;
    }
}
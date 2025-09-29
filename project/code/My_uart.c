#include "My_uart.h"

// ============================ 遗漏全局变量定义区 ============================
/**
 * 头文件中声明的extern变量，需在此处定义实体（内存分配）
 * 注：这些变量供外部模块访问，因此不能加static
 */
uint8_t packge1_finish_flag = 0;  // 串口1数据包接收完成标志（初始为未完成）
uint8_t packge4_finish_flag = 0;  // 串口4数据包接收完成标志（初始为未完成）

uint8_t uart1_buffer[UART_BUFFER_SIZE] = {0};  // 串口1 FIFO缓存数组（初始清零）
uint8_t uart4_buffer[UART_BUFFER_SIZE] = {0};  // 串口4 FIFO缓存数组（初始清零）

int16_t uart1_data_arr[2] = {0};  // 串口1解析后数据：[X角度, Y位置]（初始为0）
int16_t uart4_data_arr[1] = {-1}; // 串口4解析后数据：[分类结果]（初始为-1，与原逻辑一致）

int Now_Angle_X;          //当前X轴数据
int Now_Position_Y;       //当前Y轴数据

// ============================ 静态变量定义区 ============================
/**
 * 串口FIFO句柄（静态私有，仅内部访问）
 */
static fifo_struct uart1_fifo;  // 串口1 FIFO管理结构体
static fifo_struct uart4_fifo;  // 串口4 FIFO管理结构体

/**
 * 串口接收状态机（按串口编号对应，统一管理）
 */
static UartRxState uart_rx_state[2] = {UART_RX_STATE_IDLE, UART_RX_STATE_IDLE};
// 索引0：串口1；索引1：串口4（与UartIdx枚举对应）

/**
 * 临时接收数据缓存（单字节，所有串口共用）
 */
static uint8_t uart_rx_temp = 0;

/**
 * 串口FIFO数据长度（按串口编号对应）
 */
static uint32_t uart_fifo_len[2] = {0, 0};
// 索引0：串口1；索引1：串口4（与UartIdx枚举对应）

// ============================ 内部枚举定义 ============================
/**
 * @brief 内部串口索引枚举
 * 用于关联串口模块与状态/长度数组的索引，避免硬编码
 */
typedef enum {
    UART_IDX_1 = 0,  // 串口1对应索引
    UART_IDX_4,      // 串口4对应索引
    UART_IDX_COUNT   // 串口总数
} UartIdx;

// ============================ 内部工具函数 ============================
/**
 * 函    数：get_uart_index
 * 功    能：将串口模块号转换为内部索引（关联状态/长度数组）
 * 参    数：uart_n - 串口模块号（UART_1/UART_4）
 * 返 回 值：UartIdx - 内部索引（非法时返回UART_IDX_COUNT）
 */
static UartIdx get_uart_index(uart_index_enum uart_n) {
    switch (uart_n) {
        case UART_1: return UART_IDX_1;
        case UART_4: return UART_IDX_4;
        default: return UART_IDX_COUNT;
    }
}

/**
 * 函    数：get_uart_protocol
 * 功    能：获取指定串口的通信协议（包头/包尾）
 * 参    数：uart_n - 串口模块号
 * 参    数：head   - 输出参数，包头
 * 参    数：tail   - 输出参数，包尾
 * 返 回 值：bool - 协议是否有效（true：有效；false：无效）
 */
static bool get_uart_protocol(uart_index_enum uart_n, uint8_t* head, uint8_t* tail) {
    if (head == NULL || tail == NULL) return false;
    switch (uart_n) {
        case UART_1:
            *head = UART1_HEAD;
            *tail = UART1_TAIL;
            return true;
        case UART_4:
            *head = UART4_HEAD;
            *tail = UART4_TAIL;
            return true;
        default:
            return false;
    }
}

// ============================ 串口初始化函数 ============================
/**
 * 函    数：my_uart_init
 * 功    能：初始化串口1/4（配置引脚、波特率、FIFO与中断）
 * 参    数：无
 * 返 回 值：无
 * 备    注：1. 统一使用UART_BAUDRATE波特率；
 *           2. 使能串口接收中断，禁用发送中断（按需开启）
 */
void my_uart_init(void) {
    // 1. 初始化串口硬件（引脚+波特率）
    uart_init(UART_1, UART_BAUDRATE, UART1_TX_B12, UART1_RX_B13);
    uart_init(UART_4, UART_BAUDRATE, UART4_TX_C16, UART4_RX_C17);

    // 2. 初始化FIFO缓存
    fifo_init(&uart1_fifo, FIFO_DATA_8BIT, uart1_buffer, UART_BUFFER_SIZE);
    fifo_init(&uart4_fifo, FIFO_DATA_8BIT, uart4_buffer, UART_BUFFER_SIZE);

    // 3. 配置中断（仅使能接收中断）
    uart_rx_interrupt(UART_1, 1);  // 开启串口1接收中断
    uart_rx_interrupt(UART_4, 1);  // 开启串口4接收中断
    // uart_tx_interrupt(UART_4, 1);  // 按需开启发送中断
}

// ============================ 串口中断回调函数 ============================
/**
 * 函    数：my_uart_callback
 * 功    能：串口接收中断回调，处理数据包解析（状态机实现）
 * 参    数：uart_n - 产生中断的串口模块号
 * 返 回 值：无
 * 备    注：1. 支持串口1/4的协议解析，共用一套状态机逻辑；
 *           2. 解析完成后更新全局数据与完成标志
 */
void my_uart_callback(uart_index_enum uart_n) 
{
    // 转换串口模块号为内部索引，获取协议参数
    UartIdx uart_idx = get_uart_index(uart_n);
    uint8_t head, tail;
    if (uart_idx >= UART_IDX_COUNT || !get_uart_protocol(uart_n, &head, &tail)) 
	{
        return;  // 非法串口或协议，直接退出
    }

    // 读取接收数据（必须校验读取结果，避免死循环）
    if (uart_query_byte(uart_n, &uart_rx_temp) != 0) 
	{
        switch (uart_rx_state[uart_idx]) 
		{
            case UART_RX_STATE_IDLE:
                // 空闲状态：等待包头，FIFO为空时才更新状态
                if (uart_rx_temp == head && fifo_used((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo) == 0) 
				{
                    uart_rx_state[uart_idx] = UART_RX_STATE_DATA;  // 切换到数据接收状态
                    // 清除完成标志
                    if (uart_idx == UART_IDX_1) packge1_finish_flag = 0;
                    else packge4_finish_flag = 0;
                }
                break;

            case UART_RX_STATE_DATA:
                if (uart_rx_temp != tail) 
				{
                    // 未到包尾：写入FIFO
                    fifo_write_buffer((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo, &uart_rx_temp, 1);
                } 
				else 
				{
                    // 收到包尾：解析数据
                    uart_fifo_len[uart_idx] = fifo_used((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo);
                    // 写入换行符（保持原解析格式兼容）
                    fifo_write_buffer((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo, "\n", 1);

                    if (uart_idx == UART_IDX_1 && uart_fifo_len[uart_idx] >= 3) //这里的3根据实际情况调整
					{
                        sscanf((const char*)uart1_buffer, "%d,%d\n", &uart1_data_arr[0], &uart1_data_arr[1]);
						
                        Now_Angle_X = uart1_data_arr[0] - 45;
                        Now_Position_Y = uart1_data_arr[1];
						
                        packge1_finish_flag = 1;  // 置位完成标志
                    } 
					else if (uart_idx == UART_IDX_4 && uart_fifo_len[uart_idx] >= 2) //这里的2根据实际情况调整
					{
                        sscanf((const char*)uart4_buffer, "%d\n", &uart4_data_arr[0]);
						
                        packge4_finish_flag = 1;  // 置位完成标志
                    }

                    // 清除FIFO，切换回空闲状态
                    fifo_clear((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo);
                    uart_rx_state[uart_idx] = UART_RX_STATE_IDLE;
                }
                break;

            default:
                // 异常状态：重置
                fifo_clear((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo);
                uart_rx_state[uart_idx] = UART_RX_STATE_IDLE;
                break;
        }
    }

    // 清空临时缓存
    memset(&uart_rx_temp, 0, sizeof(uart_rx_temp));
}

// ============================ 串口数据发送函数 ============================
/**
 * 函    数：uart_send_int16_with_delimiter
 * 功    能：按串口协议发送int16_t数据（包头+高8位+低8位+包尾）
 * 参    数：uart_n - 串口模块号（UART_1/UART_4）
 * 参    数：data   - 待发送的int16_t类型数据
 * 返 回 值：无
 * 备    注：1. 发送协议与对应串口的接收协议一致（共用同一套包头包尾），因此复用get_uart_protocol函数获取协议参数；
 *           2. 固定4字节格式：包头(1B) + 数据高8位(1B) + 数据低8位(1B) + 包尾(1B)
 */
void uart_send_int16_with_delimiter(uart_index_enum uart_n, int16_t data)
{
    uint8_t send_buff[4];  // 固定格式：包头(1) + 高8位(1) + 低8位(1) + 包尾(1)
    uint8_t head, tail;

    // 获取当前串口的协议（发送与接收共用同一套包头包尾，复用协议获取逻辑）
    if (!get_uart_protocol(uart_n, &head, &tail)) 
    {
        return;  // 非法串口，退出
    }

    // 填充协议与数据
    send_buff[0] = head;                      // 包头
    send_buff[1] = (uint8_t)((data >> 8) & 0xFF);  // 数据高8位
    send_buff[2] = (uint8_t)(data & 0xFF);         // 数据低8位
    send_buff[3] = tail;                      // 包尾

    // 发送数据
    uart_write_buffer(uart_n, send_buff, 4);
}
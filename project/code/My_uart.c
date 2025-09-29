#include "My_uart.h"

// ============================ ��©ȫ�ֱ��������� ============================
/**
 * ͷ�ļ���������extern���������ڴ˴�����ʵ�壨�ڴ���䣩
 * ע����Щ�������ⲿģ����ʣ���˲��ܼ�static
 */
uint8_t packge1_finish_flag = 0;  // ����1���ݰ�������ɱ�־����ʼΪδ��ɣ�
uint8_t packge4_finish_flag = 0;  // ����4���ݰ�������ɱ�־����ʼΪδ��ɣ�

uint8_t uart1_buffer[UART_BUFFER_SIZE] = {0};  // ����1 FIFO�������飨��ʼ���㣩
uint8_t uart4_buffer[UART_BUFFER_SIZE] = {0};  // ����4 FIFO�������飨��ʼ���㣩

int16_t uart1_data_arr[2] = {0};  // ����1���������ݣ�[X�Ƕ�, Yλ��]����ʼΪ0��
int16_t uart4_data_arr[1] = {-1}; // ����4���������ݣ�[������]����ʼΪ-1����ԭ�߼�һ�£�

int Now_Angle_X;          //��ǰX������
int Now_Position_Y;       //��ǰY������

// ============================ ��̬���������� ============================
/**
 * ����FIFO�������̬˽�У����ڲ����ʣ�
 */
static fifo_struct uart1_fifo;  // ����1 FIFO����ṹ��
static fifo_struct uart4_fifo;  // ����4 FIFO����ṹ��

/**
 * ���ڽ���״̬���������ڱ�Ŷ�Ӧ��ͳһ����
 */
static UartRxState uart_rx_state[2] = {UART_RX_STATE_IDLE, UART_RX_STATE_IDLE};
// ����0������1������1������4����UartIdxö�ٶ�Ӧ��

/**
 * ��ʱ�������ݻ��棨���ֽڣ����д��ڹ��ã�
 */
static uint8_t uart_rx_temp = 0;

/**
 * ����FIFO���ݳ��ȣ������ڱ�Ŷ�Ӧ��
 */
static uint32_t uart_fifo_len[2] = {0, 0};
// ����0������1������1������4����UartIdxö�ٶ�Ӧ��

// ============================ �ڲ�ö�ٶ��� ============================
/**
 * @brief �ڲ���������ö��
 * ���ڹ�������ģ����״̬/�������������������Ӳ����
 */
typedef enum {
    UART_IDX_1 = 0,  // ����1��Ӧ����
    UART_IDX_4,      // ����4��Ӧ����
    UART_IDX_COUNT   // ��������
} UartIdx;

// ============================ �ڲ����ߺ��� ============================
/**
 * ��    ����get_uart_index
 * ��    �ܣ�������ģ���ת��Ϊ�ڲ�����������״̬/�������飩
 * ��    ����uart_n - ����ģ��ţ�UART_1/UART_4��
 * �� �� ֵ��UartIdx - �ڲ��������Ƿ�ʱ����UART_IDX_COUNT��
 */
static UartIdx get_uart_index(uart_index_enum uart_n) {
    switch (uart_n) {
        case UART_1: return UART_IDX_1;
        case UART_4: return UART_IDX_4;
        default: return UART_IDX_COUNT;
    }
}

/**
 * ��    ����get_uart_protocol
 * ��    �ܣ���ȡָ�����ڵ�ͨ��Э�飨��ͷ/��β��
 * ��    ����uart_n - ����ģ���
 * ��    ����head   - �����������ͷ
 * ��    ����tail   - �����������β
 * �� �� ֵ��bool - Э���Ƿ���Ч��true����Ч��false����Ч��
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

// ============================ ���ڳ�ʼ������ ============================
/**
 * ��    ����my_uart_init
 * ��    �ܣ���ʼ������1/4���������š������ʡ�FIFO���жϣ�
 * ��    ������
 * �� �� ֵ����
 * ��    ע��1. ͳһʹ��UART_BAUDRATE�����ʣ�
 *           2. ʹ�ܴ��ڽ����жϣ����÷����жϣ����迪����
 */
void my_uart_init(void) {
    // 1. ��ʼ������Ӳ��������+�����ʣ�
    uart_init(UART_1, UART_BAUDRATE, UART1_TX_B12, UART1_RX_B13);
    uart_init(UART_4, UART_BAUDRATE, UART4_TX_C16, UART4_RX_C17);

    // 2. ��ʼ��FIFO����
    fifo_init(&uart1_fifo, FIFO_DATA_8BIT, uart1_buffer, UART_BUFFER_SIZE);
    fifo_init(&uart4_fifo, FIFO_DATA_8BIT, uart4_buffer, UART_BUFFER_SIZE);

    // 3. �����жϣ���ʹ�ܽ����жϣ�
    uart_rx_interrupt(UART_1, 1);  // ��������1�����ж�
    uart_rx_interrupt(UART_4, 1);  // ��������4�����ж�
    // uart_tx_interrupt(UART_4, 1);  // ���迪�������ж�
}

// ============================ �����жϻص����� ============================
/**
 * ��    ����my_uart_callback
 * ��    �ܣ����ڽ����жϻص����������ݰ�������״̬��ʵ�֣�
 * ��    ����uart_n - �����жϵĴ���ģ���
 * �� �� ֵ����
 * ��    ע��1. ֧�ִ���1/4��Э�����������һ��״̬���߼���
 *           2. ������ɺ����ȫ����������ɱ�־
 */
void my_uart_callback(uart_index_enum uart_n) 
{
    // ת������ģ���Ϊ�ڲ���������ȡЭ�����
    UartIdx uart_idx = get_uart_index(uart_n);
    uint8_t head, tail;
    if (uart_idx >= UART_IDX_COUNT || !get_uart_protocol(uart_n, &head, &tail)) 
	{
        return;  // �Ƿ����ڻ�Э�飬ֱ���˳�
    }

    // ��ȡ�������ݣ�����У���ȡ�����������ѭ����
    if (uart_query_byte(uart_n, &uart_rx_temp) != 0) 
	{
        switch (uart_rx_state[uart_idx]) 
		{
            case UART_RX_STATE_IDLE:
                // ����״̬���ȴ���ͷ��FIFOΪ��ʱ�Ÿ���״̬
                if (uart_rx_temp == head && fifo_used((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo) == 0) 
				{
                    uart_rx_state[uart_idx] = UART_RX_STATE_DATA;  // �л������ݽ���״̬
                    // �����ɱ�־
                    if (uart_idx == UART_IDX_1) packge1_finish_flag = 0;
                    else packge4_finish_flag = 0;
                }
                break;

            case UART_RX_STATE_DATA:
                if (uart_rx_temp != tail) 
				{
                    // δ����β��д��FIFO
                    fifo_write_buffer((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo, &uart_rx_temp, 1);
                } 
				else 
				{
                    // �յ���β����������
                    uart_fifo_len[uart_idx] = fifo_used((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo);
                    // д�뻻�з�������ԭ������ʽ���ݣ�
                    fifo_write_buffer((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo, "\n", 1);

                    if (uart_idx == UART_IDX_1 && uart_fifo_len[uart_idx] >= 3) //�����3����ʵ���������
					{
                        sscanf((const char*)uart1_buffer, "%d,%d\n", &uart1_data_arr[0], &uart1_data_arr[1]);
						
                        Now_Angle_X = uart1_data_arr[0] - 45;
                        Now_Position_Y = uart1_data_arr[1];
						
                        packge1_finish_flag = 1;  // ��λ��ɱ�־
                    } 
					else if (uart_idx == UART_IDX_4 && uart_fifo_len[uart_idx] >= 2) //�����2����ʵ���������
					{
                        sscanf((const char*)uart4_buffer, "%d\n", &uart4_data_arr[0]);
						
                        packge4_finish_flag = 1;  // ��λ��ɱ�־
                    }

                    // ���FIFO���л��ؿ���״̬
                    fifo_clear((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo);
                    uart_rx_state[uart_idx] = UART_RX_STATE_IDLE;
                }
                break;

            default:
                // �쳣״̬������
                fifo_clear((uart_idx == UART_IDX_1) ? &uart1_fifo : &uart4_fifo);
                uart_rx_state[uart_idx] = UART_RX_STATE_IDLE;
                break;
        }
    }

    // �����ʱ����
    memset(&uart_rx_temp, 0, sizeof(uart_rx_temp));
}

// ============================ �������ݷ��ͺ��� ============================
/**
 * ��    ����uart_send_int16_with_delimiter
 * ��    �ܣ�������Э�鷢��int16_t���ݣ���ͷ+��8λ+��8λ+��β��
 * ��    ����uart_n - ����ģ��ţ�UART_1/UART_4��
 * ��    ����data   - �����͵�int16_t��������
 * �� �� ֵ����
 * ��    ע��1. ����Э�����Ӧ���ڵĽ���Э��һ�£�����ͬһ�װ�ͷ��β������˸���get_uart_protocol������ȡЭ�������
 *           2. �̶�4�ֽڸ�ʽ����ͷ(1B) + ���ݸ�8λ(1B) + ���ݵ�8λ(1B) + ��β(1B)
 */
void uart_send_int16_with_delimiter(uart_index_enum uart_n, int16_t data)
{
    uint8_t send_buff[4];  // �̶���ʽ����ͷ(1) + ��8λ(1) + ��8λ(1) + ��β(1)
    uint8_t head, tail;

    // ��ȡ��ǰ���ڵ�Э�飨��������չ���ͬһ�װ�ͷ��β������Э���ȡ�߼���
    if (!get_uart_protocol(uart_n, &head, &tail)) 
    {
        return;  // �Ƿ����ڣ��˳�
    }

    // ���Э��������
    send_buff[0] = head;                      // ��ͷ
    send_buff[1] = (uint8_t)((data >> 8) & 0xFF);  // ���ݸ�8λ
    send_buff[2] = (uint8_t)(data & 0xFF);         // ���ݵ�8λ
    send_buff[3] = tail;                      // ��β

    // ��������
    uart_write_buffer(uart_n, send_buff, 4);
}
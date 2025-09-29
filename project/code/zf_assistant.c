#include "zf_assistant.h"

// ============================ ȫ�ֱ��������� ============================
/**
 * ͼ�񱸷�����
 * ���ڷ���ǰ����ԭʼͼ�񣬱��⴫���������ͼ����µ��µĻ���˺��
 */
uint8_t image_copy[MT9V03X_H][MT9V03X_W];  // �ߴ�������ͷ�ֱ���ƥ��

// ============================ ��ʼ����غ��� ============================
/**
 * ��    ����My_ZFASSISTANT_Init
 * ��    �ܣ���ʼ��WIFIģ����������ֽӿ�
 * ��    ������
 * �� �� ֵ����
 * ��    ע��1. ����WIFI���ӣ��Զ�/�ֶ��������ֽӿڳ�ʼ����
 *           2. ��ʼ��ʧ��ʱ��������Բ���ӡ��ʾ��Ϣ
 */
void My_ZFASSISTANT_Init(void)
{
    // ��ʼ��WIFIģ�鲢����ָ���ȵ�
    while (wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
    {
        printf("\r\n connect wifi failed. \r\n");
        system_delay_ms(100);  // ��ʼ��ʧ��ʱ��ʱ����
    }

    // ��ӡWIFIģ����Ϣ
    printf("\r\n module version:%s", wifi_spi_version);       // �̼��汾
    printf("\r\n module mac    :%s", wifi_spi_mac_addr);      // MAC��ַ
    printf("\r\n module ip     :%s", wifi_spi_ip_addr_port);  // IP��ַ��˿�

    // �ֶ�����TCP���ӣ����Զ����ӹ���δ����ʱ��
    if (1 != WIFI_SPI_AUTO_CONNECT)
    {
        while (wifi_spi_socket_connect(
            "TCP",                  // ͨѶ��ʽ��TCP
            WIFI_SPI_TARGET_IP,     // Ŀ��IP����λ��IP��
            WIFI_SPI_TARGET_PORT,   // Ŀ��˿ڣ���λ���˿ڣ�Ĭ��8080��
            WIFI_SPI_LOCAL_PORT))   // ���ض˿�
        {
            printf("\r\n Connect TCP Servers error, try again.");
            system_delay_ms(100);  // ����ʧ��ʱ��ʱ����
        }
    }
    
    // ��ʼ��������ֽӿڣ�ʹ�ø���WIFI SPI���䣩
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
    
    // ���跢��ͼ�����ߣ����ڴ˴����ö�Ӧ���ú���
    // seekfree_assistant_camera_information_config(...);  // ͼ���������
    // seekfree_assistant_camera_boundary_config(...);     // ���߲�������
}

// ============================ ͼ������غ��� ============================
/**
 * ��    ����send_Image
 * ��    �ܣ����ݲ���������ͷͼ�����ݵ���λ��
 * ��    ������
 * �� �� ֵ����
 * ��    ע��1. ��������ͷ�ɼ���ɣ�mt9v03x_finish_flag=1��ʱִ�У�
 *           2. ����ǰ����ͼ��image_copy�����⴫��ʱ����˺�ѣ�
 *           3. ��ͨ��ע���л�����ԭʼͼ����ֵ��ͼ��
 */
void send_Image(void)
{
    // �������ͷ�Ƿ����һ֡�ɼ�
    if (mt9v03x_finish_flag)
    {
        mt9v03x_finish_flag = 0;  // ����ɼ���ɱ�־
        
        // ��������ͷ��Ϣ��ͼ�����͡�����������ߣ�
        seekfree_assistant_camera_information_config(
            SEEKFREE_ASSISTANT_MT9V03X,  // ����ͷ���ͣ�MT9V03X
            image_copy[0],               // ͼ�����ݻ�����
            MT9V03X_W,                   // ͼ����
            MT9V03X_H                    // ͼ��߶�
        );
        
        // ���ñ�����Ϣ��X������ߣ��ɸ��������޸ģ�
        seekfree_assistant_camera_boundary_config(
            X_BOUNDARY,                  // ���߷���X����
            MT9V03X_H,                   // ���߳���
            boder_L, boder_M, boder_R,   // ��/��/�ұ߽�
            NULL, NULL, NULL             // Ԥ������
        );
        
        // ����ͼ�����ݣ����ⷢ��ʱԭʼͼ�񱻸��µ���˺�ѣ�
        memcpy(image_copy[0], mt9v03x_image[0], MT9V03X_IMAGE_SIZE);  // ԭʼͼ��
        // memcpy(image_copy[0], image_two_value[0], MT9V03X_IMAGE_SIZE);  // ��ֵ��ͼ�񣨿�ѡ��
        
        // ����ͼ�����ݵ���λ��
        seekfree_assistant_camera_send();
    }
}
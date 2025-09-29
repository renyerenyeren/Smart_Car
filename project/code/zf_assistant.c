#include "zf_assistant.h"

// ============================ 全局变量定义区 ============================
/**
 * 图像备份数组
 * 用于发送前备份原始图像，避免传输过程中因图像更新导致的画面撕裂
 */
uint8_t image_copy[MT9V03X_H][MT9V03X_W];  // 尺寸与摄像头分辨率匹配

// ============================ 初始化相关函数 ============================
/**
 * 函    数：My_ZFASSISTANT_Init
 * 功    能：初始化WIFI模块与逐飞助手接口
 * 参    数：无
 * 返 回 值：无
 * 备    注：1. 包含WIFI连接（自动/手动）和助手接口初始化；
 *           2. 初始化失败时会持续重试并打印提示信息
 */
void My_ZFASSISTANT_Init(void)
{
    // 初始化WIFI模块并连接指定热点
    while (wifi_spi_init(WIFI_SSID_TEST, WIFI_PASSWORD_TEST))
    {
        printf("\r\n connect wifi failed. \r\n");
        system_delay_ms(100);  // 初始化失败时延时重试
    }

    // 打印WIFI模块信息
    printf("\r\n module version:%s", wifi_spi_version);       // 固件版本
    printf("\r\n module mac    :%s", wifi_spi_mac_addr);      // MAC地址
    printf("\r\n module ip     :%s", wifi_spi_ip_addr_port);  // IP地址与端口

    // 手动建立TCP连接（当自动连接功能未开启时）
    if (1 != WIFI_SPI_AUTO_CONNECT)
    {
        while (wifi_spi_socket_connect(
            "TCP",                  // 通讯方式：TCP
            WIFI_SPI_TARGET_IP,     // 目标IP（上位机IP）
            WIFI_SPI_TARGET_PORT,   // 目标端口（上位机端口，默认8080）
            WIFI_SPI_LOCAL_PORT))   // 本地端口
        {
            printf("\r\n Connect TCP Servers error, try again.");
            system_delay_ms(100);  // 连接失败时延时重试
        }
    }
    
    // 初始化逐飞助手接口（使用高速WIFI SPI传输）
    seekfree_assistant_interface_init(SEEKFREE_ASSISTANT_WIFI_SPI);
    
    // 若需发送图像或边线，需在此处调用对应配置函数
    // seekfree_assistant_camera_information_config(...);  // 图像参数配置
    // seekfree_assistant_camera_boundary_config(...);     // 边线参数配置
}

// ============================ 图像发送相关函数 ============================
/**
 * 函    数：send_Image
 * 功    能：备份并发送摄像头图像数据到上位机
 * 参    数：无
 * 返 回 值：无
 * 备    注：1. 仅在摄像头采集完成（mt9v03x_finish_flag=1）时执行；
 *           2. 发送前备份图像到image_copy，避免传输时画面撕裂；
 *           3. 可通过注释切换发送原始图像或二值化图像
 */
void send_Image(void)
{
    // 检查摄像头是否完成一帧采集
    if (mt9v03x_finish_flag)
    {
        mt9v03x_finish_flag = 0;  // 清除采集完成标志
        
        // 配置摄像头信息（图像类型、缓冲区、宽高）
        seekfree_assistant_camera_information_config(
            SEEKFREE_ASSISTANT_MT9V03X,  // 摄像头类型：MT9V03X
            image_copy[0],               // 图像数据缓冲区
            MT9V03X_W,                   // 图像宽度
            MT9V03X_H                    // 图像高度
        );
        
        // 配置边线信息（X方向边线，可根据需求修改）
        seekfree_assistant_camera_boundary_config(
            X_BOUNDARY,                  // 边线方向：X方向
            MT9V03X_H,                   // 边线长度
            boder_L, boder_M, boder_R,   // 左/中/右边界
            NULL, NULL, NULL             // 预留参数
        );
        
        // 备份图像数据（避免发送时原始图像被更新导致撕裂）
        memcpy(image_copy[0], mt9v03x_image[0], MT9V03X_IMAGE_SIZE);  // 原始图像
        // memcpy(image_copy[0], image_two_value[0], MT9V03X_IMAGE_SIZE);  // 二值化图像（可选）
        
        // 发送图像数据到上位机
        seekfree_assistant_camera_send();
    }
}
#ifndef _ZF_ASSISTANT_H
#define _ZF_ASSISTANT_H

#include "zf_common_headfile.h" 

// ============================ 宏定义区 ============================
/**
 * 逐飞助手功能使能
 * 0: 关闭所有功能
 * 1: 开启图传功能
 * 2: 开启虚拟示波器功能
 */
#define ENABLE_ZFASS   0

/**
 * WIFI连接参数
 * 需根据实际上位机配置一致才能正常连接
 */
#define WIFI_SSID_TEST          "KAI"           // WIFI名称
#define WIFI_PASSWORD_TEST      "20050601"      // WIFI密码

// ============================ 函数声明区 ============================
/**
 * 逐飞助手初始化相关函数
 */
void My_ZFASSISTANT_Init(void);  // 初始化WIFI和逐飞助手接口

/**
 * 图像发送相关函数
 */
void send_Image(void);           // 发送摄像头图像数据到上位机

#endif
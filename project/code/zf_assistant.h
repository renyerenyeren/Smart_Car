#ifndef _ZF_ASSISTANT_H
#define _ZF_ASSISTANT_H

#include "zf_common_headfile.h" 

// ============================ �궨���� ============================
/**
 * ������ֹ���ʹ��
 * 0: �ر����й���
 * 1: ����ͼ������
 * 2: ��������ʾ��������
 */
#define ENABLE_ZFASS   0

/**
 * WIFI���Ӳ���
 * �����ʵ����λ������һ�²�����������
 */
#define WIFI_SSID_TEST          "KAI"           // WIFI����
#define WIFI_PASSWORD_TEST      "20050601"      // WIFI����

// ============================ ���������� ============================
/**
 * ������ֳ�ʼ����غ���
 */
void My_ZFASSISTANT_Init(void);  // ��ʼ��WIFI��������ֽӿ�

/**
 * ͼ������غ���
 */
void send_Image(void);           // ��������ͷͼ�����ݵ���λ��

#endif
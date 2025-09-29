#ifndef _ELEMENT_H_
#define _ELEMENT_H_

#include "zf_common_headfile.h" 

// ============================ �ⲿ���������� ============================
/** ����״̬��ر��� */
extern int Island_State;                     // ����״̬��־
extern int Left_Island_Flag;                 // �󻷵���־
extern int Right_Island_Flag;                // �һ�����־
extern int continuity_change_right_flag;      // �ұ߽������Ա�־������Ϊ0��
extern int continuity_change_left_flag;   // ��߽������Ա�־������Ϊ0��
extern int monotonicity_change_left_flag;     // ��߽絥���Ա仯��־�����仯Ϊ0��
extern int monotonicity_change_right_flag;    // �ұ߽絥���Ա仯��־�����仯Ϊ0��
extern int monotonicity_change_column[2];     // �����Ա仯�����꣨[0]��, [1]�У�
extern int monotonicity_change_line[2];       // �����Ա仯�����꣨[0]��, [1]�У�
extern int right_down_guai[2];                // ���¹յ����꣨[0]��, [1]�У�

// ============================ ���������� ============================
/**
 * ʼ���߼����غ���
 */
uint8 find_start_finish_line();

/**
 * ʮ�ּ����غ���
 */
void Cross_Detect();

/**
 * ���������غ���
 */
void Island_Detect();

#endif
    
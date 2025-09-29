#ifndef __ART_H
#define __ART_H

#include "zf_common_headfile.h" 

// ============================ 宏定义区 ============================
#define QUEUE_SIZE   80      // 数据缓存队列容量（存储ART识别结果）

// ============================ 全局变量声明区 ============================
extern int ART[QUEUE_SIZE];  // ART识别结果缓存队列（存储历史识别数据）
extern uint8_t head;         // 队列头指针（指向最早存入的数据位置）
extern uint8_t tail;         // 队列尾指针（指向最新数据的写入位置）

// ============================ 函数声明区 ============================
int8_t ART_Recognition(void);

#endif
#ifndef _MY_IMAGE_H_
#define _MY_IMAGE_H_

#include "zf_common_headfile.h"

// ============================ 宏定义区 ============================
// 图像尺寸与边界参数
#define middle                 (MT9V03X_W / 2)               // 图像中线X坐标
#define y_MIN                  (MT9V03X_H * 2 / 8)           // 边界矫正最小Y值限制
#define SHORTEST               (MT9V03X_H - 20)              // 最长白列最短值阈值

// 对比度阈值
#define Y_THRESHOLD            22                             // 纵向扫线对比度阈值
#define X_THRESHOLD            15                             // 横向扫线对比度阈值

// ============================ 枚举类型定义 ============================
/**
 * @brief 斜率计算模式枚举
 * 定义不同的边界线计算逻辑
 */
typedef enum {
    SLOPE_MIDDLE,            // 中线巡线模式
    SLOPE_LEFT,              // 左边线巡线模式
    SLOPE_RIGHT              // 右边线巡线模式
} SlopeMode;

// ============================ 全局变量声明 ============================
/** 图像特征点参数 */
extern int Right_Guai[2], Left_Guai[2];        // 拐点坐标 [0]Y坐标 [1]X坐标

/** 摄像头参数 */
extern uint16 OBJECT_light;                    // 分类摄像头曝光时间
extern uint16 COLOR_light;                     // 色块识别摄像头曝光时间
extern uint16 mt9v03X_light;                   // MT9V03X摄像头曝光时间

/** 边界检测参数 */
extern int Left_Lost_Flag[MT9V03X_H];          // 左丢线标志数组(1:丢线)
extern int Right_Lost_Flag[MT9V03X_H];         // 右丢线标志数组(1:丢线)
extern uint8 boder_L[MT9V03X_H - 5];           // 左边界数组(黄色)
extern uint8 boder_R[MT9V03X_H - 5];           // 右边界数组(绿色)
extern uint8 boder_M[MT9V03X_H - 5];           // 中线数组(蓝色)
extern uint8 boder_U[MT9V03X_W];               // 上边线数组(红色)
extern uint8 boder_correct[60];                // 边线矫正采样点

/** 检测状态参数 */
extern uint8 longest, index;                   // 最长白列Y坐标和X索引
extern uint8 shortest, index_shortest;         // 最短白列Y坐标和X索引
extern uint8 cross_flag, ramp_flag;            // 十字/坡道标志位(1:检测到)
extern uint8 lose_point_num_L, lose_point_num_R;// 左右边界丢线点数
extern uint8 Both_Lost_Time;                   // 双边丢线次数
extern int Boundry_Start_Left, Boundry_Start_Right; // 左右边界起始行

/** 控制模式参数 */
extern uint8 Slope_Mode;                       // 斜率计算模式
extern uint8 isopenResetM;                     // 中线重置开关
extern uint8 found_left, found_right;          // 边界找到标志

/** 环岛检测相关 */
extern int start_column_Right_Island;          // 右环岛起始列

/** 图像数据参数 */
extern uint8 white_value;                       // 白色参考阈值

// ============================ 函数声明 ============================
/**
 * 初始化函数
 */
void my_image_init(void);                       // 图像传感器初始化

/**
 * 图像处理函数
 */
uint8 find_white_point(uint8 image_array[][188]); // 计算白色参考阈值
void find_longest(uint8* longest, uint8* index);   // 查找最长白列
void find_middle(void);                           // 计算中线和边界

/**
 * 辅助函数
 */
void show_boder_line(void);                     // 显示边界线(调试用)

/**
 * 计算斜率函数
 */
 int16 slope(uint8 slope_mode);                  // 计算图像斜率

#endif
    
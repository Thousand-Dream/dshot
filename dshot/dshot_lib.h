/* ==============================================================================
 * @file    dshot_lib.h
 * @brief   DShot & Bidirectional DShot Library (PWM + DMA Method)
 * @author  芙琳娜 (Furina)
 * @note    Designed for STM32H7 (D-Cache aligned)
 * ==============================================================================*/
#ifndef DSHOT_LIB_H
#define DSHOT_LIB_H

#include "stdint.h"
#include "stm32h7xx_hal.h"

// 宏定义：最大支持的电机数量
#define DSHOT_PORT_COUNT 4

// 电机状态数据结构
typedef struct {
    uint16_t throttle;      // 当前油门值 (48~2047)
    uint32_t erpm;          // 电调返回的电转速
    uint32_t rpm;           // 实际转速 (根据极对数计算，这里默认为 erpm/7)
    uint32_t error_count;   // CRC 或解码错误计数
} DSHOT_Motor_TypeDef;

extern DSHOT_Motor_TypeDef dshot_motors[DSHOT_PORT_COUNT];

///* ================== 用户配置区 ================== */
//// DShot600 的定时器参数 (基于你的原代码，假设 ARR=916)
//#define DSHOT_BIT_0 344  // 37.5% 占空比
//#define DSHOT_BIT_1 687  // 75% 占空比
//#define DSHOT_PWM_BUFFER_SIZE 18  // 16位数据 + 2位拉低复位
//#define DSHOT_GCR_BUFFER_SIZE 100 // 接收 GCR 数据包的边沿数量
///* ================================================ */
/* ================== 用户配置区 ================== */
// DShot600 的定时器参数 (基于 275MHz APB2 Timer Clock, ARR=457)
#define DSHOT_BIT_0 172  // 37.5% 占空比
#define DSHOT_BIT_1 344  // 75% 占空比
#define DSHOT_PWM_BUFFER_SIZE 18  // 16位数据 + 2位拉低复位
#define DSHOT_GCR_BUFFER_SIZE 100 // 接收 GCR 数据包的边沿数量
/* ================================================ */

// 核心 API
extern void DSHOT_Init(TIM_HandleTypeDef *htim);
extern void DSHOT_Send_Packet(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4, uint8_t telemetry);
extern void DSHOT_Switch_To_Receive(void);
extern uint8_t DSHOT_Decode_Telemetry(void);
extern void Furina_Arm_ESCs(void);

#endif /* DSHOT_LIB_H */

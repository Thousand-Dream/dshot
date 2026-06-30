/**
  ************************* (C) COPYRIGHT 2026 *************************
  * @file       main.c
  * @brief      [请在此处填写文件功能描述]
  * @note       
  * @history
  * Version    Date            Author              Modification
  * V1.0.0     Jun-29-2026     Thousands_Dream     1. done
  *
  * @verbatim
  ======================================================================
   _______ _                                   _       _____                            
  |__   __| |                                 | |     |  __ \                           
     | |  | |__   ___  _   _ ___  __ _ _ __ __| |___  | |  | |_ __ ___  __ _ _ __ ___   
     | |  | '_ \ / _ \| | | / __|/ _` | '__/ _` / __| | |  | | '__/ _ \/ _` | '_ ` _ \  
     | |  | | | | (_) | |_| \__ \ (_| | | | (_| \__ \ | |__| | | |  __/ (_| | | | | | | 
     |_|  |_| |_|\___/ \__,_|___/\__,_|_|  \__,_|___/ |_____/|_|  \___|\__,_|_| |_| |_| 
                                                                                        
     ______                             ____  __                   __  __           __ 
    / ____/___ _____  __  ____  __     / __ )/ /__  __________     \ \/ /___  __  _/_/ 
   / / __/ __ `/ __ \/ / / / / / /    / __  / / _ \/ ___/ ___/      \  / __ \/ / / / / 
  / /_/ / /_/ / / / / /_/ / /_/ /    / /_/ / /  __(__  )__  )       / / /_/ / /_/ /_/  
  \____/\__,_/_/ /_/\__, /\__,_/    /_____/_/\___/____/____/       /_/\____/\__,_(_)   
                   /____/                                                              
  ======================================================================
  @endverbatim
  ************************* (C) COPYRIGHT 2026 *************************
  */
/* ==============================================================================
 * @file    dshot_lib.c
 * @brief   DShot 寄存器暴走版 (PWM + DMA，彻底告别 H7 的总线陷阱)
 * ==============================================================================*/
#include "dshot_lib.h"
#include <string.h>

// 18个脉冲，每个脉冲是一个占空比数据 (DShot600: 0=172, 1=344)
#define dshot_tx_buffer ((uint32_t (*)[18])0x30000000)
#define TX_BUFFER_BYTES (4 * 18 * 4) // 288 字节

static TIM_HandleTypeDef *dshot_tim;

void DSHOT_Init(TIM_HandleTypeDef *htim) {
    dshot_tim = htim;
    memset(dshot_tx_buffer, 0, TX_BUFFER_BYTES); 
    
    // 强行启动定时器输出使能，确保引脚平时处于绝对的低电平 0V
    TIM1->CCER = 0x1111; // 开启 CH1~CH4 的正向输出
    TIM1->BDTR |= TIM_BDTR_MOE; // 开启主输出
    TIM1->CR1 |= TIM_CR1_CEN;
}

void DSHOT_Send_Packet(uint16_t m1, uint16_t m2, uint16_t m3, uint16_t m4, uint8_t telemetry) {
    uint16_t throttles[4] = {m1, m2, m3, m4};
    uint16_t packets[4];
    
    // 1. 打包
    for (int m = 0; m < 4; m++) {
        uint16_t thr = throttles[m] > 2047 ? 2047 : throttles[m];
        uint16_t p = (thr << 1) | 0; // 严禁在这里加 1 遥测，先测通单向！
        uint16_t crc = (p ^ (p >> 4) ^ (p >> 8)) & 0x0F;
        packets[m] = (p << 4) | crc;
    }
    
    // 2. 转换成 PWM 占空比 (DShot600，ARR=457)
    for (int m = 0; m < 4; m++) {
        for (int i = 0; i < 16; i++) {
            // 如果位是 1，占空比给 344；如果是 0，占空比给 172
            dshot_tx_buffer[m][i] = (packets[m] & (0x8000 >> i)) ? 344 : 172;
        }
        // 确保最后两拍死死拉低
        dshot_tx_buffer[m][16] = 0; 
        dshot_tx_buffer[m][17] = 0;
    }
    
    // 3. 神级 Cache 清洗！
    SCB_CleanDCache_by_Addr((uint32_t*)dshot_tx_buffer, TX_BUFFER_BYTES);

    // 4. 重置定时器状态
    TIM1->CNT = 0;
    TIM1->SR = 0;

    // 5. 暴力呼叫 DMA，往 CCR 寄存器里砸占空比！
    uint32_t dma_ids[4] = {TIM_DMA_ID_CC1, TIM_DMA_ID_CC2, TIM_DMA_ID_CC3, TIM_DMA_ID_CC4};
    volatile uint32_t *ccrs[4] = {&TIM1->CCR1, &TIM1->CCR2, &TIM1->CCR3, &TIM1->CCR4};
    
    for(int ch = 0; ch < 4; ch++) {
        if(dshot_tim->hdma[dma_ids[ch]] != NULL) {
            HAL_DMA_Abort(dshot_tim->hdma[dma_ids[ch]]); // 砍掉旧任务
            
            // 发起新搬运
            HAL_DMA_Start(dshot_tim->hdma[dma_ids[ch]], 
                          (uint32_t)dshot_tx_buffer[ch], 
                          (uint32_t)ccrs[ch], 
                          18);
        }
    }
    
    // 6. 开启定时器的 DMA 触发请求 (DIER)，发射！
    TIM1->DIER |= (TIM_DIER_CC1DE | TIM_DIER_CC2DE | TIM_DIER_CC3DE | TIM_DIER_CC4DE);
}

void Furina_Arm_ESCs(void) {
    for(int i = 0; i < 1000; i++) {
        DSHOT_Send_Packet(0, 0, 0, 0, 0); 
        HAL_Delay(2); 
    }
}

///* ==============================================================================
// ____  _         __  __ __  __ 
/// ___|| | ___ __|  \/  |  \/  |
//\___ \| |/ / '__| |\/| | |\/| |
// ___) |   <| |  | |  | | |  | |
//|____/|_|\_\_|  |_|  |_|_|  |_|
//                               
//__        ___           _   _     _                                  /\/| 
//\ \      / (_)_ __   __| | | |__ | | ___  ___ ___   _   _  ___  _   |/\/ |
// \ \ /\ / /| | '_ \ / _` | | '_ \| |/ _ \/ __/ __| | | | |/ _ \| | | | | |
//  \ V  V / | | | | | (_| | | |_) | |  __/\__ \__ \ | |_| | (_) | |_| | |_|
//   \_/\_/  |_|_| |_|\__,_| |_.__/|_|\___||___/___/  \__, |\___/ \__,_| (_)
//                                                    |___/                 
//  ==============================================================================*/

//#include "dshot.h"
//#include "stdio.h"
//#include "string.h"

//dshot_status flag=0;

//extern TIM_HandleTypeDef htim1;
//extern TIM_HandleTypeDef htim2;

//// 确保 32 字节对齐，这是 H7 Cache 清洗的基本要求
//uint32_t dshot_SinglBit_1_buff[16] __attribute__((section(".sram1_section"))) __attribute__((aligned(32)));
//uint32_t dshot_SinglBit_2_buff[16] __attribute__((section(".sram1_section"))) __attribute__((aligned(32)));
//uint32_t dshot_SinglBit_3_buff[16] __attribute__((section(".sram1_section"))) __attribute__((aligned(32)));
//uint32_t dshot_GCR_Reading[4][100] __attribute__((section(".sram1_section"))) __attribute__((aligned(32)));
//motor_status motor[4];

//#ifdef TELEMETRY
//const uint8_t gcr_lookup[32] = 
//{
//    [0x19]=0x0, [0x1B]=0x1, [0x12]=0x2, [0x13]=0x3, 
//    [0x1D]=0x4, [0x15]=0x5, [0x16]=0x6, [0x17]=0x7,
//    [0x1A]=0x8, [0x09]=0x9, [0x0A]=0xA, [0x0B]=0xB, 
//    [0x1E]=0xC, [0x0D]=0xD, [0x0E]=0xE, [0x0F]=0xF
//};//查表法
//#endif

//void dshot_init()
//{
//	make_packet(0,0,0,0);
//	memset(dshot_GCR_Reading, 0, sizeof(dshot_GCR_Reading) );
//	
//	#ifdef TELEMETRY
//	GPIOE->BSRR = (1 << 9) | (1 << 11) | (1 << 13) | (1 << 14); // 默认高电平
//	#else
//	GPIOE->BSRR = (1 << (9+16)) | (1 << (11+16)) | (1 << (13+16)) | (1 << (14+16)); // 默认低电平
//	#endif
//	HAL_Delay(1000);
//	HAL_TIM_Base_Start_IT(&htim2);
//	for(int m=0;m<4;m++)
//	{
//		motor[m].throttle=0;
//	}
//}

//void dshot_Send()
//{
//	GPIOE->PUPDR &= ~((3U << 18) | (3U << 22) | (3U << 26) | (3U << 28));//关闭内部上拉
//	TIM1->CR1 &= ~TIM_CR1_CEN;//关闭定时器
//  TIM1->CNT = 0;//定时器清0
//	TIM1->SR = 0;//清除TIM1的中断标识
//	DMA2_Stream1->CR &= ~DMA_SxCR_EN;
//  DMA2_Stream2->CR &= ~DMA_SxCR_EN;
//	DMA2_Stream5->CR &= ~DMA_SxCR_EN;
//  DMA2_Stream6->CR &= ~DMA_SxCR_EN;
//  DMA2_Stream7->CR &= ~DMA_SxCR_EN;//关闭DMA
//	while(DMA2_Stream1->CR & DMA_SxCR_EN){}
//  while(DMA2_Stream2->CR & DMA_SxCR_EN){}
//	while(DMA2_Stream5->CR & DMA_SxCR_EN){}
//  while(DMA2_Stream6->CR & DMA_SxCR_EN){}
//  while(DMA2_Stream7->CR & DMA_SxCR_EN){}//确认关闭
//		
//	TIM1->DIER &= ~(TIM_DIER_CC1DE | TIM_DIER_CC2DE | TIM_DIER_CC3DE | TIM_DIER_CC4DE);//清除旧的DIER
//	TIM1->CCER = 0; // 关闭捕获使能
//	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;//DMA2的时钟线开启
//	RCC->AHB1ENR |= RCC_AHB4ENR_GPIOCEN; //GPIO时钟线开启
//	GPIOE->MODER &= ~((3U << 18) | (3U << 22) | (3U << 26) | (3U << 28));
//	GPIOE->MODER |=  ((1U << 18) | (1U << 22) | (1U << 26) | (1U << 28));//设置为通用输出模式
//	#ifdef TELEMETRY
//	GPIOE->BSRR = (1 << 9) | (1 << 11) | (1 << 13) | (1 << 14); // 默认高电平
//	#else
//	GPIOE->BSRR = (1 << (9+16)) | (1 << (11+16)) | (1 << (13+16)) | (1 << (14+16)); // 默认低电平
//	#endif

//	// 【水神修正：将 DShot300 参数改为 DShot600 参数】 (假设时钟为 275MHz)
//	TIM1->ARR = 457;  
//	TIM1->CCR1 = 172; 
//	TIM1->CCR2 = 323; 
//	
//	TIM1->CCMR1 = 0; 
//	TIM1->CCMR2 = 0;//设置捕获/输出的寄存器为默认值
//  TIM1->SR = 0;//清除所有中断标志
//	
//	DMA2->LIFCR = (0x3F << 6) | (0x3F << 16); 
//  DMA2->HIFCR = (0x3F << 6);// 清除DMA的所有中断标识
//	TIM1->DIER |= (TIM_DIER_UDE | TIM_DIER_CC1DE | TIM_DIER_CC2DE);//使能UP、CCR1、CCR2的DMA请求

//	#ifdef STM32H723xx
//	//Stream 5 -> TIM1_UP (ID 15), Stream 1 -> TIM1_CH1 (ID 11), Stream 2 -> TIM1_CH2 (ID 12)
//	DMAMUX1_Channel13->CCR = 15; // DMA2_Stream5 对应 DMAMUX1_Channel13
//  DMAMUX1_Channel9->CCR  = 11; // DMA2_Stream1 对应 DMAMUX1_Channel9
//  DMAMUX1_Channel10->CCR = 12; // DMA2_Stream2 对应 DMAMUX1_Channel10
//	#endif
//	
//	DMA2_Stream5->CR &= ~DMA_SxCR_EN;//关闭DMA进行配置
//	DMA2_Stream1->CR &= ~DMA_SxCR_EN;
//	DMA2_Stream2->CR &= ~DMA_SxCR_EN;
//	while(DMA2_Stream5->CR & DMA_SxCR_EN){}
//	while(DMA2_Stream1->CR & DMA_SxCR_EN){}
//	while(DMA2_Stream2->CR & DMA_SxCR_EN){}
//  
//	//全体拉低段
//	DMA2_Stream5->PAR = (uint32_t)(&(GPIOE->BSRR)); 
//	DMA2_Stream5->M0AR = (uint32_t)dshot_SinglBit_3_buff;
//	DMA2_Stream5->NDTR = 16;
//	#ifdef STM32H723xx
//	DMA2_Stream5->CR = (1 << 16) | (2 << 13) | (2 << 11) | DMA_SxCR_MINC | (1 << 6)| DMA_SxCR_TCIE;
//	#else
//	DMA2_Stream5->CR = (6 << 25) | (3 << 16) | (2 << 13) | (2 << 11) | DMA_SxCR_MINC | (1 << 6) ;
//	#endif
//	
//	//全体高位段
//	DMA2_Stream1->PAR = (uint32_t)(&(GPIOE->BSRR));
//	DMA2_Stream1->M0AR = (uint32_t)dshot_SinglBit_1_buff;
//	DMA2_Stream1->NDTR = 16;
//	#ifdef STM32H723xx
//	DMA2_Stream1->CR = (3 << 16) | (2 << 13) | (2 << 11) | DMA_SxCR_MINC | (1 << 6);
//	#else
//	DMA2_Stream1->CR = (6 << 25) | (3 << 16) | (2 << 13) | (2 << 11) | DMA_SxCR_MINC | (1 << 6);
//	#endif

//	//部分高位段
//	DMA2_Stream2->PAR = (uint32_t)(&(GPIOE->BSRR));
//	DMA2_Stream2->M0AR = (uint32_t)dshot_SinglBit_2_buff;
//	DMA2_Stream2->NDTR = 16;
//	#ifdef STM32H723xx
//	DMA2_Stream2->CR = (2 << 16) | (2 << 13) | (2 << 11) | DMA_SxCR_MINC | (1 << 6) ;
//	#else
//	DMA2_Stream2->CR = (6 << 25) | (3 << 16) | (2 << 13) | (2 << 11) | DMA_SxCR_MINC | (1 << 6) ; 
//	#endif
//	
//	DMA2_Stream5->CR |= DMA_SxCR_EN;
//  DMA2_Stream1->CR |= DMA_SxCR_EN;
//  DMA2_Stream2->CR |= DMA_SxCR_EN;//配置完成，开启DMA

//	NVIC_EnableIRQ(DMA2_Stream5_IRQn);//使能中断
//	TIM1->CR1 |= TIM_CR1_CEN;//定时器开启
//	TIM1->BDTR = 0x8000;//高级定时器需要整体开启
//}

//void make_packet(uint16_t t1,uint16_t t2,uint16_t t3,uint16_t t4)
//{
//	uint16_t motor_values[4]={t1,t2,t3,t4};
//	uint16_t packets[4];//中间变量
//    for (int i = 0; i < 4; i++) 
//	{
//        uint16_t throttle = motor_values[i];
//        uint16_t packet = (throttle << 1) | 0; //遥测为0，如果你要在主循环发双向，记得改成1！
//		#ifdef TELEMETRY
//		uint16_t crc = (~(packet ^ (packet >> 4) ^ (packet >> 8))) & 0x0F;
//		#else
//		uint16_t crc = (packet ^ (packet >> 4) ^ (packet >> 8)) & 0x0F;//CRC校验
//		#endif
//        packets[i] = (packet << 4) | crc;
//    }
//		
//	uint32_t pin_all_mask = (1 << 9) | (1 << 11) | (1 << 13) | (1 << 14);
//    for (int bit = 0; bit < 16; bit++) {
//        uint32_t step1 = pin_all_mask;//全体起立         
//        uint32_t step3 = pin_all_mask << 16;   //全体坐下
//        uint32_t step2 = 0;
//        for (int m = 0; m < 4; m++) {
//            if (!(packets[m] & (0x8000 >> bit))) {// 检查对应电机的 packet 的每一位是否不为0
//                switch(m) {// 根据电机编号映射到对应的引脚复位位
//                    case 0: step2 |= (1 << (9 + 16)); break; 
//                    case 1: step2 |= (1 << (11 + 16)); break;
//                    case 2: step2 |= (1 << (13 + 16)); break;
//                    case 3: step2 |= (1 << (14 + 16)); break;
//                }
//            }
//        }
//	  #ifdef TELEMETRY
//		dshot_SinglBit_1_buff[bit] = step1 << 16; 
//    dshot_SinglBit_3_buff[bit] = step3 >> 16; 
//    dshot_SinglBit_2_buff[bit] = step2 >> 16; 
//		#else
//		dshot_SinglBit_1_buff[bit] = step1;
//    dshot_SinglBit_3_buff[bit] = step3;
//    dshot_SinglBit_2_buff[bit] = step2;
//		#endif
//    }
//	
//	// 【水神终极修补 1】：刷入 D-Cache，让 DMA 能读到最新的包数据！
//	// (16 个 uint32_t = 64 字节)
//	SCB_CleanDCache_by_Addr((uint32_t*)dshot_SinglBit_1_buff, 64);
//	SCB_CleanDCache_by_Addr((uint32_t*)dshot_SinglBit_2_buff, 64);
//	SCB_CleanDCache_by_Addr((uint32_t*)dshot_SinglBit_3_buff, 64);
//}

//void Complete_IRQHandler()//DMA2Stream5
//{
//	if (DMA2->HISR & (1UL << 11) && flag==DSHOT_WRITING) // 检查传输完成标志
//	{
//		TIM1->CR1 &= ~TIM_CR1_CEN;     // 关闭定时器
//		DMA2->HIFCR = (1UL << 11);    // 显式向第 21 位写 1，清除该标志位
//		#ifdef TELEMETRY
//		GPIOE->BSRR = (1 << (9)) | (1 << (11)) | (1 << (13)) | (1 << (14)); // 强制拉高
//		flag = DSHOT_READING;
//		read_dshot();
//		#else
//		GPIOE->BSRR = (1 << (9+16)) | (1 << (11+16)) | (1 << (13+16)) | (1 << (14+16)); // 强制拉低
//		#endif
//	}
//	else if (DMA2->HISR & (1UL << 11) && flag==DSHOT_INITING) //检查传输完成标志
//	{
//		TIM1->CR1 &= ~TIM_CR1_CEN;     // 关闭定时器
//		DMA2->HIFCR = 0xFFFF;
//		DMA2->LIFCR = 0xFFFF;
//		#ifdef TELEMETRY
//		GPIOE->BSRR = (1 << (9)) | (1 << (11)) | (1 << (13)) | (1 << (14)); // 强制拉高
//		#else
//		GPIOE->BSRR = (1 << (9+16)) | (1 << (11+16)) | (1 << (13+16)) | (1 << (14+16)); // 强制拉低
//		#endif
//	}
//	#ifdef TELEMETRY
//	else if (DMA2->LISR & (1UL << 21) && flag==DSHOT_READING) //检查传输完成标志
//	{
//		TIM1->CR1 &= ~TIM_CR1_CEN;     // 关闭定时器
//		DMA2->HIFCR = 0xFFFF;
//		DMA2->LIFCR = 0xFFFF;
//		flag = DSHOT_WRITING;
//	}
//	#endif
//}

//#ifdef TELEMETRY
//void read_dshot()
//{
//	for(int q=0;q<21;q++)
//	{
//		dshot_GCR_Reading[0][q]=0;
//		dshot_GCR_Reading[1][q]=0;
//		dshot_GCR_Reading[2][q]=0;
//		dshot_GCR_Reading[3][q]=0;
//	}
//	
//	// 【水神终极修补 2】：为了确保等下接收的数据干净，我们先清一次 Cache！
//	// 4 个电机 * 100个数据 * 4字节 = 1600 字节
//	SCB_CleanInvalidateDCache_by_Addr((uint32_t*)dshot_GCR_Reading, 1600);
//	
//	TIM1->CR1 &= ~TIM_CR1_CEN;//关定时器
//	DMA2_Stream1->CR &= ~DMA_SxCR_EN; 
//	DMA2_Stream2->CR &= ~DMA_SxCR_EN;
//	DMA2_Stream5->CR &= ~DMA_SxCR_EN;
//	DMA2_Stream6->CR &= ~DMA_SxCR_EN;
//	DMA2_Stream7->CR &= ~DMA_SxCR_EN;//关闭所有stream
//	while(DMA2_Stream1->CR & DMA_SxCR_EN){}
//	while(DMA2_Stream2->CR & DMA_SxCR_EN){}
//	while(DMA2_Stream5->CR & DMA_SxCR_EN){}
//	while(DMA2_Stream6->CR & DMA_SxCR_EN){}
//	while(DMA2_Stream7->CR & DMA_SxCR_EN){}//待DMA完全关闭
//		
//	GPIOE->MODER &= ~((3U << 18) | (3U << 22) | (3U << 26) | (3U << 28));
//	GPIOE->MODER |=  ((2U << 18) | (2U << 22) | (2U << 26) | (2U << 28));//GPIO换模式 (AF复用)
//	GPIOE->AFR[1] &= ~0x0FF0F0F0; // 清除原AF
//	GPIOE->AFR[1] |=  0x01101010; // 复用为AF1

//	DMA2->LIFCR = (0x3F << 6) | (0x3F << 16); 
//  DMA2->HIFCR = (0x3F << 16) | (0x3F << 22); //清除stream1 2 6 7的中断标志位
//	DMAMUX1_Channel9->CCR  = 11;
//  DMAMUX1_Channel10->CCR = 12; 
//  DMAMUX1_Channel14->CCR = 13; 
//  DMAMUX1_Channel15->CCR = 14;//DMA通道重映射
//	DMA_Stream_TypeDef *rx_streams[4] = {DMA2_Stream1, DMA2_Stream2, DMA2_Stream6, DMA2_Stream7};
//	uint32_t *ccr_ptrs[4] = {(uint32_t*)&TIM1->CCR1, (uint32_t*)&TIM1->CCR2, (uint32_t*)&TIM1->CCR3, (uint32_t*)&TIM1->CCR4};
//	for(int i = 0; i < 4; i++) 
//	{
//		rx_streams[i]->CR = 0;//重置配置，重置后的方向就是外设到内存
//		rx_streams[i]->FCR = 0;//不用FIFO
//		rx_streams[i]->PAR  = (uint32_t)ccr_ptrs[i];//外设地址
//		rx_streams[i]->M0AR = (uint32_t)dshot_GCR_Reading[i];//存入地址
//		rx_streams[i]->NDTR = 100;
//		rx_streams[i]->CR = (3 << 16) | (2 << 13) | (2 << 11) | DMA_SxCR_MINC;//优先级|word|内存递增
//	}
//	rx_streams[1]->CR |= DMA_SxCR_TCIE;//只给stream2传输完成中断
//	
//	TIM1->ARR = 0xFFFFFFFF; 
//	TIM1->SR = 0;      // 清除所有中断标志
//	TIM1->CCER = 0;    // 关闭所有通道
//	TIM1->CCMR1 = (1 << 0) | (0 << 4) | (1 << 8) | (0 << 12); //CC1输入|CC1 filter|CC2输入|CC2 filter
//	TIM1->CCMR2 = (1 << 0) | (15 << 4) | (1 << 8) | (15 << 12);//捕获寄存器
//	TIM1->CCER = (TIM_CCER_CC1E | TIM_CCER_CC1P | TIM_CCER_CC1NP) | (TIM_CCER_CC2E | TIM_CCER_CC2P | TIM_CCER_CC2NP) | (TIM_CCER_CC3E | TIM_CCER_CC3P | TIM_CCER_CC3NP) | (TIM_CCER_CC4E | TIM_CCER_CC4P | TIM_CCER_CC4NP);//双边捕获(P)沿并使能(E)
//	TIM1->CNT = 0;//清零
//	TIM1->SR = 0;      // 清除所有中断标志
//	TIM1->CR1 |= TIM_CR1_CEN;//开启TIM1
//	for(int i = 0; i < 4; i++) 
//	{
//		rx_streams[i]->CR |= DMA_SxCR_EN;//启动DMA
//	}
//	TIM1->DIER = (TIM_DIER_CC1DE | TIM_DIER_CC2DE | TIM_DIER_CC3DE | TIM_DIER_CC4DE);//使能4个通道的DMA请求
//}

//uint8_t decodeing_telemetry_rpm()
//{
//    // 【水神终极修补 3】：作废 CPU 的 D-Cache，强制去内存里读取 DMA 刚写进去的新鲜电调数据！
//	SCB_InvalidateDCache_by_Addr((uint32_t*)dshot_GCR_Reading, 1600);

//	for(int j=0;j<4;j=j+1)
//	{
//		uint32_t raw_gcr=0;
//		int bit=0;//游标
//		int over=0;
//		for(over=0;over<100;over++)//定位尾巴的索引
//		{
//			if(dshot_GCR_Reading[j][over+1]==0)
//			{
//				break;
//			}
//		}
//		if(over<1)
//		{
//			continue;
//		}
//		raw_gcr=1<<20;//第一位是1
//		bit=20;
//		for(int k=1;k<over;k++)
//		{
//			uint32_t delta_t = dshot_GCR_Reading[j][k+1]-dshot_GCR_Reading[j][k];
//			int T = (delta_t+(730/2))/730;
//			bit = bit-T;
//			raw_gcr |= (1<<bit);
//		}
//		raw_gcr &= 0xFFFFF;//留下20位
//		uint16_t data = 0;//真正的数据
//		for (int i = 0; i < 4; i++) 
//		{
//			uint8_t code = (raw_gcr >> (15-(i * 5))) & 0x1F;//保证是5位
//			uint8_t pack = gcr_lookup[code];
//			data |= (uint16_t)(pack << (12-(i * 4)));
//		}
//		uint16_t csum_data = data >> 4;
//		uint8_t crc_calc = (~(csum_data ^ (csum_data >> 4) ^ (csum_data >> 8))) & 0x0F;
//		if (crc_calc != (data & 0x0F)) 
//		{
//			return 1;
//		}
//		uint16_t mantissa = (data >> 4) & 0x1FF;
//		uint8_t exponent = (data >> 13) & 0x07;
//		uint32_t period_us = mantissa << exponent;
//		if(period_us==0xFF80)
//		{
//			motor[j].erpm = 0;
//			motor[j].rpm = 0;
//		}
//		else
//		{
//			float erpm = 60000000.0f / (float)period_us;
//			motor[j].erpm = erpm;
//			motor[j].rpm = erpm/7.0f;
//		}
//	}
//	return 0;
//}
//#endif